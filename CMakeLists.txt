cmake_minimum_required(VERSION 3.2)
project(router CXX)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
	add_compile_options(/W4 -D_WIN32_WINNT=0x0601)
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
endif()

find_package (Threads REQUIRED)

file(GLOB CPP_FILES *.cpp *.c *.hpp)
add_executable(router ${CPP_FILES})

target_link_libraries(router Threads::Threads)
