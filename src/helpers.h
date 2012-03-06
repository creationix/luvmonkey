#ifndef LUV_HELPERS_H
#define LUV_HELPERS_H

/* Helper macro to check the return value of uv calls and throw errors */
#define UV_CALL(uv_func, ...)                              \
  if (uv_func(__VA_ARGS__)) {                              \
    uv_err_t err = uv_last_error(uv_default_loop());       \
    JS_ReportError(cx, "##uv_func: %s", uv_strerror(err)); \
    return JS_FALSE;                                       \
  }

#define LUV_REF(cx, obj)                                   \
  (luv_ref_t*)malloc(sizeof(luv_ref_t));                   \
  ref->cx = cx;                                            \
  ref->obj = obj;

#endif