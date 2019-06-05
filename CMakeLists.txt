cmake_minimum_required(VERSION 3.1...3.14)

project(zstd_codec VERSION 0.2.0 LANGUAGES CXX)

option(ZSTD_CODEC_EMSCRIPTEN "USE EMSCRIPTEN" OFF)

# core library
add_subdirectory(lib/zstd-codec-core)

# bidnings
if (CMAKE_JS_VERSION)
    # Node.js
    add_subdirectory(src/zstd-codec-node)
elseif(ZSTD_CODEC_EMSCRIPTEN)
    # web (WebAssembly powered by Emscripten)
    add_subdirectory(src/zstd-codec-web)
else ()
    # development edition
    add_subdirectory(src/zstd-codec-dev)
endif ()
