#include <napi.h>
#include <vector>
#include "libvdf.h"

class VDFAsyncWorker: public Napi::AsyncWorker {
  public: enum class Operation {
    Generate,
    Verify
  };

  private: Operation operation;
  std::string seed;
  std::string proof_blob; // Used for verification only
  int iterations;
  int intSizeBits;
  std::string result;
  int verifyResult; 
  Napi::Promise::Deferred deferred;

  public:
    VDFAsyncWorker(Napi::Env env, std::string seed, int iterations, int intSizeBits, Napi::Promise::Deferred deferred): Napi::AsyncWorker(env),
  operation(Operation::Generate),
  seed(std::move(seed)),
  iterations(iterations),
  intSizeBits(intSizeBits),
  deferred(deferred) {}

  VDFAsyncWorker(Napi::Env env, std::string seed, std::string proof_blob, int iterations, int intSizeBits, Napi::Promise::Deferred deferred): Napi::AsyncWorker(env),
  operation(Operation::Verify),
  seed(std::move(seed)),
  proof_blob(std::move(proof_blob)),
  iterations(iterations),
  intSizeBits(intSizeBits),
  deferred(deferred) {}

  void Execute() override {
    switch (operation) {
    case Operation::Generate: {
      // Using const_cast to cast away constness
      char * genResult = GenerateVDFWrapper(const_cast < char * > (seed.c_str()), iterations, intSizeBits);
      if (genResult != nullptr) {
        result = std::string(genResult);
        free(genResult); // Assuming the API allocates memory for the result
      } else {
        SetError("VDF generation failed");
      }
      break;
    }
    case Operation::Verify: {
      // Similar workaround with const_cast, and same caution applies
      verifyResult = VerifyVDFWrapper(const_cast < char * > (seed.c_str()), const_cast < char * > (proof_blob.c_str()), iterations, intSizeBits);
      break;
    }
    }
  }

void OnOK() override {
  Napi::Env env = Env();
  if (operation == Operation::Generate) {
    deferred.Resolve(Napi::String::New(env, result));
  } else {
    bool boolResult = verifyResult != 0;
    deferred.Resolve(Napi::Boolean::New(env, boolResult));
  }
}
};

Napi::Value GenerateVDF(const Napi::CallbackInfo & info) {
  Napi::Env env = info.Env();
if (info.Length() != 3 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber()) {
    Napi::TypeError::New(env, "Expected: (seed: string, iterations: int, intSizeBits: int)").ThrowAsJavaScriptException();
     return env.Null();
}

  std::string seed = info[0].As < Napi::String > ();
  int iterations = info[1].As < Napi::Number > ().Int32Value();
  int intSizeBits = info[2].As < Napi::Number > ().Int32Value();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  auto worker = new VDFAsyncWorker(env, seed, iterations, intSizeBits, deferred);
  worker -> Queue();

  return deferred.Promise();
}

Napi::Value VerifyVDF(const Napi::CallbackInfo & info) {
  Napi::Env env = info.Env();
  if (info.Length() != 4 || !info[0].IsString() || !info[1].IsString() || !info[2].IsNumber() || !info[3].IsNumber()) {
    Napi::TypeError::New(env, "(seed: string, proof_blob: string, iterations: int, intSizeBits: int)");
     return env.Null();
  }

  std::string seed = info[0].As < Napi::String > ();
  std::string proof_blob = info[1].As < Napi::String > ();
  int iterations = info[2].As < Napi::Number > ().Int32Value();
  int intSizeBits = info[3].As < Napi::Number > ().Int32Value();
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

  auto worker = new VDFAsyncWorker(env, seed, proof_blob, iterations, intSizeBits, deferred);
  worker -> Queue();

  return deferred.Promise();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("generateVDF", Napi::Function::New(env, GenerateVDF, "generateVDF"));
  exports.Set("verifyVDF", Napi::Function::New(env, VerifyVDF, "verifyVDF"));
  return exports;
}

NODE_API_MODULE(weselowski_vdf_bindings, Init)