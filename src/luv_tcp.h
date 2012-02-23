#ifndef LUV_TCP_H
#define LUV_TCP_H

#include "uv.h"
#include "jsapi.h"
#include "luv_stream.h"

JSObject* Tcp_prototype;

int luv_tcp_init(JSContext* cx, JSObject *uv);

#endif