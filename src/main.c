
#include "jsapi.h"
#include "luv.h"
#include "main.h"
#include "js_scripts.h"

/* The class of the global object. */
static JSClass global_class = {
  "global", JSCLASS_GLOBAL_FLAGS,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

/* The error reporter callback. */
void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
  fprintf(stderr, "%s:%u:%s\n",
          report->filename ? report->filename : "<no filename>",
          (unsigned int) report->lineno,
          message);
}

static JSBool
PrintInternal(JSContext *cx, unsigned argc, jsval *vp, FILE *file)
{
  jsval *argv;
  int i;
  JSString *str;
  char *bytes;

  argv = JS_ARGV(cx, vp);
  for (i = 0; i < argc; i++) {
    str = JS_ValueToString(cx, argv[i]);
    if (!str)
      return JS_FALSE;
    bytes = JS_EncodeString(cx, str);
    if (!bytes)
      return JS_FALSE;
    fprintf(file, "%s%s", i ? " " : "", bytes);
    JS_free(cx, bytes);
  }

  fputc('\n', file);
  fflush(file);

  JS_SET_RVAL(cx, vp, JSVAL_VOID);
  return JS_TRUE;
}

static JSBool
Print(JSContext *cx, unsigned argc, jsval *vp)
{
  return PrintInternal(cx, argc, vp, stdout);
}

static JSBool
PrintErr(JSContext *cx, unsigned argc, jsval *vp)
{
  return PrintInternal(cx, argc, vp, stderr);
}

static JSBool Exit(JSContext *cx, unsigned argc, jsval *vp) {
  int exitCode;
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "i", &exitCode)) {
    return JS_FALSE;
  }
  exit(exitCode);
}

static JSBool executeFile(JSContext *cx, unsigned argc, jsval *vp) {
  JSString* str;
  JSObject* obj;
  char *filename;
  JSScript* script;
  jsval result;

  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "So", &str, &obj)) {
    return JS_FALSE;
  }
  filename = JS_EncodeString(cx, str);

  script = JS_CompileUTF8File(cx, JS_THIS_OBJECT(cx, vp), filename);
  JS_free(cx, filename);
  if (!script) {
    return JS_FALSE;
  }

  if (!JS_ExecuteScript(cx, obj, script, &result)) {
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, result);
  return JS_TRUE;
}

static JSFunctionSpec global_functions[] = {
  JS_FS("print", Print, 0, 0),
  JS_FS("printErr", PrintErr, 0, 0),
  JS_FS("executeFile", executeFile, 2, 0),
  JS_FS("exit", Exit, 1, 0),
  JS_FS_END
};

static JSFunctionSpec binding_functions[] = {
  JS_FS("uv", luv_init, 0, 0),
  JS_FS_END
};

int main(int argc, const char *argv[])
{
  int index;

  /* JS variables. */
  JSRuntime* rt;
  JSContext* cx;
  JSObject* global;
  JSObject* alpha;
  JSObject* bindings;
  jsval global_val;
  JSObject* args;
  jsval args_val;

  /* Create a JS runtime. */
  rt = JS_NewRuntime(8L * 1024L * 1024L);
  if (rt == NULL)
      return 1;

  /* Create a context. */
  cx = JS_NewContext(rt, 8192);
  if (cx == NULL)
      return 1;
  JS_SetOptions(cx, JSOPTION_VAROBJFIX | JSOPTION_METHODJIT);
  JS_SetVersion(cx, JSVERSION_LATEST);
  JS_SetErrorReporter(cx, reportError);

  /* Create the global object in a new compartment. */
  global = JS_NewCompartmentAndGlobalObject(cx, &global_class, NULL);
  if (global == NULL) return 1;

  /* Populate the global object with the standard globals,
     like Object and Array. */
  if (!JS_InitStandardClasses(cx, global)) return 1;

  alpha = JS_DefineObject(cx, global, "alpha", NULL, NULL, 0);

  /* Attach the global functions */
  if (!JS_DefineFunctions(cx, alpha, global_functions)) return 1;

  /* expose the binding functions for require to use */
  bindings = JS_DefineObject(cx, alpha, "bindings", NULL, NULL, 0);
  if (!JS_DefineFunctions(cx, bindings, binding_functions)) return 1;

  /* Set global on alpha */
  global_val = OBJECT_TO_JSVAL(global);
  if (!JS_SetProperty(cx, alpha, "global", &global_val)) return 1;

  /* Set args on alpha */
  args = JS_NewArrayObject(cx, 0, NULL);
  args_val = OBJECT_TO_JSVAL(args);
  if (!JS_SetProperty(cx, alpha, "args", &args_val)) return 1;
  for (index = 0; index < argc; index++) {
    jsval arg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, argv[index]));
    if (!JS_SetElement(cx, args, index, &arg)) return 1;
  }

  /* Bootstrap using the code in main.js */
  JS_EvaluateScript(cx, global, embedded_src_main_js, strlen(embedded_src_main_js), "main.js", 1, NULL);

  /* Cleanup. */
  JS_DestroyContext(cx);
  JS_DestroyRuntime(rt);
  JS_ShutDown();
  return 0;
}