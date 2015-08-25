# Minor makefile to help build the tool.

BUILD_TYPE ?= release
CMAKE_BASE_OPTIONS :=

# Depending on the build type, we want to set a number of variables.
ifeq ($(BUILD_TYPE),release)
BUILD_DIR ?= build
else ifeq ($(BUILD_TYPE),debug)
BUILD_DIR ?= build-dev
CMAKE_BASE_OPTIONS := $(CMAKE_BASE_OPTIONS) -DCMAKE_BUILD_TYPE=Debug
else
$(error Build type "$(BUILD_TYPE)" not recognized)
endif

# First check that we will not create a conflict with the following rules and
# the name of the build directory. Let's just forbid that name, it's OK.
ifeq ($(BUILD_DIR),build-jarbocul)
$(error Build directory cannot be named "build-jarbocul" per convention)
endif

.PHONY: all build-jarbocul

all: build-jarbocul

build-jarbocul: $(BUILD_DIR)
	if ! [ -f "$(BUILD_DIR)/Makefile" ]; then \
		cd $(BUILD_DIR); \
		cmake .. $(CMAKE_BASE_OPTIONS) $(CMAKE_OPTIONS); \
	fi
	make -C $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $@
