cmake_minimum_required(VERSION 2.8)
project(packJPG)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_BINARY_DIR bin)
set(CMAKE_LIBRARY_PATH lib)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-I. -O3 -Wall -pedantic -funroll-loops -ffast-math -fsched-spec-load -fomit-frame-pointer")
set(CMAKE_STATIC_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")


set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(BUILD_CONFIG_PATH ${BUILD_PATH}/config)

set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BINARY packJPG)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(SOURCES
        src/aricoder.cpp
        src/bitops.cpp
        src/packjpg.cpp
        )

set(HEADERS
        src/aricoder.h
        src/bitops.h
        src/dct8x8.h
        src/packjpglib.h
        src/packjpgdll.h
        src/pjpgtbl.h
        )

add_executable(${BINARY} ${SOURCES} ${HEADERS})
add_library(${BINARY}.a SHARED ${SOURCES} ${HEADERS})
