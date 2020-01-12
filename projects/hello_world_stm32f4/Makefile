# Lets get the name of the project, it is used all over in rules.mk
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))

#Path to libopen
OPENCM3_DIR = ../../libopencm3
#Our build folder for all binaries and object files
BUILD_DIR = build

# Include project specific settings
include project.mk

#Include configuration for linker file
include $(OPENCM3_DIR)/mk/genlink-config.mk

#Include main rules
include ../../rules.mk

#Include rules for linker file
include $(OPENCM3_DIR)/mk/genlink-rules.mk
