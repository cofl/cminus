cmake_minimum_required(VERSION 2.8.12)
project(cminus_compiler CXX)

if(NOT DEFINED TARGET)
    SET(TARGET "cmc")
endif()

SET(GCC_CFLAGS "-g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CFLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CFLAGS}")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
    message(FATAL_ERROR "Compiler does not support C++14.")
endif()

FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

BISON_TARGET(CminusParser src/CminusParser.y ${CMAKE_CURRENT_BINARY_DIR}/CminusParser.cpp)
FLEX_TARGET(CminusScanner src/CminusScanner.l ${CMAKE_CURRENT_BINARY_DIR}/CminusScanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(CminusScanner CminusParser)

FILE(GLOB AST_SOURCES src/ast/*.cpp)
FILE(GLOB ASM_SOURCES src/asm/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR} /usr/include)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_executable(${TARGET}
    "src/main.cpp"
    "src/CminusDriver.cpp"
    "src/structures/SymbolTable.cpp"
    "src/state/State.cpp"
    ${AST_SOURCES}
    ${ASM_SOURCES}
    ${BISON_CminusParser_OUTPUTS} ${FLEX_CminusScanner_OUTPUTS})
