#ifndef LUV_TCP_H
#define LUV_TCP_H

#include "jsapi.h"
#include "luv_stream.h"
#include "helpers.h"

JSObject* Tcp_prototype;

int luv_tcp_init(JSContext* cx, JSObject *uv);

#endif