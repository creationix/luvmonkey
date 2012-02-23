OS_NAME=$(shell uname -s)
ifeq (${OS_NAME},Darwin)
LDFLAGS=-framework CoreServices
else ifeq (${OS_NAME},Linux)
LDFLAGS=-Wl,-E -lrt
endif
LDFLAGS+=-lmozjs185

YAJLDIR=deps/yajl
UVDIR=deps/uv
HTTPDIR=deps/http-parser

DEPS=${YAJLDIR}/yajl.a  \
     ${UVDIR}/uv.a      \
     ${HTTPDIR}/http_parser.o

PARTS=build/luv.o \
      build/main.o

export Q=

all: spiderluv

${YAJLDIR}/CMakeLists.txt:
	git submodule update --init ${YAJLDIR}

${YAJLDIR}/Makefile: deps/Makefile.yajl ${YAJLDIR}/CMakeLists.txt
	cp deps/Makefile.yajl ${YAJLDIR}/Makefile

${YAJLDIR}/yajl.a: ${YAJLDIR}/Makefile
	rm -rf ${YAJLDIR}/src/yajl
	cp -r ${YAJLDIR}/src/api ${YAJLDIR}/src/yajl
	$(MAKE) -C ${YAJLDIR}

${UVDIR}/Makefile:
	git submodule update --init ${UVDIR}

${UVDIR}/uv.a: ${UVDIR}/Makefile
	$(MAKE) -C ${UVDIR} uv.a

${HTTPDIR}/Makefile:
	git submodule update --init ${HTTPDIR}

${HTTPDIR}/http_parser.o: ${HTTPDIR}/Makefile
	${MAKE} -C ${HTTPDIR} http_parser.o

spiderluv: ${PARTS} ${DEPS}
	$(CC) -g -o $@ ${PARTS} ${DEPS} ${LDFLAGS}

build/%.o: src/%.c src/%.h ${DEPS}
	mkdir -p build
	$(CC) --std=c89 -D_GNU_SOURCE -g -Wall -Werror -c $< -o $@ -I${HTTPDIR} -I${UVDIR}/include -I${YAJLDIR}/src/api -I${YAJLDIR}/src -I/usr/include/js -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

clean:
	${MAKE} -C ${HTTPDIR} clean
	${MAKE} -C ${YAJLDIR} clean
	${MAKE} -C ${UVDIR} distclean
	rm -rf build spiderluv

.PHONY: all clean
