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
    src/graph/instruction.cc
    src/graph/block.cc
    src/graph/link.cc
    src/graph/graph.cc
    src/main.cc
)

ADD_EXECUTABLE(jarbocul ${JARBOCUL_SRCS})
TARGET_LINK_LIBRARIES(jarbocul ${JARBOCUL_LDFLAGS})
