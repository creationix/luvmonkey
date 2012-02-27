{
  'variables': {
  },

  'targets': [

    { 'target_name': 'luv',
      'type': 'static_library',
      'dependencies': [
        'deps/uv/uv.gyp:uv',
      ],
      'conditions': [
        ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
          'cflags': [ '--std=c89' ],
          'defines': [ '_GNU_SOURCE' ]
        }],
      ],
      'include_dirs': [
        'src',
        'deps/uv/src/ares',
        '/usr/local/include/js'
      ],
      'sources': [
        'src/luv_handle.c',
        'src/luv_stream.c',
        'src/luv_tcp.c',
        'src/luv.c',
      ],
    },

    { 'target_name': 'luvmonkey',
      'type': 'executable',
      'dependencies': [
        'luv'
      ],
      'conditions': [
        ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
          'cflags': [ '--std=c89' ],
          'defines': [ '_GNU_SOURCE' ]
        }],
      ],
      'include_dirs': [
        'src',
        '/usr/local/include/js'
      ],
      'ldflags': [
        "-L/usr/local/lib"
      ],
      'libraries': [
        "-lmozjs",
        "-ldl",
        "-lm",
        "-ltr"
      ],
      'sources': [
        'src/main.c',
      ],
    }

  ],
}
