#include "luv_tcp.h"

static JSClass Tcp_class = {
  "Tcp", 0,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool Tcp_constructor(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* obj = JS_NewObject(cx, &Tcp_class, Tcp_prototype, NULL);
  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}


static JSBool luv_bind(JSContext *cx, uintN argc, jsval *vp) {
  printf("TODO: Implement luv_bind\n");
  return JS_TRUE;
}

static JSFunctionSpec Tcp_methods[] = {
  JS_FS("bind", luv_bind, 0, JSPROP_ENUMERATE),
  JS_FS_END
};

int luv_tcp_init(JSContext* cx, JSObject *uv) {
  Tcp_prototype = JS_InitClass(cx, uv, Stream_prototype,
    &Tcp_class, Tcp_constructor, 0, 
    NULL, Tcp_methods, NULL, NULL);
  return 0;
}
