This project is an effort to create a custom SpiderMonkey runtime that has libuv
built-in.  This will give it very node.js-like semantics, but using a different
JavaScript engine.

Currently, the project is very alpha and exploratory.  

Come to #luvmonkey on freenode irc to discuss any questions.

## Building

At the moment it's only tested on Ubuntu 10.04, 11.04 and OS X 10.7.

Building luvmonkey is super easy. Simply go to the luvmonkey source tree and
do the normal `./configure && make` dance. Python 2.5 or newer (but *not* 3.x)
is required because we use gyp.

```sh
cd $HOME/luvmonkey
./configure
make -j4 # or however many cores you have
# Test it
./out/Debug/luvmonkey test.js
```
