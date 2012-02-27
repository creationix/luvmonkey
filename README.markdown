This project is an effort to create a custom SpiderMonkey runtime that has libuv built-in.  This will give it very node.js-like semantics, but using a different JavaScript engine.

Currently, the project is very alpha and exploratory.  

Come to #luvmonkey on freenode irc to discuss any questions.

## Building

At the moment it's only been tested on Ubuntu.

You need to build spidermonkey on your own.  Here are quick instructions.

### Building SpiderMonkey

First download mozilla-central (the repo that contains spidermonkey).  If you have good internet, use the mecurial client, if not, you can download a roughly 100mb tarball of the latest revision at <http://hg.mozilla.org/mozilla-central>

Once downloaded, go to the js folder and build spidermonkey.

```sh
cd mozilla-central/js/src
autoconf2.13
./configure
make -j 4 # or however many cores you have
sudo make install
```

There is currently a bug in spidermonkey's build system where "make install" doesn't install everything correctly.
(https://bugzilla.mozilla.org/show_bug.cgi?id=730910)

To work around this bug, do the following:

```sh
sudo mkdir /usr/local/include/mozilla/
sudo cp mozilla-central/mfbt/* /usr/local/include/mozilla/
sudo cp mozilla-central/js/public /usr/local/include/js/
```

### Building

Once you have SpiderMonkey, building luvmonkey is super easy.

Go to the luvmonkey source tree and do the normal configure;make dance. (Require python installed since we use gyp)

```sh
./configure
make -j4 # or however many cores you have
# Test it
./out/Release/luvmonkey test.js
```
