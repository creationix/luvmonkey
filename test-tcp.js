var p = require('utils').prettyPrint;

var Tcp = require('uv').Tcp;

var server = new Tcp();
server.bind("0.0.0.0", 1337);
server.listen(128, function () {
	var client = new Tcp();
	server.accept(client);
	client.readStart(function (chunk) {
		p("onRead", chunk);
	});
	p("Connection!", client);
});

p(server);