// Keep locals out of global scope
(function () {
"use strict"

// Copy the interesting bits from alpha to where we want them
var bindings = alpha.bindings;
var executeFile = alpha.executeFile;
var global = alpha.global;
global.global = global;
global.args = alpha.args;
global.print = alpha.print;
global.exit = alpha.exit;
global.printErr = alpha.printErr;
delete global.alpha;

// This is needed before we can require anything
var moduleCache = {};

var uv = realRequire('uv');
// Assumes the executible is in $PREFIX/bin/luvmonkey and calculates prefix backwards
var prefix = uv.exepath().match(/^(.*)(\/[^\/]+){2}$/)[1] + "/";

function loadModule(filename) {
  var dirname = filename.substr(0, filename.lastIndexOf("/"));
  function require(name) {
    return realRequire(name, dirname);
  }
  require.resolve = function resolve(name) {
    return realResolve(name, dirname);
  }
  var exports = moduleCache[filename];
  var module = { exports: exports };
  var sandbox = Object.create(global, {
    exports: { value: exports },
    module: { value: module },
    __filename: { value: filename },
    __dirname: { value: dirname },
    require: { value: require }
  });
  // TODO: figure out why local variables in modules are getting put on global.
  executeFile(filename, sandbox);
  return module.exports;
}

function realResolve(name, base) {
  // TODO: put prefix here once makefile puts binary in bin folder
  return "lib/luvmonkey/" + name + ".js";
}

function realRequire(name, base) {
  // First look for built-in C bindings (like 'uv')
  if (bindings.hasOwnProperty(name)) {
    if (moduleCache.hasOwnProperty(name)) {
      return moduleCache[name];
    }
    return moduleCache[name] = bindings[name]();
  }

  // Then look for javascript modules
  var filename = realResolve(name, base);
  if (moduleCache.hasOwnProperty(filename)) {
    return moduleCache[filename];
  }
  moduleCache[filename] = {};
  return moduleCache[filename] = loadModule(filename);
}

if (args.length < 2) {
  print("Usage\n\t" + args[0] + " filename.js\n")
  exit(1);
}

// Execute the first arg as a luvmonkey script.
var path = args[1];
if (path[0] !== "/") path = uv.cwd() + "/" + path;
loadModule(path);

// Start the event loop.
uv.run();

}());