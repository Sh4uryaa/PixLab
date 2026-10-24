CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
SRC = src/Image.cpp src/Grayscale.cpp src/Sepia.cpp src/Reflect.cpp src/Blur.cpp

.PHONY: all test clean wasm
all: pixlab_filter

pixlab_filter: src/main.cpp $(SRC)
	$(CXX) $(CXXFLAGS) -Isrc $^ -o $@

pixlab_tests: tests/filter_tests.cpp $(SRC)
	$(CXX) $(CXXFLAGS) -Isrc $^ -o $@

test: pixlab_tests
	./pixlab_tests

# Requires Emscripten (emcc). Generates pixlab_wasm.js and pixlab_wasm.wasm.
wasm:
	emcc -std=c++17 -O3 -Isrc src/pixlab_wasm.cpp -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=createPixLabModule -s EXPORTED_FUNCTIONS='["_pixlab_apply_filter","_malloc","_free"]' -s EXPORTED_RUNTIME_METHODS='[]' -o web/pixlab_wasm.js

clean:
	rm -f pixlab_filter pixlab_tests pixlab_test_fixture.bmp pixlab_test_output.bmp web/pixlab_wasm.js web/pixlab_wasm.wasm
