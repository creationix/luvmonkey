{
  'variables': {
  },
  
  'targets': [
    {
      'target_name': 'spiderluv',
      'type': 'executable',

      'dependencies': [
        'deps/http-parser/http_parser.gyp:http_parser',
        'deps/zlib/zlib.gyp:zlib',
        'deps/uv/uv.gyp:uv',
      ],

      'include_dirs': [
        'src',
        'deps/uv/src/ares',
      ],

      'defines': [
        '_GNU_SOURCE',
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
