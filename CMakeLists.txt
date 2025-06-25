cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(SFML_DIR "C:/SFML-2.6.1/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)

target_link_libraries(main PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)
