{
    "targets": [
        {
            "target_name": "omem",
            "product_prefix": "lib",
            "type": "static_library",
            "sources": [
                "alloc.c",
                
            ],
            "direct_dependent_settings": {
                "include_dirs": [
                    "."
                ],
            },
            "conditions": [
              ['OS=="win"', {
                "sources": [
                  "omem-winapi-tmpfile.c"
                ]
              }, {
                "sources": [
                  "omem-fopencookie.c",
                  "omem-funopen",
                  "omem-open_memstream.c",
                  "omem-tmpfile.c",
                ]
              }]
            ]
        }
    ]
}
