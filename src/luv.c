#include "uv.h"
#include "jsapi.h"
#include "luv.h"

static JSBool luv_run(JSContext *cx, uintN argc, jsval *vp) {
  uv_run(uv_default_loop());
  return JS_TRUE;
}

static JSBool luv_ref(JSContext *cx, uintN argc, jsval *vp) {
  uv_ref(uv_default_loop());
  return JS_TRUE;
}

static JSBool luv_unref(JSContext *cx, uintN argc, jsval *vp) {
  uv_unref(uv_default_loop());
  return JS_TRUE;
}

JSFunctionSpec luv_functions[] = {
    JS_FS("run", luv_run, 0, 0),
    JS_FS("ref", luv_ref, 0, 0),
    JS_FS("unref", luv_unref, 0, 0),
    JS_FS_END
};



