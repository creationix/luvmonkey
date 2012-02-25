#include "uv.h"
#include "luv_tcp.h"

static void Tcp_finalize(JSContext *cx, JSObject *obj);

static JSClass Tcp_class = {
  "Tcp", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, Tcp_finalize,
  JSCLASS_NO_OPTIONAL_MEMBERS
};


static JSBool Tcp_constructor(JSContext *cx, uintN argc, jsval *vp) {
  printf("Tcp instance getting created\n");
  JSObject* obj = JS_NewObject(cx, &Tcp_class, Tcp_prototype, NULL);

  uv_tcp_t* handle = malloc(sizeof(uv_tcp_t));
  uv_tcp_init(uv_default_loop(), handle);
  JS_SetPrivate(cx, obj, handle);

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}

/* Free the uv_tcp_t* when the object gets GCed */
static void Tcp_finalize(JSContext *cx, JSObject *this) {
  printf("Tcp instance getting freed\n");
  free(JS_GetPrivate(cx, this));
}

static JSBool luv_tcp_bind(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_tcp_t* handle;
  handle = (uv_tcp_t*)JS_GetInstancePrivate(cx, this, &Tcp_class, NULL);

  /* TODO: don't hardcode */
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

  /* TODO: don't hardcode */
  int enable = 1;

  UV_CALL(uv_tcp_nodelay, handle, enable);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_tcp_keepalive(JSContext *cx, uintN argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_tcp_t* handle;
  handle = (uv_tcp_t*)JS_GetInstancePrivate(cx, this, &Tcp_class, NULL);

  /* TODO: don't hardcode */
  int enable = 1;
  int delay = 500;

  UV_CALL(uv_tcp_keepalive, handle, enable, delay);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSFunctionSpec Tcp_methods[] = {
  JS_FS("bind", luv_tcp_bind, 0, 0),
  JS_FS("nodelay", luv_tcp_nodelay, 0, 0),
  JS_FS("keepalive", luv_tcp_keepalive, 0, 0),
  JS_FS_END
};

int luv_tcp_init(JSContext* cx, JSObject *uv) {
  Tcp_prototype = JS_InitClass(cx, uv, Stream_prototype,
    &Tcp_class, Tcp_constructor, 0, 
    NULL, Tcp_methods, NULL, NULL);
  return 0;
}
