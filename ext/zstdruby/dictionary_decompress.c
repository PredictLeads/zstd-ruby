#include "common.h"

struct dictionary_decompress_t {
    ZSTD_DCtx* ctx;
};

static void
dictionary_decompress_mark(void *p)
{
    struct dictionary_decompress_t *dd = p;
}

static void
dictionary_decompress_free(void *p)
{
    struct dictionary_decompress_t *dd = p;

    ZSTD_DCtx* ctx = dd->ctx;
    if (ctx != NULL) {
        ZSTD_freeDCtx(ctx);
    }

    xfree(dd);
}

static size_t
dictionary_decompress_memsize(const void *p)
{
    return sizeof(struct dictionary_decompress_t);
}

#ifdef HAVE_RB_GC_MARK_MOVABLE
static void
dictionary_decompress_compact(void *p)
{
    struct dictionary_decompress_t *dd = p;
}
#endif

static const rb_data_type_t dictionary_decompress_type = {
  "dictionary_decompress",
  {
    dictionary_decompress_mark,
    dictionary_decompress_free,
    dictionary_decompress_memsize,
#ifdef HAVE_RB_GC_MARK_MOVABLE
    dictionary_decompress_compact,
#endif
  },
  0, 0, RUBY_TYPED_FREE_IMMEDIATELY
};


static VALUE
rb_dictionary_decompress_allocate(VALUE klass)
{
    struct dictionary_decompress_t* dd;
    VALUE obj = TypedData_Make_Struct(klass, struct dictionary_decompress_t, &dictionary_decompress_type, dd);
    dd->ctx = NULL;
    return obj;
}


static VALUE
rb_dictionary_decompress_initialize(int argc, VALUE *argv, VALUE obj)
{
    VALUE kwargs;
    rb_scan_args(argc, argv, "00:", &kwargs);

    struct dictionary_decompress_t* dd;
    TypedData_Get_Struct(obj, struct dictionary_decompress_t, &dictionary_decompress_type, dd);

    ZSTD_DCtx* const ctx = ZSTD_createDCtx();
    if (ctx == NULL) {
        rb_raise(rb_eRuntimeError, "%s", "ZSTD_createDCtx error");
    }

    dd->ctx = ctx;

    set_decompress_params(ctx, kwargs);

    return obj;
}


static VALUE decompress_buffered(ZSTD_DCtx* dctx, const char* input_data, size_t input_size)
{
  ZSTD_inBuffer input = { input_data, input_size, 0 };
  VALUE result = rb_str_new(0, 0);

  while (input.pos < input.size) {
    ZSTD_outBuffer output = { NULL, 0, 0 };
    output.size += ZSTD_DStreamOutSize();
    VALUE output_string = rb_str_new(NULL, output.size);
    output.dst = RSTRING_PTR(output_string);

    size_t ret = zstd_stream_decompress(dctx, &output, &input, false);
    if (ZSTD_isError(ret)) {
      ZSTD_freeDCtx(dctx);
      rb_raise(rb_eRuntimeError, "%s: %s", "ZSTD_decompressStream failed", ZSTD_getErrorName(ret));
    }
    rb_str_cat(result, output.dst, output.pos);
    RB_GC_GUARD(output_string);
  }
  ZSTD_freeDCtx(dctx);
  return result;
}


static VALUE
rb_dictionary_decompress_decompress(VALUE obj, VALUE input_value)
{

    struct dictionary_decompress_t* dd;
    TypedData_Get_Struct(obj, struct dictionary_decompress_t, &dictionary_decompress_type, dd);

    StringValue(input_value);
    char* input_data = RSTRING_PTR(input_value);
    size_t input_size = RSTRING_LEN(input_value);

    unsigned long long const uncompressed_size = ZSTD_getFrameContentSize(input_data, input_size);
    if (uncompressed_size == ZSTD_CONTENTSIZE_ERROR) {
        rb_raise(rb_eRuntimeError, "%s: %s", "not compressed by zstd", ZSTD_getErrorName(uncompressed_size));
    }
    // ZSTD_decompressStream may be called multiple times when ZSTD_CONTENTSIZE_UNKNOWN, causing slowness.
    // Therefore, we will not standardize on ZSTD_decompressStream
    if (uncompressed_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        return decompress_buffered(dd->ctx, input_data, input_size);
    }

    VALUE output = rb_str_new(NULL, uncompressed_size);
    char* output_data = RSTRING_PTR(output);

    size_t const decompress_size = zstd_decompress(dd->ctx, output_data, uncompressed_size, input_data, input_size, false);
    if (ZSTD_isError(decompress_size)) {
        rb_raise(rb_eRuntimeError, "%s: %s", "decompress error", ZSTD_getErrorName(decompress_size));
    }

    return output;
}


extern VALUE rb_mZstd, cDictionaryDecompress;
void
zstd_ruby_dictionary_decompress_init(void)
{
    VALUE cDictionaryDecompress = rb_define_class_under(rb_mZstd, "DictionaryDecompress", rb_cObject);
    rb_define_alloc_func(cDictionaryDecompress, rb_dictionary_decompress_allocate);
    rb_define_method(cDictionaryDecompress, "initialize", rb_dictionary_decompress_initialize, -1);
    rb_define_method(cDictionaryDecompress, "decompress", rb_dictionary_decompress_decompress, 1);
}
