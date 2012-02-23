OS_NAME=$(shell uname -s)
ifeq (${OS_NAME},Darwin)
LDFLAGS=-framework CoreServices
else ifeq (${OS_NAME},Linux)
LDFLAGS=-Wl,-E -lrt -lm
endif

UVDIR=../deps/uv
HTTPDIR=../deps/http-parser

DEPS=${UVDIR}/uv.a      \
     ${HTTPDIR}/http_parser.o

export Q=

all: http-simple


${UVDIR}/Makefile:
	git submodule update --init ${UVDIR}

${UVDIR}/uv.a: ${UVDIR}/Makefile
	$(MAKE) -C ${UVDIR} uv.a

${HTTPDIR}/Makefile:
	git submodule update --init ${HTTPDIR}

${HTTPDIR}/http_parser.o: ${HTTPDIR}/Makefile
	${MAKE} -C ${HTTPDIR} http_parser.o

http-simple: http-simple.o ${DEPS}
	$(CC) -g -o $@ $< ${DEPS} ${LDFLAGS}

%.o: %.c ${DEPS}
	$(CC) --std=c89 -D_GNU_SOURCE -g -Wall -Werror -c $< -o $@ -I${HTTPDIR} -I${UVDIR}/include -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

clean:
	rm -f http-simple http-simple.o

distclean: clean
	${MAKE} -C ${HTTPDIR} clean
	${MAKE} -C ${UVDIR} distclean

.PHONY: all clean distclean
