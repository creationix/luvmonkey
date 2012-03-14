#include "uv.h"
#include "luv_handle.h"

static JSClass Handle_class = {
  "Handle", 0,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool Handle_constructor(JSContext *cx, unsigned argc, jsval *vp) {
  JSObject* obj = JS_NewObject(cx, &Handle_class, Handle_prototype, NULL);
  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}

void luv_on_close(uv_handle_t* handle) {
  luv_ref_t* ref;
  ref = (luv_ref_t*)handle->data;
  if (!luv_call_callback(ref->cx, ref->obj, "onClose", 0, NULL)) {
    /* TODO: report properly */
    printf("Error in onClose callback\n");
  }
  free(handle);
}

static JSBool luv_close(JSContext *cx, unsigned argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_handle_t* handle;
  JSObject* callback;

  handle = (uv_handle_t*)JS_GetPrivate(this);

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &callback)) {
    return JS_FALSE;
  }

  if (!luv_store_callback(cx, this, "onClose", callback)) return JS_FALSE;

  uv_close(handle, luv_on_close);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSFunctionSpec Handle_methods[] = {
  JS_FS("close", luv_close, 0, 0),
  JS_FS_END
};

int luv_handle_init(JSContext* cx, JSObject *uv) {
  Handle_prototype = JS_InitClass(cx, uv, NULL,
    &Handle_class, Handle_constructor, 0,
    NULL, Handle_methods, NULL, NULL);
  return 0;
}

/* Store an async callback in an object and put the object in the gc root for safekeeping */
JSBool luv_store_callback(JSContext* cx, JSObject *this, const char* name, JSObject* callback) {
  jsval callback_val;
  if (!JS_AddObjectRoot(cx, &this)) return JS_FALSE;
  callback_val = OBJECT_TO_JSVAL(callback);
  return JS_SetProperty(cx, this, name, &callback_val);
}

/* Call a callback stored in an object and free it from the gc root */
JSBool luv_call_callback(JSContext* cx, JSObject *this, const char* name, unsigned argc, jsval* argv) {
  jsval callback_val;
  JSBool ret = JS_GetProperty(cx, this, name, &callback_val);

  if (ret && JSVAL_IS_OBJECT(callback_val) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(callback_val))) {
    jsval result;
    ret = JS_CallFunctionValue(cx, this, callback_val, argc, argv, &result);
  }
  JS_RemoveObjectRoot(cx, &this);
  return ret;
}
