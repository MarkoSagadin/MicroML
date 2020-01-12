######################################
# Project settings 
######################################
# Name of the MCU, use exact name, for example stm32f405vg, this is needed by libopencm3
DEVICE = stm32f405vg

# General settings
OPT = -Og
DEBUG = -g

# Source files are added here, wildcard function adds them automaticaly,
# if you are going to create seperate folders you have to add them by yourself.
# example: driver/motor.c -> $(wildcard driver/*.c)
CFILES = $(wildcard *.c)
CXXFILES = $(wildcard *.cpp)
CCFILES = $(wildcard *.cc)
AFILES = $(wildcard *.s)
LDLIBS = microlite_build/microlite.a

# If you add new folders do not forget to update the INCLUDES!
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
