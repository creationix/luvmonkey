#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "lhttp_parser.h"
#include "http_parser.h"

/*
static const char* method_to_str(unsigned short m) {
  switch (m) {
#define X(num, name, string) case HTTP_##name: return #string;
  HTTP_METHOD_MAP(X)
#undef X
  }
  return "UNKNOWN_METHOD";
}
*/

static struct http_parser_settings lhttp_parser_settings;

static int lhttp_parser_on_message_begin(http_parser *p) {
  printf("on_message_begin\n");
  return 0;
}

static int lhttp_parser_on_message_complete(http_parser *p) {
  printf("on_message_complete\n");
  return 0;
}


static int lhttp_parser_on_url(http_parser *p, const char *at, size_t length) {
  printf("on_url\n");
  return 0;
}

static int lhttp_parser_on_header_field(http_parser *p, const char *at, size_t length) {
  printf("on_header_field\n");
  return 0;
}

static int lhttp_parser_on_header_value(http_parser *p, const char *at, size_t length) {
  printf("on_header_value\n");
  return 0;
}

static int lhttp_parser_on_body(http_parser *p, const char *at, size_t length) {
  printf("on_header_body\n");
  return 0;
}

static int lhttp_parser_on_headers_complete(http_parser *p) {
  printf("on_headers_complete\n");
  return 0;
}

static void HttpParser_finalize(JSContext *cx, JSObject *obj);

static JSClass HttpParser_class = {
  "HttpParser", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, HttpParser_finalize,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool HttpParser_constructor(JSContext *cx, unsigned argc, jsval *vp) {
  JSObject* obj = JS_NewObject(cx, &HttpParser_class, HttpParser_prototype, NULL);

  http_parser* parser = malloc(sizeof(http_parser));
  http_parser_init(parser, HTTP_REQUEST);
  JS_SetPrivate(obj, parser);

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

  return JS_TRUE;
}

static void HttpParser_finalize(JSContext *cx, JSObject *this) {
  free(JS_GetPrivate(this));
}

static JSBool lhttp_parser_execute(JSContext *cx, unsigned argc, jsval *vp) {
  JSObject* this;
  JSString* str;
  http_parser* parser;
  size_t chunk_len;
  char *chunk;
  size_t offset;
  size_t length;
  size_t nparsed;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "Sii", &str, &offset, &length)) {
    return JS_FALSE;
  }
  chunk_len = JS_GetStringEncodingLength(cx, str);
  chunk = (char*)malloc(chunk_len);
  chunk_len = JS_EncodeStringToBuffer(str, chunk, chunk_len); 

  this = JS_THIS_OBJECT(cx, vp);
  parser = (http_parser*)JS_GetInstancePrivate(cx, this, &HttpParser_class, NULL);

  if (offset < chunk_len) {
    JS_ReportError(cx, "Offset is out of bounds");
    free(chunk);
    return JS_FALSE;
  }
  if (offset + length <= chunk_len) {
    JS_ReportError(cx, "Length extends beyond end of chunk");
    free(chunk);
    return JS_FALSE;
  }

  nparsed = http_parser_execute(parser, &lhttp_parser_settings, chunk + offset, length);

  free(chunk);

  JS_SET_RVAL(cx, vp, INT_TO_JSVAL(nparsed));
  return JS_TRUE;
}

