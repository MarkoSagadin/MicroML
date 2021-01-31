################################################################################
# Make Verbosity															   #
################################################################################
# Be silent per default, but 'make V=1' will show all compiler calls.
# If you want, V=99 will print out all sorts of things.
V?=0
ifeq ($(V),0)
Q	:= @
NULL	:= 2>/dev/null
endif

#For checking macros, to use it write make print-VARIABLE
print-%  : ; @echo $* = $($*)

################################################################################
# Tool paths																   #
################################################################################
PREFIX	?= arm-none-eabi-
CC	= $(PREFIX)gcc
CXX = $(PREFIX)g++
LD	= $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
AR = $(PREFIX)ar
OBJCOPY	= $(PREFIX)objcopy
OBJDUMP	= $(PREFIX)objdump
SIZE	= $(PREFIX)size
OPENOCD = openocd
MINICOM = minicom


################################################################################
# Includes																	   #
################################################################################
TENSORFLOW_DIR = ../../tensorflow
THIRD_PARTY_DIR = $(TENSORFLOW_DIR)/tensorflow/lite/micro/tools/make/downloads
OPENCM3_INC = $(OPENCM3_DIR)/include
# Inclusion of library header files
INCLUDES += $(patsubst %,-I%, . $(OPENCM3_INC) )
INCLUDES += \
-I. \
-I$(TENSORFLOW_DIR) \
-I$(THIRD_PARTY_DIR)/gemmlowp \
-I$(THIRD_PARTY_DIR)/flatbuffers/include \
-I$(THIRD_PARTY_DIR)/kissfft \
-I$(THIRD_PARTY_DIR)/ruy \
-I$(THIRD_PARTY_DIR)/cmsis/CMSIS/Core/Include/ \
-I$(THIRD_PARTY_DIR)/cmsis/CMSIS/DSP/Include/  \
-isystem$(THIRD_PARTY_DIR)/cmsis/CMSIS/Core/Include/ \
-Isystem$(THIRD_PARTY_DIR)/cmsis/CMSIS/DSP/Include/  \
-I$(THIRD_PARTY_DIR)/cmsis/CMSIS/NN/Include/  \


################################################################################
# Objects and binaries														   #
################################################################################
# Default
OBJS = $(CFILES:%.c=$(BUILD_DIR)/%.o)
OBJS += $(CXXFILES:%.cpp=$(BUILD_DIR)/%.o)
OBJS += $(CCFILES:%.cc=$(BUILD_DIR)/%.o)
OBJS += $(AFILES:%.S=$(BUILD_DIR)/%.o)

GENERATED_BINS = firmware.elf firmware.bin firmware.map

# Test
TEST_OBJS = $(TESTFILES:%.cc=$(TEST_BUILD_DIR)/%.o)
 

################################################################################
# Defines																	   #
################################################################################
C_DEFS :=
CXX_DEFS :=  \
-DNDEBUG \
-DTF_LITE_STATIC_MEMORY \
-DGEMMLOWP_ALLOW_SLOW_SCALAR_FALLBACK 


################################################################################
# Compiler Flags															   #
################################################################################
FLAGS :=\
-Wall \
-Wno-strict-aliasing \
-Wno-sign-compare \
-fdata-sections \
-ffunction-sections \
-Wno-narrowing\
-funsigned-char \
-nostdlib \
-fmessage-length=0 \
-fno-unwind-tables \
-fomit-frame-pointer \
-fno-common \
$(ARCH_FLAGS) \
$(OPT) \
$(DEBUG) \
$(CPPFLAGS)  			# These come from libopencm3
FLAGS += -DCMSIS_NN 	# Needed due to ifdef statement in tensorflow code


C_FLAGS := $(FLAGS) $(C_DEFS) -std=c11 
CXX_FLAGS := $(FLAGS) $(CXX_DEFS) -std=c++11 -std=gnu++11 \
	-fno-rtti -fpermissive -fno-threadsafe-statics -fno-use-cxa-atexit
AS_FLAGS := $(FLAGS)

# Test flags
TESTLITE_CXXFLAGS 	= -std=c++11 -DTF_LITE_STATIC_MEMORY -Wno-narrowing
TESTLITE_CXXFLAGS  += -O3 -DTF_LITE_DISABLE_X86_NEON 

################################################################################
# Linker Flags and Lib			    										   #
################################################################################
LIBS := -Wl,--start-group -lc -lgcc -lm -lnosys -Wl,--end-group
LDFLAGS +=  \
$(ARCH_FLAGS) \
-nostartfiles \
-specs=nano.specs \
-specs=nosys.specs \
-L$(OPENCM3_DIR)/lib \
-T$(LDSCRIPT) \
$(LIBS) \
-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
-Wl,--gc-sections \
-Wl,--no-wchar-size-warning \
-funsigned-char \
-ffunction-sections \
-fdata-sections \
-fno-common \
-static  \
$(OPT) \
$(DEBUG) 

