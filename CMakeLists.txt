cmake_minimum_required(VERSION 2.8.12)
project(cminus_compiler)

if(NOT DEFINED TARGET)
    SET(TARGET "cmc")
endif()

SET(GCC_CFLAGS "-g")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CFLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CFLAGS}")

FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

BISON_TARGET(CminusParser src/CminusParser.ypp ${CMAKE_CURRENT_BINARY_DIR}/CminusParser.cpp)
FLEX_TARGET(CminusScanner src/CminusScanner.l ${CMAKE_CURRENT_BINARY_DIR}/CminusScanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(CminusScanner CminusParser)
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR} /usr/include)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_executable(${TARGET} ${SOURCES} ${BISON_CminusParser_OUTPUTS} ${FLEX_CminusScanner_OUTPUTS})
