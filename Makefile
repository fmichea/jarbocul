# Minor makefile to help build the tool.

BUILD_TYPE ?= UNSET
CMAKE_BASE_OPTIONS ?=

# Build type is required
ifeq ($(BUILD_TYPE),UNSET)
$(error Build type must be set with variable BUILD_TYPE; possible values are "release" and "debug")
endif

# Depending on the build type, we want to set a number of variables.
ifeq ($(BUILD_TYPE),release)
CMAKE_BASE_OPTIONS := $(CMAKE_BASE_OPTIONS) -DCMAKE_BUILD_TYPE=Release
else ifeq ($(BUILD_TYPE),debug)
CMAKE_BASE_OPTIONS := $(CMAKE_BASE_OPTIONS) -DCMAKE_BUILD_TYPE=Debug
else ifeq ($(BUILD_TYPE),test)
BINARY_NAME := test-jarbocul
CMAKE_BASE_OPTIONS := $(CMAKE_BASE_OPTIONS) -DCMAKE_BUILD_TYPE=Debug -DTESTING=1
else
$(error Build type "$(BUILD_TYPE)" not recognized)
endif

BUILD_DIR ?= build-$(BUILD_TYPE)
BINARY_NAME ?= jarbocul

# First check that we will not create a conflict with the following rules and
# the name of the build directory. Let's just forbid that name, it's OK.
ifeq ($(BUILD_DIR),build-jarbocul)
$(error Build directory cannot be named "build-jarbocul" per convention)
endif

.PHONY: all show-bin build-jarbocul clean

all: build-jarbocul

build-jarbocul: $(BUILD_DIR)
	if ! [ -f "$(BUILD_DIR)/Makefile" ]; then \
		cd $(BUILD_DIR); \
		cmake .. $(CMAKE_BASE_OPTIONS) $(CMAKE_OPTIONS); \
	fi
	make -C $(BUILD_DIR)

start: build-jarbocul
	@echo "[+] Running binary from $(PWD):"
	@time ./$(BUILD_DIR)/$(BINARY_NAME) $(BINARY_ARGS)

show-bin:
	@make BUILD_TYPE=$(BUILD_TYPE) 1>&2
	@echo ./$(BUILD_DIR)/jarbocul

clean:
	test -d "$(BUILD_DIR)" && rm -r ./$(BUILD_DIR) || true

$(BUILD_DIR):
	mkdir -p $@
