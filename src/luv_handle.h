#ifndef LUV_HANDLE_H
#define LUV_HANDLE_H

#include "jsapi.h"

JSObject* Handle_prototype;

int luv_handle_init(JSContext* cx, JSObject *uv);

JSBool luv_store_callback(JSContext* cx, JSObject *this, const char* name, JSObject* callback);
JSBool luv_call_callback(JSContext* cx, JSObject *this, const char* name, unsigned argc, jsval* argv);

typedef struct {
  JSContext* cx;
  JSObject* obj;
} luv_ref_t;


#endif