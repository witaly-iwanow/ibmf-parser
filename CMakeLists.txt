cmake_minimum_required(VERSION 3.10)
project(ibmf-parser VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(IBMF_API_HEADERS include/ibmf/box.h include/ibmf/parser.h include/ibmf/stream-reader.h)
set(IBMF_SOURCES src/parser.cpp)

add_executable(ibmf-parser main.cpp file-stream-reader.h ${IBMF_API_HEADERS} ${IBMF_SOURCES})
target_include_directories(ibmf-parser PUBLIC include)
