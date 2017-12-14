{
  "conditions": [
    ['OS=="win"', {
      "variables": {
        "GTKRoot": "C:/GTK"
      }
    }]
  ],
  "targets": [
      {
        "target_name": "node-quirc",
        "sources": [
          "src/node-quirc.cc"
        ],
        "include_dirs": [
          "<!(node -e \"require('nan')\")",
        ],
        "dependencies": [
          "deps/quirc/binding.gyp:quirc",
          "deps/omem/binding.gyp:omem",
          "node_quirc_decode"
        ],
        "conditions": [
          ['OS=="win"', {
            "libraries": [
              "-l<(GTKRoot)/lib/libpng.lib"
            ],
            "include_dirs": [
              "<(GTKRoot)/include"
            ],
          }, { # OS != "win"
            "libraries": [
              "-lpng"
            ]
          }]
        ]
    },
    {
      "target_name": "node_quirc_decode",
      "product_prefix": "lib",
      "type": "static_library",
      "sources": [
        "src/node_quirc_decode.c"
      ],
      "cflags+":   [ "-std=c++11" ],
      "cflags_c+": [ "-std=c++11" ],
      "defines":   [
        "_WIN64",
        "_XOPEN_SOURCE=700"
      ],
      "dependencies": [
        "deps/quirc/binding.gyp:quirc",
        "deps/omem/binding.gyp:omem"
      ],
      "conditions": [
        ['OS=="win"', {
          "libraries": [
            "-l<(GTKRoot)/lib/libpng.lib"
          ],
          "include_dirs": [
            "<(GTKRoot)/include"
          ],
        }, { # OS != "win"
          "libraries": [
            "-lpng"
          ]
        }]
      ]
    }
  ]
}
