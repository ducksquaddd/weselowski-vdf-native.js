{
  "targets": [
    {
      "target_name": "weselowski_vdf_bindings",
      "sources": [
        "src/vdf-bindings.cpp"
      ],
      "include_dirs": [
        "./node_modules/node-addon-api",
        "./include/"
      ],
       "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "libraries": [
        "-L<(module_root_dir)/include",
        "-lvdf" 
      ],
      "cflags_cc": [ "-fexceptions" ],
      "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      "conditions": [
        ['OS=="linux"', {
          "ldflags": [
            "-Wl,-rpath,'$$ORIGIN/'"
          ]
        }],
      ]
    }
  ]
}
