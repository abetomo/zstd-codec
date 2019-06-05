#include <iostream>

#include "napi.h"

#include "zstd_codec_core/ZstdApi.h"
#include "zstd_codec_core/types.h"

int lib_main() {
    Vec<u8> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Vec<u8> buff;

    const auto result = ZstdApi().compress(buff, data, 5);
    const auto status = result.isOk() ? "success" : "error";
    std::cout << "status: " << status << ", buff.size(): " << buff.size() << std::endl;

    return 0;
}

Napi::String Method(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    lib_main();

    return Napi::String::New(env, "world");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "hello"),
                Napi::Function::New(env, Method));
    return exports;
}

NODE_API_MODULE(hello, Init)

