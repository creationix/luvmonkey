#ifndef LHTTP_PARSER_H
#define LHTTP_PARSER_H

#include "jsapi.h"

JSObject* HttpParser_prototype;

JSBool lhttp_parser_init(JSContext *cx, unsigned argc, jsval *vp);

#endif