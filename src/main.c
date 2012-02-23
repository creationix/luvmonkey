
#include "uv.h"
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



/* Read a file to memory */
char* read_file(const char* filename) {
  FILE* fd;
  long length;
  char* data;
  size_t result;

  fd = fopen(filename , "rb" );
  if (fd == NULL) { fputs("File error", stderr); exit (1); }

  fseek (fd, 0, SEEK_END);
  length = ftell(fd);
  rewind(fd);

  data = (char*)malloc(sizeof(char) * (length + 1));
  if (data == NULL) { fputs ("Memory error", stderr); exit (2); }

  result = fread(data, 1, length, fd);
  if (result != length) { fputs ("Reading error", stderr); exit (3); }

  fclose(fd);

  return data;
}

int main(int argc, const char *argv[])
{

  if ( argc != 2 )
  {
    /* We print argv[0] assuming it is the program name */
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
  JS_SetOptions(cx, JSOPTION_VAROBJFIX | JSOPTION_JIT | JSOPTION_METHODJIT);
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

  /* Put the uv_* function under the global object "uv" */
  JSObject *uv = JS_NewObject(cx, NULL, NULL, NULL);
  if (!JS_DefineFunctions(cx, uv, luv_functions)) return 1;
  jsval uv_val = OBJECT_TO_JSVAL(uv);
  if (!JS_SetProperty(cx, global, "uv", &uv_val)) return 1;

  /* Read the file on argv */
  const char* filename = argv[1];


  /* Test the API */
  jsval rval;
  char* source = read_file(filename);
  JS_EvaluateScript(cx, global, source, strlen(source), filename, 0, &rval);
  free(source);

  /* Cleanup. */
  JS_DestroyContext(cx);
  JS_DestroyRuntime(rt);
  JS_ShutDown();
  return 0;
}