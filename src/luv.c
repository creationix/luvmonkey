#include "uv.h"
#include "jsapi.h"
#include "luv.h"

#ifndef PATH_MAX
#define PATH_MAX (8096)
#endif

static JSBool luv_run(JSContext *cx, uintN argc, jsval *vp) {
  uv_run(uv_default_loop());
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_ref(JSContext *cx, uintN argc, jsval *vp) {
  uv_ref(uv_default_loop());
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_unref(JSContext *cx, uintN argc, jsval *vp) {
  uv_unref(uv_default_loop());
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_exepath(JSContext *cx, uintN argc, jsval *vp) {
  size_t size = 2*PATH_MAX;
  char exec_path[2*PATH_MAX];
  if (uv_exepath(exec_path, &size)) {
    uv_err_t err = uv_last_error(uv_default_loop());
    JS_ReportError(cx, "uv_exepath: %s", uv_strerror(err));
    return JS_FALSE;
  }
  JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyN(cx, exec_path, size)));
  return JS_TRUE;
}

JSFunctionSpec luv_functions[] = {
  JS_FS("run", luv_run, 0, 0),
  JS_FS("ref", luv_ref, 0, 0),
  JS_FS("unref", luv_unref, 0, 0),
  JS_FS("exepath", luv_exepath, 0, 0),
  JS_FS_END
};



