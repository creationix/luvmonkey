#include "uv.h"
#include "luv_stream.h"

static JSClass Stream_class = {
  "Stream", 0,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool Stream_constructor(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* obj = JS_NewObject(cx, &Stream_class, Stream_prototype, NULL);
  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}


static JSBool luv_write(JSContext *cx, uintN argc, jsval *vp) {
  printf("TODO: Implement luv_write\n");
  return JS_TRUE;
}

static JSFunctionSpec Stream_methods[] = {
  JS_FS("write", luv_write, 0, JSPROP_ENUMERATE),
  JS_FS_END
};

int luv_stream_init(JSContext* cx, JSObject *uv) {
  Stream_prototype = JS_InitClass(cx, uv, Handle_prototype,
    &Stream_class, Stream_constructor, 0, 
    NULL, Stream_methods, NULL, NULL);
  return 0;
}
