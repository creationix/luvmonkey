var p = require('utils').prettyPrint;

var http_parser = require('http_parser');
var parseUrl = require('http_parser').parseUrl;
var HttpParser = require('http_parser').HttpParser;
p("http_parser", http_parser);

var url = parseUrl("http://github.com:6060/creationix/luvmonkey?test=true#frag");
p("url", url);


var parser = new HttpParser();
p("parser", parser);
parser.reinitialize("response");
var strings = [
	"Content-Type: application/javascript\r\n",
	"Content-Length: 6\r\n",
	"\r\n",
	"Hello\n"
];
strings.forEach(function (string) {
	parser.execute(string, 0, string.length);
});
parser.finish();
