project(CppDiagHelper)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

include_directories("src/")
add_compile_options(-pedantic -Wall -Wextra -Wshadow -Werror)

find_package(Curses REQUIRED)

add_executable(CppDiagHelper src/main.cc src/ui.cc)
target_compile_options(CppDiagHelper PRIVATE "$<$<CONFIG:Debug>:-fsanitize=address,undefined,leak>")
set(
	CMAKE_EXE_LINKER_FLAGS_DEBUG
	"${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address,undefined,leak"
) # Костыль для cmake 3.12
# target_link_options(CppDiagHelper PRIVATE "$<$<CONFIG:Debug>:-fsanitize=address,undefined,leak>")
target_include_directories(CppDiagHelper PRIVATE ${CURSES_INCLUDE_DIR})
target_link_libraries(CppDiagHelper PRIVATE ${CURSES_LIBRARIES})
