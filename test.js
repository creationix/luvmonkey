var p = require('utils').prettyPrint;

var Timer = require('uv').Timer;

var timer = new Timer();
var timer2 = new Timer();

timer.start(2000, 0, function onTimer() {
  p("on_timeout");
  timer2.stop();
  timer.stop();
  timer.close(p);
  timer2.close(p);
});

timer2.start(333, 333, function onInterval() {
  p("on_interval");
  var period = timer2.getRepeat();
  p("period", period);
  timer2.setRepeat(period / 1.2 + 1);
});

p(timer, timer2);
