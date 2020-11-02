######################################
# Project settings 
######################################
# Name of the MCU, use exact name, for example stm32f405vg, this is needed by libopencm3
DEVICE = stm32f767zi

# General settings
OPT = -O3
DEBUG = 

# Source test files, currently only testing on development machine is available,
# so do not include any routines that need hardware.
# Explicitly define files used. For now only .cc files are supported 
# Test routine is invoked with make test in project directory
# Make sure to filter out test files that will not be used in real application
IMAGEFILES 	:= $(wildcard images/*.cc)
MODELFILE 	:= full_quant_model.cc 

# Source files are added here, wildcard function adds them automatically,
# if you are going to create seperate folders you have to add them by yourself.
# example: driver/motor.c -> $(wildcard driver/*.c)
CFILES   := $(wildcard src/*.c)
CXXFILES := $(wildcard src/*.cpp)
CCFILES  := $(wildcard src/*.cc)
CCFILES  += $(wildcard src/images/*.cc)
CCFILES  += $(wildcard src/model/*.cc)

# Test files are separe
TEST_LDLIBS	:= testlite_build/testlite.a
TESTFILES 	:= $(wildcard test/*.cc)


# It is needed to add archived microlite library
LIBDEPS := microlite_build/microlite.a


# If you add new folders do not forget to update the INCLUDES!
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
