{
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
                "node_quirc_decode"
            ],
        },
        {
            "target_name": "node_quirc_decode",
            "product_prefix": "lib",
            "type": "static_library",
            "sources": [
                "src/node_quirc_decode.c"
            ],
            "libraries": [
                "-lpng"
            ],
            "dependencies": [
                "deps/quirc/binding.gyp:quirc",
            ],
            "direct_dependent_settings": {
                "libraries": [
                    "-lpng"
                ]
            }
        }
    ]
}
