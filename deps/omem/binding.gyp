{
    "targets": [
        {
            "target_name": "omem",
            "product_prefix": "lib",
            "type": "static_library",
            "sources": [
                "alloc.c",
                "omem-open_memstream.c",
                "omem-tmpfile.c",
                "omem-winapi-tmpfile.c"
            ],
            "direct_dependent_settings": {
                "include_dirs": [
                    "."
                ],
            }
        }
    ]
}