static JSBool lhttp_parser_finish(JSContext *cx, unsigned argc, jsval *vp) {
  JSObject* this;
  http_parser* parser;

  this = JS_THIS_OBJECT(cx, vp);
  parser = (http_parser*)JS_GetInstancePrivate(cx, this, &HttpParser_class, NULL);

  if (http_parser_execute(parser, &lhttp_parser_settings, NULL, 0)) {
    JS_ReportError(cx, "Parse Error");
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool lhttp_parser_reinitialize(JSContext *cx, unsigned argc, jsval *vp) {
  JSObject* this;
  http_parser* parser;
  JSString* str;
  char *type;

  this = JS_THIS_OBJECT(cx, vp);
  parser = (http_parser*)JS_GetInstancePrivate(cx, this, &HttpParser_class, NULL);
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &str)) {
    return JS_FALSE;
  }
  type = JS_EncodeString(cx, str);

  if (0 == strcmp(type, "request")) {
    http_parser_init(parser, HTTP_REQUEST);
  } else if (0 == strcmp(type, "response")) {
    http_parser_init(parser, HTTP_RESPONSE);
  } else {
    JS_ReportError(cx, "type must be 'request' or 'response'");
    JS_free(cx, type);
    return JS_FALSE;
  }
  JS_free(cx, type);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool lhttp_parser_parse_url(JSContext *cx, unsigned argc, jsval *vp) {

  size_t length;
  char *url;
  struct http_parser_url u;
  int is_connect = 0;
  JSString* str;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S/i", &str, &is_connect)) {
    return JS_FALSE;
  }
  length = JS_GetStringEncodingLength(cx, str);
  url = (char*) malloc(length);
  length = JS_EncodeStringToBuffer(str, url, length); 

  if (http_parser_parse_url(url, length, is_connect, &u)) {
    JS_ReportError(cx, "Error parsing url %s", url);
    free(url);
    return JS_FALSE;
  }

  JSObject* obj = JS_NewObject(cx, NULL, NULL, NULL);

  if (u.field_set & (1 << UF_SCHEMA)) {
    jsval val = STRING_TO_JSVAL(JS_NewStringCopyN(cx, url + u.field_data[UF_SCHEMA].off, u.field_data[UF_SCHEMA].len));
    JS_SetProperty(cx, obj, "schema", &val);
  }
  if (u.field_set & (1 << UF_HOST)) {
    jsval val = STRING_TO_JSVAL(JS_NewStringCopyN(cx, url + u.field_data[UF_HOST].off, u.field_data[UF_HOST].len));
    JS_SetProperty(cx, obj, "host", &val);
  }
  if (u.field_set & (1 << UF_PORT)) {
    jsval val = STRING_TO_JSVAL(JS_NewStringCopyN(cx, url + u.field_data[UF_PORT].off, u.field_data[UF_PORT].len));
    JS_SetProperty(cx, obj, "port_string", &val);
    jsval port = INT_TO_JSVAL(u.port);
    JS_SetProperty(cx, obj, "port", &port);
  }
  if (u.field_set & (1 << UF_PATH)) {
    jsval val = STRING_TO_JSVAL(JS_NewStringCopyN(cx, url + u.field_data[UF_PATH].off, u.field_data[UF_PATH].len));
    JS_SetProperty(cx, obj, "path", &val);
  }
  if (u.field_set & (1 << UF_QUERY)) {
    jsval val = STRING_TO_JSVAL(JS_NewStringCopyN(cx, url + u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len));
    JS_SetProperty(cx, obj, "query", &val);
  }
  if (u.field_set & (1 << UF_FRAGMENT)) {
    jsval val = STRING_TO_JSVAL(JS_NewStringCopyN(cx, url + u.field_data[UF_FRAGMENT].off, u.field_data[UF_FRAGMENT].len));
    JS_SetProperty(cx, obj, "fragment", &val);
  }

  free(url);

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
  return JS_TRUE;
}


static JSFunctionSpec HttpParser_methods[] = {
  JS_FS("execute", lhttp_parser_execute, 0, 0),
  JS_FS("finish", lhttp_parser_finish, 0, 0),
  JS_FS("reinitialize", lhttp_parser_reinitialize, 0, 0),
  JS_FS_END
};


JSBool lhttp_parser_init(JSContext *cx, unsigned argc, jsval *vp) {

  /* Hook up the C callbacks */
  lhttp_parser_settings.on_message_begin    = lhttp_parser_on_message_begin;
  lhttp_parser_settings.on_url              = lhttp_parser_on_url;
  lhttp_parser_settings.on_header_field     = lhttp_parser_on_header_field;
  lhttp_parser_settings.on_header_value     = lhttp_parser_on_header_value;
  lhttp_parser_settings.on_headers_complete = lhttp_parser_on_headers_complete;
  lhttp_parser_settings.on_body             = lhttp_parser_on_body;
  lhttp_parser_settings.on_message_complete = lhttp_parser_on_message_complete;

  JSObject* http_parser = JS_NewObject(cx, NULL, NULL, NULL);

  HttpParser_prototype = JS_InitClass(cx, http_parser, NULL,
    &HttpParser_class, HttpParser_constructor, 0,
    NULL, HttpParser_methods, NULL, NULL);

  JS_DefineFunction(cx, http_parser, "parseUrl", lhttp_parser_parse_url, 0, 0);

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(http_parser));
  return JS_TRUE;
}

