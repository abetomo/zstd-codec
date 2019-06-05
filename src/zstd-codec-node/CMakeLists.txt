cmake_minimum_required(VERSION 3.1...3.14)

project(zstd_codec_node VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES
        src/lib.cc
        )

add_library(${PROJECT_NAME}
        SHARED
        ${SOURCE_FILES})

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        NAPI_DISABLE_CPP_EXCEPTIONS
        )

target_link_libraries(${PROJECT_NAME}
        core::lib
        )

# enable emscripten headers on development environment.
if (NOT CMAKE_JS_VERSION)
    # TODO: include nodejs libraries
    # - CMAKE_JS_INC
    # - CMAKE_JS_LIB
    set(CMAKE_JS_INC /Users/hirakawa.yoshihito/.ndenv/versions/v10.15.3/include/node)
    message(STATUS "build for development environment.")
endif ()

if (CMAKE_JS_VERSION)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".node")
endif ()

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_JS_INC}
        extern/node-addon-api
        )

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_JS_LIB}
        )
