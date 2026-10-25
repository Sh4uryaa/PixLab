CXX ?= g++
CXXFLAGS ?= -std=c++17 -O3 -Wall -Wextra -Wpedantic -Iinclude

SRC = src/Image.cpp \
      src/GrayscaleFilter.cpp \
      src/SepiaFilter.cpp \
      src/BlurFilter.cpp \
      src/ReflectFilter.cpp \
      src/PixelateFilter.cpp \
      src/VignetteFilter.cpp \
      src/UnsharpMaskEnhancer.cpp \
      src/BMPHandler.cpp

.PHONY: all wasm cli clean

all: wasm

# Native command-line executable
cli: src/main.cpp $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o pixlab_cli

# Emscripten WebAssembly compilation to pixlab_wasm.js & pixlab_wasm.wasm
wasm: src/main.cpp $(SRC)
	emcc -std=c++17 -O3 -Iinclude $^ \
		-s WASM=1 \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s EXPORTED_FUNCTIONS="['_processImage','_getOutputSize','_freeBuffer','_malloc','_free']" \
		-s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
		-o pixlab_wasm.js

clean:
	rm -f pixlab_cli pixlab_wasm.js pixlab_wasm.wasm
