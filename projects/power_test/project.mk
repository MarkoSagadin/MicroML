######################################
# Project settings 
######################################
# Name of the MCU, use exact name, for example stm32f405vg, this is needed by libopencm3
DEVICE = stm32f767zi

# General settings
OPT = -Og
DEBUG = -g

# Source files are added here, wildcard function adds them automaticaly,
# if you are going to create seperate folders you have to add them by yourself.
# example: driver/motor.c -> $(wildcard driver/*.c)
CFILES   := $(wildcard src/*.c)
CFILES   += $(wildcard src/flir/*.c)
CFILES   += $(wildcard src/simple_shell/*.c)
CFILES   += $(wildcard src/system_setup/*.c)

CXXFILES := $(wildcard src/*.cpp)

CCFILES  := $(wildcard src/*.cc)
CCFILES  += $(wildcard src/test_images/*.cc)
#CCFILES  += $(wildcard src/model/*.cc)
CCFILES  += $(wildcard src/system_setup/*.cc)
#CCFILES  += $(wildcard src/inference/*.cc)

# If you add new folders do not forget to update the INCLUDES!
INCLUDES += $(patsubst %,-I%, . src $(SHARED_DIR))

LIBDEPS := microlite_build/microlite.a
