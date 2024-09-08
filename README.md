# weselowski-vdf-native.js

Bindings of [harmony-one/vdf](https://github.com/harmony-one/vdf) to Node.js
This repository provides a Node.js integration for Harmony's Verifiable Delay Function (VDF) implementation

## How it works?

1. Golang wrapper functions for the VDF library written in Golang, as every function does not need to be rewritten to be compatible with CGO.
2. Compiling the Golang wrapper functions into a shared library using CGO.
3. Node.js native addon using N-API to interface with the generated shared library.
4. Implementing a JavaScript wrapper for the native addon to simplify usage.

## Installation

### Build From Source

1. You will need to build the `lib.go` file
   `cd go && go build -o ../include/libvdf.so -buildmode=c-shared lib.go && cd ../`
2. Build the bindings with node-gyp
   `node-gyp configure && node-gyp build`
   3, If you get an error saying `/usr/bin/ld: cannot find -l:libvdf.so: No such file or directory` or similar
   You need to export the Export the Library Path
   `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/include`

## Example code

This code shows how to generate a VDF and verify it, while also showing the fact it doesnt block the event loop.

```js
const vdf = require("../lib/index");

// Testing the event loop

(async () => {
  const intervalId = setInterval(() => {
    console.log("Event loop going");
  }, 1000);

  try {
    const result = await vdf.generateVDF(stringToHex("test"), 1000, 2048);
    console.log("VDF Result:", result);

    const check = await vdf.verifyVDF(stringToHex("test"), result, 1000, 2048);

    console.log(check);
  } catch (error) {
    console.error("Failed to generate VDF:", error);
  } finally {
    clearInterval(intervalId);
  }
})();

console.log("This runs before everything else");

function stringToHex(str) {
  const buf = Buffer.from(str, "utf8");
  return buf.toString("hex");
}
```

## Notice

This project was originally written for the Highlayer project this version can be found [here](https://github.com/highlayer-team/weselowski-vdf-native.js)
The version above does have some changes made to the code commited by [Angrymouse](https://github.com/Angrymouse)

This repo was uploaded here as I officially _left_ the Highlayer team. Code was not licensed and I wasnt compensated for my work they in no way shape or form own this version.

This repo is not maintained and is only for reference of my work.
