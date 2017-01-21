{
    "targets": [
        {
            "target_name": "NativeExtension",
            "sources": [ "NativeExtension.cc", "shmcache_wrapper.cc"],
            "include_dirs" : [
 	 			"<!(node -e \"require('nan')\")",
                "/usr/include/fastcommon",
                "/usr/include/shmcache"
			],
            "libraries": [
                "-lfastcommon",
                "-lshmcache"
            ]
        }
    ],
}