ifeq ($(V),99)
LDFLAGS += -Wl,--print-gc-sections
endif

# error if not using linker script generator
ifeq (,$(DEVICE))
$(LDSCRIPT):
ifeq (,$(wildcard $(LDSCRIPT)))
    $(error Unable to find specified linker script: $(LDSCRIPT))
endif
else
# if linker script generator was used, make sure it's cleaned.
GENERATED_BINS += $(LDSCRIPT)
endif

# Linker script generator fills this in for us.
#ifeq (,$(DEVICE))
#LDLIBS += -l$(OPENCM3_LIB)
#endif


################################################################################
# Extra																		   #
################################################################################
# Burn in legacy hell fortran modula pascal yacc idontevenwat
.SUFFIXES:
.SUFFIXES: .c .S .h .o .cxx .cpp .cc .elf .bin

# Bad make, never *ever* try to get a file out of source control by yourself.
%: %,v
%: RCS/%,v
%: RCS/%
%: s.%
%: SCCS/s.%


################################################################################
# Rules																		   #
################################################################################

# DO NOT CALL get_size.sh for now
# All command will also call get_size script, right now we are hardcoding 
# flash and ram values, however this should be auto generated
	#@printf "  SIZE\t$<\n"
	#$(Q)bash ../../get_size.sh $(BUILD_DIR)/firmware.elf 0x200000 0x60000
	#$(Q)$(SIZE) $(BUILD_DIR)/firmware.elf
all: $(BUILD_DIR)/firmware.elf $(BUILD_DIR)/firmware.bin

$(BUILD_DIR)/%.o: %.c
	@printf "  CC\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(C_FLAGS) $(INCLUDES) -o $@ -c $<

$(BUILD_DIR)/%.o: %.cxx
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(CXX_FLAGS) $(INCLUDES) -o $@ -c $<

$(BUILD_DIR)/%.o: %.cpp
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXX_FLAGS) $(INCLUDES) -o $@ -c $<

$(BUILD_DIR)/%.o: %.cc
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXX_FLAGS) $(INCLUDES) -o $@ -c $<

$(BUILD_DIR)/%.o: %.S
	@printf "  AS\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(AS) $(AS_FLAGS) $(INCLUDES) -o $@ -c $<

$(BUILD_DIR)/firmware.elf: $(OBJS) $(LDSCRIPT) $(LIBDEPS)
	@printf "  LD\t$@\n"
	$(Q)$(LD) $(OBJS) $(LDFLAGS) $(INCLUDES) $(LIBDEPS) -o $@

$(BUILD_DIR)/firmware.bin: $(BUILD_DIR)/firmware.elf
	@printf "  OBJCOPY\t$@\n"
	$(Q)$(OBJCOPY) -O binary -S $< $@

# Test rules, clear prefix first, so we can use native gcc
test: PREFIX = 
test: $(TEST_BUILD_DIR)/test_firmware
	@printf "  SIZE\t$<\n"
	$(Q)$(SIZE) $(TEST_BUILD_DIR)/test_firmware
	@./$(TEST_BUILD_DIR)/test_firmware

$(TEST_BUILD_DIR)/test_firmware: $(TEST_OBJS)
	@printf "  LD\t$@\n"
	$(Q)$(LD) $(TESTLITE_CXXFLAGS) $(TEST_OBJS) $(TEST_LDLIBS) -lm -o $@

$(TEST_BUILD_DIR)/%.o: %.cc
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(TESTLITE_CXXFLAGS) $(INCLUDES) -o $@ -c $<


# It is expected that a openocd.cfg file is in project folder
flash: $(BUILD_DIR)/firmware.bin
	@printf "  SIZE\t$<\n"
	$(Q)bash ../../get_size.sh $(BUILD_DIR)/firmware.elf 0x200000 0x60000
	#$(Q)$(SIZE) $(BUILD_DIR)/firmware.elf
	@printf "  OPENOCD\t$<\n"
	$(Q)$(OPENOCD)

monitor: $(BUILD_DIR)/firmware.bin
	@printf "  SIZE\t$<\n"
	$(Q)$(SIZE) $(BUILD_DIR)/firmware.elf
	@printf "  OPENOCD\t$<\n"
	$(Q)$(OPENOCD)
	@printf "  MINICOM\t$<\n"
	$(Q)$(MINICOM)

clean:
	rm -rf $(BUILD_DIR) generated.*

clean_all:
	rm -rf $(BUILD_DIR) generated.* microlite_build

clean_test:
	rm -rf $(TEST_BUILD_DIR)

clean_test_all:
	rm -rf $(TEST_BUILD_DIR) testlite_build

.PHONY: all clean flash monitor
-include $(OBJS:.o=.d)

