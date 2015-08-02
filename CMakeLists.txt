PROJECT(jarbocul)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# CXX compiler
SET(CMAKE_CXX_COMPILER "clang++")

# CXXFLAGS used by compiler.
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DRELEASE")
SET(CMAKE_CXX_FLAGS_DEBUG "-g3 -DDISASSEMBLER")

# LDFLAGS used when linking jarbocul.
SET(JARBOCUL_LDFLAGS "")
SET(JARBOCUL_SRCS
    src/graph/block.cc
    src/graph/graph.cc
    src/graph/instruction.cc
    src/graph/link.cc
    src/main.cc
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

ADD_EXECUTABLE(jarbocul ${JARBOCUL_SRCS} ${JARBOCUL_DEFINITIONS})
TARGET_LINK_LIBRARIES(jarbocul ${JARBOCUL_LDFLAGS})
