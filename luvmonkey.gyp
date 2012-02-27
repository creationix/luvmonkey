{
  'variables': {
  },

  'targets': [

    { 'target_name': 'luvmonkey',
      'type': 'executable',
      'dependencies': [
        'deps/uv/uv.gyp:uv',
      ],
      'conditions': [
        ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
          'cflags': [ '--std=c89', '-Wall', '-Werror' ],
          'defines': [ '_GNU_SOURCE' ]
        }],
      ],
      'include_dirs': [
        'src',
        'deps/uv/src/ares',
        'deps/mozilla-central/js/src/dist/include'
      ],
      'libraries': [
        "-ldl",
        "-lm",
        "-lrt",
        'deps/mozilla-central/js/src/libjs_static.a',
      ],
      'sources': [
        'src/luv_handle.c',
        'src/luv_stream.c',
        'src/luv_tcp.c',
        'src/luv.c',
        'src/main.c',
      ],
    }

  ],
}
