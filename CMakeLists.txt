project(CppDiagHelper)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

include_directories("src/")
add_compile_options(-pedantic -Wall -Wextra -Wshadow -Werror)
add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined,leak>")
add_link_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined,leak>")

find_package(Curses REQUIRED)

add_executable(CppDiagHelper src/main.cpp src/TextUI.cpp)
target_link_libraries(CppDiagHelper ${CURSES_LIBRARIES})
target_include_directories(CppDiagHelper PRIVATE ${CURSES_INCLUDE_DIR})
