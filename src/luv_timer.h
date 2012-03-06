#ifndef LUV_TIMER_H
#define LUV_TIMER_H

#include "jsapi.h"
#include "luv_handle.h"
#include "helpers.h"

JSObject* Timer_prototype;

int luv_timer_init(JSContext* cx, JSObject *uv);

#endif