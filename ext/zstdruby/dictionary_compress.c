#include "common.h"

struct dictionary_compress_t {
    ZSTD_CCtx* ctx;
};

static void
dictionary_compress_mark(void *p)
{
    struct dictionary_compress_t *dc = p;
}

static void
dictionary_compress_free(void *p)
{
    struct dictionary_compress_t *dc = p;

    ZSTD_CCtx* ctx = dc->ctx;
    if (ctx != NULL) {
        ZSTD_freeCCtx(ctx);
    }

    xfree(dc);
}

static size_t
dictionary_compress_memsize(const void *p)
{
    return sizeof(struct dictionary_compress_t);
}

#ifdef HAVE_RB_GC_MARK_MOVABLE
static void
dictionary_compress_compact(void *p)
{
    struct dictionary_compress_t *dc = p;
}
#endif

static const rb_data_type_t dictionary_compress_type = {
  "dictionary_compress",
  {
    dictionary_compress_mark,
    dictionary_compress_free,
    dictionary_compress_memsize,
#ifdef HAVE_RB_GC_MARK_MOVABLE
    dictionary_compress_compact,
#endif
  },
  0, 0, RUBY_TYPED_FREE_IMMEDIATELY
};


static VALUE
rb_dictionary_compress_allocate(VALUE klass)
{
    struct dictionary_compress_t* dc;
    VALUE obj = TypedData_Make_Struct(klass, struct dictionary_compress_t, &dictionary_compress_type, dc);
    dc->ctx = NULL;
    return obj;
}


static VALUE
rb_dictionary_compress_initialize(int argc, VALUE *argv, VALUE obj)
{
    VALUE kwargs;
    VALUE compression_level_value;
    rb_scan_args(argc, argv, "00:", &kwargs);

    struct dictionary_compress_t* dc;
    TypedData_Get_Struct(obj, struct dictionary_compress_t, &dictionary_compress_type, dc);

    ZSTD_CCtx* const ctx = ZSTD_createCCtx();
    if (ctx == NULL) {
        rb_raise(rb_eRuntimeError, "%s", "ZSTD_createCCtx error");
    }

    dc->ctx = ctx;

    compression_level_value = Qnil;

    set_compress_params(ctx, compression_level_value, kwargs);

    return obj;
}


static VALUE
rb_dictionary_compress_compress(VALUE obj, VALUE input_value)
{

    struct dictionary_compress_t* dc;
    TypedData_Get_Struct(obj, struct dictionary_compress_t, &dictionary_compress_type, dc);

    StringValue(input_value);
    char* input_data = RSTRING_PTR(input_value);
    size_t input_size = RSTRING_LEN(input_value);

    size_t max_compressed_size = ZSTD_compressBound(input_size);
    VALUE output = rb_str_new(NULL, max_compressed_size);
    char* output_data = RSTRING_PTR(output);

    size_t const ret = zstd_compress(dc->ctx, output_data, max_compressed_size, input_data, input_size, false);
    if (ZSTD_isError(ret)) {
        rb_raise(rb_eRuntimeError, "compress error error code: %s", ZSTD_getErrorName(ret));
    }
    rb_str_resize(output, ret);

    return output;
}


extern VALUE rb_mZstd, cDictionaryCompress;
void
zstd_ruby_dictionary_compress_init(void)
{
    VALUE cDictionaryCompress = rb_define_class_under(rb_mZstd, "DictionaryCompress", rb_cObject);
    rb_define_alloc_func(cDictionaryCompress, rb_dictionary_compress_allocate);
    rb_define_method(cDictionaryCompress, "initialize", rb_dictionary_compress_initialize, -1);
    rb_define_method(cDictionaryCompress, "compress", rb_dictionary_compress_compress, 1);
}
