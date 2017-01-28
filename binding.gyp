{
    "targets": [
        {
            "target_name": "<(module_name)",
            "sources": [ "NativeExtension.cc", "shmcache_wrapper.cc"],
            "include_dirs" : [
 	 			"<!(node -e \"require('nan')\")",
                "./deps/libfastcommon/src",
                "./deps/libshmcache/src"
			],
            "libraries": [
                "<!(pwd)/deps/libfastcommon/src/libfastcommon.a",
                "<!(pwd)/deps/libshmcache/src/libshmcache.a"
            ],
            "defines": [
                "__STDC_FORMAT_MACROS"
            ]
        },
        {
            "target_name": "action_after_build",
            "type": "none",
            "dependencies": [ "<(module_name)" ],
            "copies": [
                {
                    "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
                    "destination": "<(module_path)"
                }
            ]
        }
    ],
}
