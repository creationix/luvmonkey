This project is an effort to create a custom SpiderMonkey runtime that has libuv built-in.  This will give it very node.js-like semantics, but using a different JavaScript engine.

Currently, the project is very alpha and exploratory.  

Come to #luvmonkey on freenode irc to discuss any questions.

## Building

At the moment it's only been tested on Ubuntu.

You need to build spidermonkey on your own.  Here are quick instructions.

### Building SpiderMonkey

First download mozilla-central (the repo that contains spidermonkey).  If you have good internet, use the mecurial client, if not, you can download a roughly 100mb tarball of the latest revision at <http://hg.mozilla.org/mozilla-central>

Once downloaded, go to the js folder and build spidermonkey as a static library

```sh
cd mozilla-central/js/src
autoconf2.13
./configure --disable-shared-js
make -j 4 # or however many cores you have
```

### Building

Once you have SpiderMonkey, link it in the deps folder of luvmonkey.

```sh
cd $HOME/luvmonkey/deps
ln -s $HOME/mozilla-central
```

Then building luvmonkey is super easy. Simply go to the luvmonkey source tree and do the normal configure;make dance. (Requires python installed since we use gyp)

```sh
cd $HOME/luvmonkey
./configure
make -j4 # or however many cores you have
# Test it
./out/Release/luvmonkey test.js
```
