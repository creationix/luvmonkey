
var colors = {
  black:    "0;30",
  red:      "0;31",
  green:    "0;32",
  yellow:   "0;33",
  blue:     "0;34",
  magenta:  "0;35",
  cyan:     "0;36",
  white:    "0;37",
  Bblack:   "1;30",
  Bred:     "1;31",
  Bgreen:   "1;32",
  Byellow:  "1;33",
  Bblue:    "1;34",
  Bmagenta: "1;35",
  Bcyan:    "1;36",
  Bwhite:   "1;37"
};

exports.color = color;
function color(colorName) {
  return "\033[" + (colors[colorName] || "0") + "m";
}

exports.colorize = colorize;
function colorize(colorName, string, resetName) {
  return color(colorName) + string + color(resetName);
}

var backslash = colorize("Bgreen", "\\\\", "green");
var nullbyte  = colorize("Bgreen", "\\0", "green");
var newline   = colorize("Bgreen", "\\n", "green");
var carraige  = colorize("Bgreen", "\\r", "green");
var tab       = colorize("Bgreen", "\\t", "green");
var quote     = colorize("Bgreen", '"', "green");
var quote2    = colorize("Bgreen", '"');
var obracket  = colorize("white", '[');
var cbracket  = colorize("white", ']');
var obrace    = colorize("white", '{');
var cbrace    = colorize("white", '}');

exports.forEach = forEach;
function forEach(o, callback, thisp) {
  if (Array.isArray(o)) {
    return o.forEach(callback, thisp);
  }
  if (!callback) print(o, callback, thisp);
  var keys = Object.getOwnPropertyNames(o);
  for (var i = 0, l = keys.length; i < l; i++) {
    var key = keys[i];
    callback.call(thisp, o[key], key, o);
  }
}

exports.dump = dump;
function dump(o) {
  var seen = [];
  function realDump(o, depth) {
    var t = typeof o;
    if (t === 'string') {
      return quote + o.replace(/\\/g, backslash).replace(/\0/g, nullbyte).replace(/\n/g, newline).replace(/\r/g, carraige).replace(/\t/g, tab) + quote2;
    }
    if (o === null) {
      return colorize("Bwhite", "" + o);
    }
    if (t === 'undefined') {
      return colorize("Bblack", "" + o);
    }
    if (t === 'boolean') {
      return colorize("yellow", "" + o);
    }
    if (t === 'number') {
      return colorize("blue", "" + o);
    }
    if (t === 'xml') {
      return colorize('Bmagenta', "" + o)
    }
    if (t === 'function') {
      return colorize("cyan", "[Function" + (o.name ? ": " + o.name : "") + "]");
    }
    if (t === 'object') {
      if (depth > 1 || seen.indexOf(o) >= 0) {
        return colorize("yellow", "" + o);
      }
      seen.push(o);
      var indent = (new Array(depth + 1)).join("  ");
      var isArray = Array.isArray(o);
      var first = true;
      var lines = [];
      var estimated = 0;

      forEach(o, function (v, k) {
        var s
        if (isArray) {
          s = "";
        } else {
          if (typeof k === "string" && (/^[a-z_$][a-z0-9_$]*$/i).test(k)) {
            s = k + ": ";
          } else {
            s = realDump(k, 100) + ": ";
          }
        }
        s = s + realDump(v, depth + 1);
        lines.push(s);
        estimated += s.length;
      });
      var open = isArray ? obracket : obrace;
      var close = isArray ? cbracket : cbrace;
      if (estimated > 200) {
        return open + "\n  " + indent + lines.join(",\n  " + indent) + "\n" + indent + close;
      }
      return open + " " + lines.join(", ") + " " + close;
    }
    return "" + o;
  }
  return realDump(o, 0);
}

exports.prettyPrint = prettyPrint;
function prettyPrint() {
  print.apply(this, Array.prototype.map.call(arguments, dump));
}
