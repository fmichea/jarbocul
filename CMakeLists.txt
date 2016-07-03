PROJECT(jarbocul)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Require Boost for compilation.
FIND_PACKAGE(Boost REQUIRED)

# CXX compiler
SET(CMAKE_CXX_COMPILER "clang++")

# CXXFLAGS used by compiler.
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DRELEASE")
SET(CMAKE_CXX_FLAGS_DEBUG "-g3 -DDEBUG")

# LDFLAGS used when linking jarbocul.
SET(JARBOCUL_LDFLAGS "")

# All the include directories for the project.
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/src")

# These sources are common to the test suite and the main binary (all files but
# the file defining the main function).
SET(JARBOCUL_COMMON_SRCS
    src/graph/block.cc
    src/graph/link.cc
    src/lib/file_reader.cc
    src/lib/flowtype.cc
    src/lib/ostream_writable.cc
    src/processors/gb_z80/cpu.cc
    src/processors/gb_z80/disassembler.cc
)

SET(JARBOCUL_DEFINITIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/processors/gb_z80/opcodes.gdef
)

# Compile definitions for GameBoy z80 processor.
ADD_CUSTOM_COMMAND(
    OUTPUT ${JARBOCUL_DEFINITIONS}
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/src/processors/gb_z80/gen_opcode_defs.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/processors/gb_z80/gen_opcode_defs.py
)

# Build the main binary.
IF (TESTING EQUAL 1)
    # Add rules to install Google Test.
    INCLUDE(ExternalProject)

    ExternalProject_Add(gtest
        URL https://github.com/google/googletest/archive/release-1.7.0.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        DOWNLOAD_NO_PROGRESS 1
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(gtest source_dir binary_dir)
    SET(GTEST_INCLUDE_DIRS "${source_dir}/include")
    SET(GTEST_LIBRARIES "${binary_dir}/libgtest.a")

    # Add GTest to include directories.
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

    # Testing source files.
    SET(JARBOCUL_TESTS_SRCS
        ${JARBOCUL_COMMON_SRCS}
        tests/main.cc
        tests/lib/addroffset.cc
    )

    # Build the test binary.
    ADD_EXECUTABLE(test-jarbocul ${JARBOCUL_TESTS_SRCS} ${JARBOCUL_DEFINITIONS})
    ADD_DEPENDENCIES(test-jarbocul gtest)
    TARGET_LINK_LIBRARIES(test-jarbocul ${JARBOCUL_LDFLAGS} ${GTEST_LIBRARIES})
ELSE ()
    SET(JARBOCUL_SRCS src/main.cc ${JARBOCUL_COMMON_SRCS})
    ADD_EXECUTABLE(jarbocul ${JARBOCUL_SRCS} ${JARBOCUL_DEFINITIONS})
    TARGET_LINK_LIBRARIES(jarbocul ${JARBOCUL_LDFLAGS})
ENDIF ()
