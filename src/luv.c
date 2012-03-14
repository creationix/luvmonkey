#include "uv.h"
#include "luv.h"
#include "luv_handle.h"
#include "luv_stream.h"
#include "luv_tcp.h"
#include "luv_timer.h"

#ifndef PATH_MAX
#define PATH_MAX (8096)
#endif

static JSBool luv_run(JSContext *cx, unsigned argc, jsval *vp) {
  uv_run(uv_default_loop());
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_ref(JSContext *cx, unsigned argc, jsval *vp) {
  uv_ref(uv_default_loop());
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_unref(JSContext *cx, unsigned argc, jsval *vp) {
  uv_unref(uv_default_loop());
  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_exepath(JSContext *cx, unsigned argc, jsval *vp) {
  size_t size = 2*PATH_MAX;
  char exec_path[2*PATH_MAX];
  UV_CALL(uv_exepath, exec_path, &size);

  JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyN(cx, exec_path, size)));
  return JS_TRUE;
}

static JSBool luv_cwd(JSContext *cx, unsigned argc, jsval *vp) {
  size_t size = 2*PATH_MAX;
  char cwd_path[2*PATH_MAX];

  uv_err_t err = uv_cwd(cwd_path, size);
  if (err.code != UV_OK) {
    JS_ReportError(cx, "uv_cwd: %s", uv_strerror(err)); \
    return JS_FALSE;                                       \
  }

  JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyN(cx, cwd_path, strlen(cwd_path))));
  return JS_TRUE;
}

static JSBool luv_get_free_memory(JSContext *cx, unsigned argc, jsval *vp) {
  uint64_t size = uv_get_free_memory();
  JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(size));
  return JS_TRUE;
}

static JSBool luv_get_total_memory(JSContext *cx, unsigned argc, jsval *vp) {
  uint64_t size = uv_get_total_memory();
  JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(size));
  return JS_TRUE;
}

static JSBool luv_loadavg(JSContext *cx, unsigned argc, jsval *vp) {
  double avg[3];
  jsval values[3];
  uv_loadavg(avg);
  values[0] = DOUBLE_TO_JSVAL(avg[0]);
  values[1] = DOUBLE_TO_JSVAL(avg[1]),
  values[2] = DOUBLE_TO_JSVAL(avg[2]);
  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JS_NewArrayObject(cx, 3, values)));
  return JS_TRUE;
}

static JSBool luv_uptime(JSContext *cx, unsigned argc, jsval *vp) {
  double uptime;
  uv_uptime(&uptime);
  JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(uptime));
  return JS_TRUE;
}

static JSFunctionSpec luv_functions[] = {
  JS_FS("run", luv_run, 0, JSPROP_ENUMERATE),
  JS_FS("ref", luv_ref, 0, JSPROP_ENUMERATE),
  JS_FS("unref", luv_unref, 0, JSPROP_ENUMERATE),
  JS_FS("exepath", luv_exepath, 0, JSPROP_ENUMERATE),
  JS_FS("cwd", luv_cwd, 0, JSPROP_ENUMERATE),
  JS_FS("get_free_memory", luv_get_free_memory, 0, JSPROP_ENUMERATE),
  JS_FS("get_total_memory", luv_get_total_memory, 0, JSPROP_ENUMERATE),
  JS_FS("loadavg", luv_loadavg, 0, JSPROP_ENUMERATE),
  JS_FS("uptime", luv_uptime, 0, JSPROP_ENUMERATE),
  JS_FS_END
};

JSBool luv_init(JSContext *cx, unsigned argc, jsval *vp) {
  JSObject* uv = JS_NewObject(cx, NULL, NULL, NULL);
  if (!JS_DefineFunctions(cx, uv, luv_functions)) {
    return JS_FALSE;
  }
  if (luv_handle_init(cx, uv)) {
    return JS_FALSE;
  }
  if (luv_stream_init(cx, uv)) {
    return JS_FALSE;
  }
  if (luv_tcp_init(cx, uv)) {
    return JS_FALSE;
  }
  if (luv_timer_init(cx, uv)) {
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(uv));
  return JS_TRUE;
}

