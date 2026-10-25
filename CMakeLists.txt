cmake_minimum_required(VERSION 3.16)
project(PixLabEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

set(PIXLAB_SOURCES
    src/Image.cpp
    src/GrayscaleFilter.cpp
    src/SepiaFilter.cpp
    src/BlurFilter.cpp
    src/ReflectFilter.cpp
    src/PixelateFilter.cpp
    src/VignetteFilter.cpp
    src/UnsharpMaskEnhancer.cpp
    src/BMPHandler.cpp
    src/main.cpp
)

if(EMSCRIPTEN)
    add_executable(pixlab_wasm ${PIXLAB_SOURCES})
    set_target_properties(pixlab_wasm PROPERTIES
        OUTPUT_NAME "pixlab_wasm"
        SUFFIX ".js"
        LINK_FLAGS "-O3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=\"['_processImage','_getOutputSize','_freeBuffer','_malloc','_free']\" -s EXPORTED_RUNTIME_METHODS=\"['ccall','cwrap']\""
    )
    set_target_properties(pixlab_wasm PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
else()
    add_executable(pixlab_cli ${PIXLAB_SOURCES})
endif()
