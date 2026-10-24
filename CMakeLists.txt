cmake_minimum_required(VERSION 3.16)
project(PixLabImageEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(FILTER_SOURCES
    src/Image.cpp
    src/Grayscale.cpp
    src/Sepia.cpp
    src/Reflect.cpp
    src/Blur.cpp
)

add_executable(pixlab_filter src/main.cpp ${FILTER_SOURCES})
target_include_directories(pixlab_filter PRIVATE src)

include(CTest)
if(BUILD_TESTING)
    add_executable(pixlab_tests tests/filter_tests.cpp ${FILTER_SOURCES})
    target_include_directories(pixlab_tests PRIVATE src)
    add_test(NAME pixlab_tests COMMAND pixlab_tests)
endif()
