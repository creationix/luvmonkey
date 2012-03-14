#include "uv.h"
#include "luv_timer.h"

static void Timer_finalize(JSContext *cx, JSObject *obj);

static JSClass Timer_class = {
  "Timer", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, Timer_finalize,
  JSCLASS_NO_OPTIONAL_MEMBERS
};


static JSBool Timer_constructor(JSContext *cx, unsigned argc, jsval *vp) {
  JSObject* obj = JS_NewObject(cx, &Timer_class, Timer_prototype, NULL);
  uv_timer_t* handle;
  luv_ref_t* ref;

  handle = malloc(sizeof(uv_timer_t));
  uv_timer_init(uv_default_loop(), handle);
  JS_SetPrivate(obj, handle);

  /* Store a reference to the object in the handle */
  ref = LUV_REF(cx, obj);
  handle->data = ref;

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}

/* Free the uv_tcp_t* when the object gets GCed */
static void Timer_finalize(JSContext *cx, JSObject *this) {
  free(JS_GetPrivate(this));
}

void luv_on_timer(uv_timer_t* handle, int status) {
  luv_ref_t* ref;
  ref = (luv_ref_t*)handle->data;
  if (!luv_call_callback(ref->cx, ref->obj, "onTimer", 0, NULL)) {
    /* TODO: report properly */
    printf("Error in onTimer callback\n");
  }
}


static JSBool luv_timer_start(JSContext* cx, unsigned argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_timer_t* handle;
  int32_t timeout;
  int32_t repeat;
  JSObject* callback;

  handle = (uv_timer_t*)JS_GetInstancePrivate(cx, this, &Timer_class, NULL);

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "uuo", &timeout, &repeat, &callback)) {
    return JS_FALSE;
  }

  if (!luv_store_callback(cx, this, "onTimer", callback)) return JS_FALSE;

  UV_CALL(uv_timer_start, handle, luv_on_timer, timeout, repeat);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_timer_stop(JSContext* cx, unsigned argc, jsval* vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_timer_t* handle;
  handle = (uv_timer_t*)JS_GetInstancePrivate(cx, this, &Timer_class, NULL);

  UV_CALL(uv_timer_stop, handle);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_timer_again(JSContext* cx, unsigned argc, jsval* vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_timer_t* handle;
  handle = (uv_timer_t*)JS_GetInstancePrivate(cx, this, &Timer_class, NULL);

  UV_CALL(uv_timer_again, handle);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_timer_set_repeat(JSContext* cx, unsigned argc, jsval* vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_timer_t* handle;
  int32_t repeat;

  handle = (uv_timer_t*)JS_GetInstancePrivate(cx, this, &Timer_class, NULL);

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "u", &repeat)) {
    return JS_FALSE;
  }

  uv_timer_set_repeat(handle, repeat);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_timer_get_repeat(JSContext* cx, unsigned argc, jsval* vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_timer_t* handle;
  int64_t repeat;

  handle = (uv_timer_t*)JS_GetInstancePrivate(cx, this, &Timer_class, NULL);

  repeat = uv_timer_get_repeat(handle);

  JS_SET_RVAL(cx, vp, INT_TO_JSVAL(repeat));
  return JS_TRUE;
}


static JSFunctionSpec Timer_methods[] = {
  JS_FS("start", luv_timer_start, 0, 0),
  JS_FS("stop", luv_timer_stop, 0, 0),
  JS_FS("again", luv_timer_again, 0, 0),
  JS_FS("setRepeat", luv_timer_set_repeat, 0, 0),
  JS_FS("getRepeat", luv_timer_get_repeat, 0, 0),
  JS_FS_END
};

int luv_timer_init(JSContext* cx, JSObject *uv) {
  Timer_prototype = JS_InitClass(cx, uv, Handle_prototype,
    &Timer_class, Timer_constructor, 0,
    NULL, Timer_methods, NULL, NULL);
  return 0;
}
