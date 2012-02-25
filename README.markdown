This project is an effort to create a custom SpiderMonkey runtime that has libuv built-in.  This will give it very node.js-like semantics, but using a different JavaScript engine.

Currently, the project is very alpha and exploratory.  

Come to #luvmonkey on freenode irc to discuss any questions.

## Building

At the moment it's only been tested on Ubuntu.  Also I'm using the libmozjs package from debian to skip building spidermonkey from source.

The new gyp build seems broken right now, but here is how you would build it.

```sh
sudo apt-get install libmozjs185-dev 
./configure
make -j4 # or however many cores you have
# Test it
./out/Release/luvmonkey test.js
```
