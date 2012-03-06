#include "uv.h"
#include "luv_timer.h"

static void Timer_finalize(JSContext *cx, JSObject *obj);

static JSClass Timer_class = {
  "Timer", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, Timer_finalize,
  JSCLASS_NO_OPTIONAL_MEMBERS
};


static JSBool Timer_constructor(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* obj = JS_NewObject(cx, &Timer_class, Timer_prototype, NULL);

  uv_timer_t* handle = malloc(sizeof(uv_timer_t));
  uv_timer_init(uv_default_loop(), handle);
  JS_SetPrivate(cx, obj, handle);

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}

/* Free the uv_tcp_t* when the object gets GCed */
static void Timer_finalize(JSContext *cx, JSObject *this) {
  free(JS_GetPrivate(cx, this));
}
/*
static JSBool luv_tcp_bind(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_tcp_t* handle;
  handle = (uv_tcp_t*)JS_GetInstancePrivate(cx, this, &Tcp_class, NULL);

  const char* host = "0.0.0.0";
  int port = 8080;

  struct sockaddr_in address = uv_ip4_addr(host, port);

  UV_CALL(uv_tcp_bind, handle, address);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_tcp_nodelay(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_tcp_t* handle;
  handle = (uv_tcp_t*)JS_GetInstancePrivate(cx, this, &Tcp_class, NULL);

  int enable = 1;

  UV_CALL(uv_tcp_nodelay, handle, enable);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_tcp_keepalive(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_tcp_t* handle;
  handle = (uv_tcp_t*)JS_GetInstancePrivate(cx, this, &Tcp_class, NULL);

  int enable = 1;
  int delay = 500;

  UV_CALL(uv_tcp_keepalive, handle, enable, delay);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}
*/

static JSFunctionSpec Timer_methods[] = {
  JS_FS_END
};

int luv_timer_init(JSContext* cx, JSObject *uv) {
  Timer_prototype = JS_InitClass(cx, uv, Handle_prototype,
    &Timer_class, Timer_constructor, 0, 
    NULL, Timer_methods, NULL, NULL);
  return 0;
}
