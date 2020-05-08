cmake_minimum_required(VERSION 3.10)

project(tafl C)

find_package(Threads)
add_executable(tafl src/tafl.c src/taflboard.c src/taflutil.c src/ai.c)
target_link_libraries(tafl ${CMAKE_THREAD_LIBS_INIT})
target_compile_options(tafl PRIVATE -Wall -Wextra -pedantic -Werror -O3)
