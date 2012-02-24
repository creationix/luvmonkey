This project is an effort to create a custom SpiderMonkey runtime that has libuv built-in.  This will give it very node.js-like semantics, but using a different JavaScript engine.

Currently, the project is very alpha and exploratory.  

Come to #luvmonkey on freenode irc or to discuss any questions.

## Building

At the moment it's only been tested on Ubuntu (this should change soon when gyp support lands).  Also I'm using the libmozjs package from debian to skip building spidermonkey from source.

```sh
sudo apt-get install libmozjs185-dev 
make -j4 # or however many cores you have
# Test it
./luvmonkey test.js
```
