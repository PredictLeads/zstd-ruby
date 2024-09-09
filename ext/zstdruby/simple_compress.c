#include "common.h"

struct simple_compress_t {
    ZSTD_CCtx* cctx;
    ZSTD_CCtx* dctx;
};

static void
simple_compress_mark(void *p)
{
    struct simple_compress_t *sc = p;
}

static void
simple_compress_free(void *p)
{
    struct simple_compress_t *sc = p;

    ZSTD_CCtx* cctx = sc->cctx;
    if (ctx != NULL) {
        ZSTD_freeCCtx(cctx);
    }

    ZSTD_DCtx* dctx = sc->dctx;
    if (dctx != NULL) {
        ZSTD_freeCCtx(dctx);
    }

    xfree(sc);
}

static size_t
simple_compress_memsize(const void *p)
{
    return sizeof(struct simple_compress_t);
}

#ifdef HAVE_RB_GC_MARK_MOVABLE
static void
simple_compress_compact(void *p)
{
    struct simple_compress_t *sc = p;
}
#endif

static const rb_data_type_t simple_compress_type = {
  "simple_compress",
  {
    simple_compress_mark,
    simple_compress_free,
    simple_compress_memsize,
#ifdef HAVE_RB_GC_MARK_MOVABLE
    simple_compress_compact,
#endif
  },
  0, 0, RUBY_TYPED_FREE_IMMEDIATELY
};


static VALUE
rb_simple_compress_allocate(VALUE klass)
{
    struct simple_compress_t* sc;
    VALUE obj = TypedData_Make_Struct(klass, struct simple_compress_t, &simple_compress_type, sc);
    sc->cctx = NULL;
    sc->dctx = NULL;
    return obj;
}


static VALUE
rb_simple_compress_initialize(int argc, VALUE *argv, VALUE obj)
{
    VALUE kwargs;
    VALUE compression_level_value;
    rb_scan_args(argc, argv, "00:", &kwargs);

    struct simple_compress_t* sc;
    TypedData_Get_Struct(obj, struct simple_compress_t, &simple_compress_type, sc);

    ZSTD_CCtx* const ctx = ZSTD_createCCtx();
    if (ctx == NULL) {
        rb_raise(rb_eRuntimeError, "%s", "ZSTD_createCCtx error");
    }

    sc->ctx = ctx;

    compression_level_value = Qnil;

    set_compress_params(ctx, compression_level_value, kwargs);

    return obj;
}


static VALUE
rb_simple_compress_compress(VALUE obj, VALUE input_value)
{

    struct simple_compress_t* sc;
    TypedData_Get_Struct(obj, struct simple_compress_t, &simple_compress_type, sc);

    StringValue(input_value);
    char* input_data = RSTRING_PTR(input_value);
    size_t input_size = RSTRING_LEN(input_value);

    size_t max_compressed_size = ZSTD_compressBound(input_size);
    VALUE output = rb_str_new(NULL, max_compressed_size);
    char* output_data = RSTRING_PTR(output);

    size_t const ret = zstd_compress(sc->cctx, output_data, max_compressed_size, input_data, input_size, false);
    if (ZSTD_isError(ret)) {
        rb_raise(rb_eRuntimeError, "compress error error code: %s", ZSTD_getErrorName(ret));
    }
    rb_str_resize(output, ret);

    return output;
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
rb_simple_compress_decompress(VALUE obj, VALUE input_value)
{

    struct simple_compress_t* sc;
    TypedData_Get_Struct(obj, struct simple_compress_t, &simple_compress_type, sc);

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
        return decompress_buffered(sc->dctx, input_data, input_size);
    }

    VALUE output = rb_str_new(NULL, uncompressed_size);
    char* output_data = RSTRING_PTR(output);

    size_t const decompress_size = zstd_decompress(sc->dctx, output_data, uncompressed_size, input_data, input_size, false);
    if (ZSTD_isError(decompress_size)) {
        rb_raise(rb_eRuntimeError, "%s: %s", "decompress error", ZSTD_getErrorName(decompress_size));
    }

    return output;
}


extern VALUE rb_mZstd, cSimpleCompress;
void
zstd_ruby_simple_compress_init(void)
{
    VALUE cSimpleCompress = rb_define_class_under(rb_mZstd, "SimpleCompress", rb_cObject);
    rb_define_alloc_func(cSimpleCompress, rb_simple_compress_allocate);
    rb_define_method(cSimpleCompress, "initialize", rb_simple_compress_initialize, -1);
    rb_define_method(cSimpleCompress, "compress", rb_simple_compress_compress, 1);
    rb_define_method(cSimpleCompress, "decompress", rb_simple_compress_decompress, 1);
}
