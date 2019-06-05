cmake_minimum_required(VERSION 3.1...3.14)

project(zstd_codec_core VERSION 0.2.0 LANGUAGES CXX)

set(ZSTD_MULTITHREAD_SUPPORT OFF CACHE BOOL "don't support multithreads.")
set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "don't build programs.")
add_subdirectory(extern/zstd/build/cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(HEADER_FILES
        src/lib.h
        include/zstd_codec_core/ZstdApi.h
        include/zstd_codec_core/types.h
        )

set(SOURCE_FILES
        src/lib.cc
        src/ZstdApi.cc)

add_library(${PROJECT_NAME}
        ${HEADER_FILES}
        ${SOURCE_FILES})
add_library(core::lib ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        extern/zstd/lib
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include
        )

#target_link_libraries(
#        ${PROJECT_NAME}
#        PUBLIC
#        ${PROJECT_SOURCE_DIR}/extern/zstd/lib/libzstd.a
#)

target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        libzstd_static
)
