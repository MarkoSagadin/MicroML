######################################
# Project settings 
######################################
# Name of the MCU, use exact name, for example stm32f405vg, this is needed by libopencm3
DEVICE = stm32f767zi

# General settings
OPT = -O3
DEBUG = -g

# Source test files, currently only testing on development machine is available,
# so do not include any routines that need hardware.
# Explicitly define files used. For now only .cc files are supported 
# Test routine is invoked with make test in project directory
TESTFILES =  cifar_test.cc cifar_model.cc model_settings.cc \
$(wildcard pictures/*.cc)
TEST_LDLIBS = testlite_build/testlite.a

# Source files are added here, wildcard function adds them automatically,
# if you are going to create seperate folders you have to add them by yourself.
# example: driver/motor.c -> $(wildcard driver/*.c)
CFILES = $(wildcard *.c)
CXXFILES = $(wildcard *.cpp)
CCFILES = $(filter-out cifar_test.cc,$(wildcard *.cc)) # Needed to filter out testfiles
AFILES = $(wildcard *.s)
# It is needed to add archived microlite library
LDLIBS = microlite_build/microlite.a


# If you add new folders do not forget to update the INCLUDES!
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
