#ifndef LUV_HANDLE_H
#define LUV_HANDLE_H

#include "jsapi.h"

JSObject* Handle_prototype;

int luv_handle_init(JSContext* cx, JSObject *uv);

#endif