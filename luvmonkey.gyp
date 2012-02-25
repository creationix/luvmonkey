{
  'variables': {
  },
  
  'targets': [
    {
      'target_name': 'luv',
      'type': 'static_library',
      'dependencies': [
        'deps/uv/uv.gyp:uv',
      ],
      'include_dirs': [
        'src',
        'deps/uv/src/ares',
        '/usr/include/js'
      ],
      'conditions': [
        ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
          'cflags': [ '--std=c89' ],
          'defines': [ '_GNU_SOURCE' ]
        }],
      ],
      'sources': [
        'src/luv_handle.c',
        'src/luv_stream.c',
        'src/luv_tcp.c',
      ],
    },
    {
      'target_name': 'luvmonkey',
      'type': 'executable',

      'dependencies': [
        'luv'
      ],

      'include_dirs': [
        'src',
        'deps/uv/src/ares',
        '/usr/include/js'
      ],

      'libraries': [
        '-lmozjs185',
      ],

      'sources': [
        'src/main.c',
      ],
    }
  ],
}
