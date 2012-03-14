#include "uv.h"
#include "assert.h"
#include "luv_stream.h"

static JSClass Stream_class = {
  "Stream", 0,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool Stream_constructor(JSContext *cx, unsigned argc, jsval *vp) {
  JSObject* obj = JS_NewObject(cx, &Stream_class, Stream_prototype, NULL);
  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}


static void luv_on_connection(uv_stream_t* server, int status) {
  luv_ref_t* ref;
  ref = (luv_ref_t*)server->data;
  if (!luv_call_callback(ref->cx, ref->obj, "onConnection", 0, NULL)) {
    /* TODO: report properly */
    printf("Error in onConnection callback\n");
  }
}

static JSBool luv_listen(JSContext* cx, unsigned argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_stream_t* stream;
  int backlog;
  JSObject* callback;

  stream = (uv_stream_t*)JS_GetPrivate(this);

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "io", &backlog, &callback)) {
    return JS_FALSE;
  }

  if (!luv_store_callback(cx, this, "onConnection", callback)) return JS_FALSE;

  UV_CALL(uv_listen, stream, backlog, luv_on_connection);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_accept(JSContext* cx, unsigned argc, jsval *vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_stream_t* server;
  uv_stream_t* client;
  JSObject* obj;

  server = (uv_stream_t*)JS_GetPrivate(this);

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &obj)) {
    return JS_FALSE;
  }
  client = (uv_stream_t*)JS_GetPrivate(obj);

  UV_CALL(uv_accept, server, client);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static uv_buf_t luv_on_alloc(uv_handle_t* handle, size_t suggested_size) {
  uv_buf_t buf;
  buf.base = malloc(suggested_size);
  buf.len = suggested_size;
  return buf;
}

static void luv_on_read(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
  luv_ref_t* ref;
  ref = (luv_ref_t*)stream->data;

  if (nread >= 0) {
    jsval chunk = STRING_TO_JSVAL(JS_NewStringCopyN(ref->cx, buf.base, nread));

    if (!luv_call_callback(ref->cx, ref->obj, "onData", 1, &chunk)) {
      /* TODO: report properly */
      printf("Error in onData callback\n");
    }

  } else {
    uv_err_t err = uv_last_error(uv_default_loop());
    if (err.code == UV_EOF) {
      if (!luv_call_callback(ref->cx, ref->obj, "onEnd", 0, NULL)) {
        /* TODO: report properly */
        printf("Error in onError callback\n");
      }
    } else {
      /*
       TODO: Implement close
       uv_close((uv_handle_t*)handle, luv_on_close);
       */
      uv_err_t err = uv_last_error(uv_default_loop());
      /*
       TODO: Proper error handling
       */
      printf("luv_on_read: %s\n", uv_strerror(err));
    }
  }

  free(buf.base);
}

static JSBool luv_read_start(JSContext* cx, unsigned argc, jsval* vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_stream_t* stream;
  stream = (uv_stream_t*)JS_GetPrivate(this);

  UV_CALL(uv_read_start, stream, luv_on_alloc, luv_on_read);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool luv_read_stop(JSContext* cx, unsigned argc, jsval* vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_stream_t* stream;
  stream = (uv_stream_t*)JS_GetPrivate(this);

  UV_CALL(uv_read_stop, stream);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static void luv_on_write(uv_write_t* req, int status) {
  luv_ref_t* ref;
  JSContext* cx;
  JSObject* this;
  JSObject* callback;

  /* Get the context, instance, and handle */
  ref = (luv_ref_t*)req->handle->data;
  cx = ref->cx;
  this = ref->obj;

  /* Get the callback */
  ref = (luv_ref_t*)req->data;
  assert(ref->cx == cx);
  callback = ref->obj;
  free(ref);

  if (JS_ObjectIsFunction(cx, callback)) {
    jsval result;
    if (!JS_CallFunctionValue(cx, this, OBJECT_TO_JSVAL(callback), 0, NULL, &result)) {
      /* TODO: report properly */
      printf("Error in onWrite callback\n");
    }
  }

  /* TODO: free the chunk */
  /* JS_free(cx, chunk); */
}

static JSBool luv_write(JSContext* cx, unsigned argc, jsval* vp) {
  JSObject* this = JS_THIS_OBJECT(cx, vp);
  uv_stream_t* stream;
  JSString* str;
  JSObject* callback;
  uv_buf_t* buf;
  size_t len;
  luv_ref_t* ref;
  uv_write_t* req;

  stream = (uv_stream_t*)JS_GetPrivate(this);

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "So", &str, &callback)) {
    return JS_FALSE;
  }
  /* Put the string in a new uv_buf_t */
  buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
  len = JS_GetStringEncodingLength(cx, str);
  buf->base = (char*)malloc(len);
  buf->len = JS_EncodeStringToBuffer(str, buf->base, len);

  /* Store a reference to the callback in the write request */
  ref = LUV_REF(cx, callback);
  req = (uv_write_t*)malloc(sizeof(uv_write_t));
  req->data = ref;

  UV_CALL(uv_write, req, stream, buf, 1, luv_on_write);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSFunctionSpec Stream_methods[] = {
  JS_FS("write", luv_write, 0, JSPROP_ENUMERATE),
  JS_FS("listen", luv_listen, 0, JSPROP_ENUMERATE),
  JS_FS("accept", luv_accept, 0, JSPROP_ENUMERATE),
  JS_FS("readStart", luv_read_start, 0, JSPROP_ENUMERATE),
  JS_FS("readStop", luv_read_stop, 0, JSPROP_ENUMERATE),
  JS_FS_END
};

int luv_stream_init(JSContext* cx, JSObject *uv) {
  Stream_prototype = JS_InitClass(cx, uv, Handle_prototype,
    &Stream_class, Stream_constructor, 0,
    NULL, Stream_methods, NULL, NULL);
  return 0;
}
