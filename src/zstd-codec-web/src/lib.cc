#include <iostream>

#include <emscripten/bind.h>

#include "zstd_codec_core/ZstdApi.h"
#include "zstd_codec_core/types.h"

using namespace emscripten;

int lib_main() {
    Vec<u8> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Vec<u8> buff;

    const auto result = ZstdApi().compress(buff, data, 5);
    const auto status = result.isOk() ? "success" : "error";
    std::cout << "status: " << status << ", buff.size(): " << buff.size() << std::endl;

    return 0;
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("lib_main", &lib_main);
}
