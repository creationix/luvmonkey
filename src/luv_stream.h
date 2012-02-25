#ifndef LUV_STREAM_H
#define LUV_STREAM_H

#include "jsapi.h"
#include "luv_handle.h"

JSObject* Stream_prototype;

int luv_stream_init(JSContext* cx, JSObject *uv);

#endif