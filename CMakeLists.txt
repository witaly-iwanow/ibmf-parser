cmake_minimum_required(VERSION 3.10)
project(ibmf-parser VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(IBMF_API_HEADERS include/ibmf/box.h include/ibmf/parser.h include/ibmf/stream-reader.h)
set(IBMF_SOURCES src/parser.cpp)

file(GLOB_RECURSE THIRD_PARTY_HEADERS third-party/*.h third-party/*.hpp)
file(GLOB_RECURSE THIRD_PARTY_SOURCES third-party/*.c third-party/*.cpp third-party/*.cxx)

add_executable(ibmf-parser main.cpp base64-to-file.h file-stream-reader.h ${THIRD_PARTY_HEADERS} ${THIRD_PARTY_SOURCES} ${IBMF_API_HEADERS} ${IBMF_SOURCES})
target_include_directories(ibmf-parser PUBLIC include)
