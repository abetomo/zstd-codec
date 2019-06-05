cmake_minimum_required(VERSION 3.1...3.14)

project(zstd_codec_web VERSION 0.2.0 LANGUAGES CXX)

option(ZSTD_CODEC_EMSCRIPTEN "USE EMSCRIPTEN" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES
        src/lib.cc
        )

add_library(${PROJECT_NAME}
        SHARED
        ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
        core::lib
        )

# enable emscripten headers on development environment.
if (NOT ZSTD_CODEC_EMSCRIPTEN)
    set(EMSCRIPTEN_HOME "$ENV{EMSCRIPTEN}")
    set(EMSCRIPTEN_INCLUDE "${EMSCRIPTEN_HOME}/system/include")
    target_include_directories(${PROJECT_NAME}
            PRIVATE
            ${EMSCRIPTEN_INCLUDE}
            )
endif ()

if (ZSTD_CODEC_EMSCRIPTEN)
    message(STATUS "enable the settings for Emscripten.")
    set(EMCC_LINKER_FLAGS
            "--bind --memory-init-file 0 -s ASSERTIONS=1 -s DEMANGLE_SUPPORT=1 -s MODULARIZE=1")
    set_target_properties(${PROJECT_NAME}
            PROPERTIES
            LINK_FLAGS
            ${EMCC_LINKER_FLAGS}
            )

    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".js")
endif()

