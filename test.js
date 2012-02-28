var p = require('utils').prettyPrint;
var uv = require('uv');

function bytesToHuman(bytes) {
  if (bytes > 0x40000000) {
    return (bytes / Math.pow(2, 30)).toFixed(2) + "Gb";
  }
  if (bytes > 0x100000) {
    return (bytes / Math.pow(2, 20)).toFixed(2) + "Mb";
  }
  if (bytes > 0x400) {
    return (bytes / Math.pow(2, 10)).toFixed(2) + "Kb";
  }
  return bytes + " bytes";
}


// p(global);

p("exepath", uv.exepath());
p("free memory", uv.get_free_memory(), bytesToHuman(uv.get_free_memory()));
p("total memory", uv.get_total_memory(), bytesToHuman(uv.get_total_memory()));
p("loadavg", uv.loadavg());
p("uptime", uv.uptime());

p("uv.Handle", uv.Handle);
p("uv.Handle.prototype", uv.Handle.prototype);
var h = new uv.Handle();
p("h = new uv.Handle()", h);
p("h instanceof uv.Handle", h instanceof uv.Handle);
p("h.__proto__.constructor", h.__proto__.constructor)
p("uv.Stream.prototype", uv.Stream.prototype);
var s = new uv.Stream();
p("s = new uv.Stream()", s);
p("s instanceof uv.Stream", s instanceof uv.Stream);
p("s instanceof uv.Handle", s instanceof uv.Handle);
p("s.__proto__.constructor", s.__proto__.constructor)
p("uv.Tcp.prototype", uv.Tcp.prototype);
var t = new uv.Tcp();
p("t = new uv.Tcp()", t);
p("t instanceof uv.Tcp", t instanceof uv.Tcp);
p("t instanceof uv.Stream", t instanceof uv.Stream);
p("t instanceof uv.Handle", t instanceof uv.Handle);
p("t.__proto__.constructor", t.__proto__.constructor);
p("t.close === h.close", t.close === h.close);
p("s.close === h.close", s.close === h.close);
p("t.write === s.write", t.write === s.write);
t.close()

p("args", args);
p("uv", uv);

p({
  __filename: __filename,
  __dirname: __dirname,
  exports: exports,
  module: module
});

var server = new uv.Tcp();
server.nodelay(1);
server.keepalive(1, 500);
server.bind("0.0.0.0", 8080);
server.close()

