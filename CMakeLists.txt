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

# Sources
set(SOURCES
        src/aricoder.cpp
        src/bitops.cpp
        src/packjpg.cpp
        )
# Headers
set(HEADERS
        src/aricoder.h
        src/bitops.h
        src/dct8x8.h
        src/packjpglib.h
        src/packjpgdll.h
        src/pjpgtbl.h
        )

include(FindOpenMP)

if(DEBUG)
    message("Debug is on")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb3")
endif(DEBUG)

if(OPENMP_FOUND)
    message("OpenMP is found")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else(OPENMP_FOUND)
    message("ERROR: OpenMP could not be found.")
endif(OPENMP_FOUND)

add_executable(${BINARY} ${SOURCES} ${HEADERS} src/symbol.h src/coder_limits.h src/table.h src/table_s.h src/model_s.cpp src/model_s.h)
add_library(${BINARY}.a SHARED ${SOURCES} ${HEADERS})