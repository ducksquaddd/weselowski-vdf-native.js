package main

/*
#include <stdlib.h>
#include <string.h>
*/
import "C"
import (
	"encoding/hex"
	"fmt"
	"unsafe"

	vdf "github.com/harmony-one/vdf/src/vdf_go"
)

//export GenerateVDFWrapper
func GenerateVDFWrapper(cSeed *C.char, iterations C.int, intSizeBits C.int) *C.char {
    // Get seed length and convert it to _Ctype_int
    seedLength := C.int(C.strlen(cSeed))
    seed := C.GoBytes(unsafe.Pointer(cSeed), seedLength)
    
    y, proofBlob := vdf.GenerateVDF(seed, int(iterations), int(intSizeBits))

    output := append(y, proofBlob...)
    outputStr := fmt.Sprintf("%x", output)
    
    return C.CString(outputStr)
}

//export VerifyVDFWrapper
func VerifyVDFWrapper(cSeed *C.char, cBlob *C.char, iterations C.int, intSizeBits C.int) C.int {
    // Convert C char* into []byte
    seedLength := C.int(C.strlen(cSeed))
    seed := C.GoBytes(unsafe.Pointer(cSeed), seedLength)
    
    // take the length of the blob and convert it from hex string
    outputHexLength := C.int(C.strlen(cBlob))
    outputHex := C.GoStringN(cBlob, outputHexLength)
    
    // Decode hex back to byte slice
    output, err := hex.DecodeString(outputHex)
    if err != nil {
        fmt.Println("Failed to decode hex string:", err)
        return 0
    }

    valid := vdf.VerifyVDF(seed, output, int(iterations), int(intSizeBits))
    if valid {
        return 1
    }
    return 0
}

func main() {
}

