{
  'variables': {
    'library_files': [
      'src/main.js',
    ],
  },

  'targets': [
    {
      'target_name': 'js2c',
      'type': 'none',
      'toolsets': ['host'],
      'actions': [
        {
          'action_name': 'js2c',

          'inputs': [
            './tools/js2c.py',
            '<@(library_files)',
          ],

          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/js_scripts.h',
          ],

          'action': [
            'python',
            'tools/js2c.py',
            '<@(_outputs)',
            '<@(library_files)'
          ],
        },
      ],
    },

    {
      'target_name': 'luvmonkey',
      'type': 'executable',
      'dependencies': [
        # 'deps/smjs/smjs.gyp:smjs',
        'deps/uv/uv.gyp:uv',
        'deps/http-parser/http_parser.gyp:http_parser',
        'js2c#host'
      ],
      'conditions': [
        ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"', {
          'cflags': [ '--std=c89', '-Wall', '-Werror' ],
          'defines': [ '_GNU_SOURCE' ],
          'libraries': [ '-lrt' ],
        }],
        ['OS!="win"', {
          'libraries': [
            '-ldl',
            '-lm',
            '/home/tim/mozilla-central/js/src/libjs_static.a'
          ],
        }],
      ],
      'include_dirs': [
        'src',
        'deps/uv/src/ares',
        '/home/tim/mozilla-central/js/src/',
        '/home/tim/mozilla-central/js/src/dist/include/',
        '<(SHARED_INTERMEDIATE_DIR)' # for js_scripts.h
      ],
      'sources': [
        'src/lhttp_parser.c',
        'src/luv_handle.c',
        'src/luv_stream.c',
        'src/luv_tcp.c',
        'src/luv_timer.c',
        'src/luv.c',
        'src/main.c',
      ],
    },

  ],
}
