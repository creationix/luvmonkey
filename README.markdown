This project is an effort to create a custom SpiderMonkey runtime that has libuv built-in.  This will give it very node.js-like semantics, but using a different JavaScript engine.

Currently, the project is very alpha and exploratory.  

Come to #luvmonkey on freenode irc to discuss any questions.

## Building

At the moment it's only been tested on Ubuntu.

You need to build spidermonkey on your own.  Here are quick instructions.

Install spidermonkey version 185.  On ubuntu this is libmozjs185-dev.

Go to the luvmonkey source tree and do the normal configure;make dance. (Requires python installed since we use gyp)

```sh
cd $HOME/luvmonkey
./configure
make -j4 # or however many cores you have
# Test it
./out/Release/luvmonkey test.js
```
