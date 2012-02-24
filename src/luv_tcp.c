#include "luv_tcp.h"

static JSClass Tcp_class = {
  "Tcp", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

/* TODO: Free the struct in the finalizer */

static JSBool Tcp_constructor(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* obj = JS_NewObject(cx, &Tcp_class, Tcp_prototype, NULL);

  JS_SetPrivate(cx, obj, malloc(sizeof(uv_tcp_t)));

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}


static JSBool luv_tcp_bind(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_tcp_t* handle;
  handle = (uv_tcp_t*)JS_GetInstancePrivate(cx, this, &Tcp_class, NULL);

  /* TODO: don't hardcode */
  const char* host = "0.0.0.0";
  int port = 8080;

  struct sockaddr_in address = uv_ip4_addr(host, port);

  if (uv_tcp_bind(handle, address)) {
    uv_err_t err = uv_last_error(uv_default_loop());
    JS_ReportError(cx, "uv_tcp_bind: %s", uv_strerror(err));
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSFunctionSpec Tcp_methods[] = {
  JS_FS("bind", luv_tcp_bind, 0, JSPROP_ENUMERATE),
  JS_FS_END
};

int luv_tcp_init(JSContext* cx, JSObject *uv) {
  Tcp_prototype = JS_InitClass(cx, uv, Stream_prototype,
    &Tcp_class, Tcp_constructor, 0, 
    NULL, Tcp_methods, NULL, NULL);
  return 0;
}
