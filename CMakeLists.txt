cmake_minimum_required(VERSION 3.20)
project(stream_pc_to_pi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(pc_sender pc/src/main.cpp)
target_include_directories(pc_sender PRIVATE common/include)

add_executable(pi_receiver pi/src/main.cpp)
target_include_directories(pi_receiver PRIVATE common/include)

foreach(target pc_sender pi_receiver)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
    )
endforeach()
