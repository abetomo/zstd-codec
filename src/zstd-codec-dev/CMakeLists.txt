cmake_minimum_required(VERSION 3.1...3.14)

project(zstd_codec_dev VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES
        src/main.cc
        )

add_executable(${PROJECT_NAME}
        ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
        core::lib
        )
