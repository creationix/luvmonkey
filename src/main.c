
#include "jsapi.h"
#include "luv.h"
#include "main.h"

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
PrintInternal(JSContext *cx, uintN argc, jsval *vp, FILE *file)
{
  jsval *argv;
  uintN i;
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
Print(JSContext *cx, uintN argc, jsval *vp)
{
  return PrintInternal(cx, argc, vp, stdout);
}

static JSBool
PrintErr(JSContext *cx, uintN argc, jsval *vp)
{
  return PrintInternal(cx, argc, vp, stderr);
}

static JSFunctionSpec global_functions[] = {
  JS_FS("print", Print, 0, 0),
  JS_FS("printErr", PrintErr, 0, 0),
  JS_FS_END
};


static JSBool compileFile(JSContext *cx, uintN argc, jsval *vp) {

  /* TODO: get filename from arg */
  const char* filename = "lib/utils.js";
  JSObject* script = JS_CompileUTF8File(cx, JS_THIS_OBJECT(cx, vp), filename);
  if (!script) {
    return JS_FALSE;
  }
  jsval result;
  if (!JS_ExecuteScript(cx, JS_GetGlobalObject(cx), script, &result)) {
    return JS_FALSE;
  }

  JS_SET_RVAL(cx, vp, result);
  return JS_TRUE;
}


int main(int argc, const char *argv[])
{

  if ( argc < 2 ) {
    printf( "usage: %s filename\n", argv[0] );
    return 1;
  }

  /* JS variables. */
  JSRuntime *rt;
  JSContext *cx;
  JSObject  *global;

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

  /* Attach the global functions */
  if (!JS_DefineFunctions(cx, global, global_functions)) return 1;

  /* Make global reference itself at "global" */
  jsval global_val = OBJECT_TO_JSVAL(global);
  if (!JS_SetProperty(cx, global, "global", &global_val)) return 1;

  /* Create a "uv" namespace for the uv_* functions */
  JSObject* uv = JS_DefineObject(cx, global, "uv", NULL, NULL, 0);
  if (luv_init(cx, uv)) return 1;

  /* define a compileFile function */
  JS_DefineFunction(cx, global, "compileFile", compileFile, 1, 0);

  /* Set args as global */
  JSObject* args = JS_NewArrayObject(cx, 0, NULL);
  jsval args_val = OBJECT_TO_JSVAL(args);
  if (!JS_SetProperty(cx, global, "args", &args_val)) return 1;
  int index;
  for (index = 0; index < argc; index++) {
    jsval arg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, argv[index]));
    if (!JS_SetElement(cx, args, index, &arg)) return 1;
  }

  /* Execute the file given on argv[1] */
  const char* filename = argv[1];
  JSScript* script = JS_CompileUTF8File(cx, global, filename);
  if (!script) return 2;
  JS_ExecuteScript(cx, global, script, NULL);

  /* Cleanup. */
  JS_DestroyContext(cx);
  JS_DestroyRuntime(rt);
  JS_ShutDown();
  return 0;
}