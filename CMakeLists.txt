project(CppDiagHelper)
cmake_minimum_required(VERSION 3.1)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

include_directories("src/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wshadow -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG")

add_executable(CppDiagHelper src/main.cpp src/TextUI.cpp)
target_compile_features(CppDiagHelper PRIVATE cxx_std_17)
target_link_libraries(CppDiagHelper ${CURSES_LIBRARIES})
