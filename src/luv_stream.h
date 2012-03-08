#ifndef LUV_STREAM_H
#define LUV_STREAM_H

#include "jsapi.h"
#include "luv_handle.h"
#include "helpers.h"

JSObject* Stream_prototype;

int luv_stream_init(JSContext* cx, JSObject *uv);

#endif