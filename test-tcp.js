var p = require('utils').prettyPrint;

var Tcp = require('uv').Tcp;

var server = new Tcp();
server.bind("0.0.0.0", 1337);
server.listen(128, function () {
  var client = new Tcp();
  server.accept(client);
  client.readStart();
  client.onData = function (chunk) {
    p("onData", chunk);
    client.write(chunk, function () {
      p("onWrite", chunk);
    });
  };
  client.onEnd = function () {
    p("onEnd");
    client.close();
  };
  p("Connection!", client);
});

p(server);