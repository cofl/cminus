cmake_minimum_required(VERSION 2.8.12)
project(cminus_compiler CXX)

if(NOT DEFINED TARGET)
    SET(TARGET "cmc")
endif()

SET(GCC_CFLAGS "-g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CFLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CFLAGS}")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS+CXX1z)
    if(COMPILER_SUPPORTS_CXX1z)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
    else()
        message(FATAL_ERROR "Compiler does not support C++17 or C++1z, cannot continue.")
    endif()
endif()

FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

BISON_TARGET(CminusParser src/CminusParser.ypp ${CMAKE_CURRENT_BINARY_DIR}/CminusParser.cpp)
FLEX_TARGET(CminusScanner src/CminusScanner.l ${CMAKE_CURRENT_BINARY_DIR}/CminusScanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(CminusScanner CminusParser)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR} /usr/include)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_executable(${TARGET}
    "src/main.cpp"
    "src/CminusDriver.cpp"
    "src/structures/ASTToken.cpp"
    "src/structures/SymbolTable.cpp"
    ${BISON_CminusParser_OUTPUTS} ${FLEX_CminusScanner_OUTPUTS})
