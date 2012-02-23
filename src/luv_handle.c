#include "luv_handle.h"

static JSClass Handle_class = {
  "Handle", 0,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool Handle_constructor(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* obj = JS_NewObject(cx, &Handle_class, Handle_prototype, NULL);
  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}

void luv_on_close(uv_handle_t* handle) {
  printf("TODO: luv_on_close\n");
}

static JSBool luv_close(JSContext *cx, uintN argc, jsval *vp) {
  printf("TODO: Implement luv_close\n");
  /*
  uv_handle_t* handle = (uv_handle_t*) get from somewhere?
  uv_close(handle, luv_on_close);
  */
  return JS_TRUE;
}

static JSFunctionSpec Handle_methods[] = {
  JS_FS("close", luv_close, 0, JSPROP_ENUMERATE),
  JS_FS_END
};

int luv_handle_init(JSContext* cx, JSObject *uv) {
  Handle_prototype = JS_InitClass(cx, uv, NULL,
    &Handle_class, Handle_constructor, 0, 
    NULL, Handle_methods, NULL, NULL);
  return 0;
}
