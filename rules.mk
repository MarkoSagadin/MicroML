######################################
# Make Verbosity
######################################
# Be silent per default, but 'make V=1' will show all compiler calls.
# If you want, V=99 will print out all sorts of things.
V?=0
ifeq ($(V),0)
Q	:= @
NULL	:= 2>/dev/null
endif


###################################### 
# Tool paths
######################################
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

###################################### 
# Includes
######################################
TENSORFLOW_DIR = ../../tensorflow
THIRD_PARTY_DIR = $(TENSORFLOW_DIR)/tensorflow/lite/micro/tools/make/downloads
OPENCM3_INC = $(OPENCM3_DIR)/include
# Inclusion of library header files
INCLUDES += $(patsubst %,-I%, . $(OPENCM3_INC) )
INCLUDES := \
-I. \
-I$(THIRD_PARTY_DIR)/gemmlowp \
-I$(THIRD_PARTY_DIR)/flatbuffers/include \
-I$(THIRD_PARTY_DIR)/kissfft \
-I$(TENSORFLOW_DIR)


######################################
# OBJECTS and BINS
######################################
OBJS = $(CFILES:%.c=$(BUILD_DIR)/%.o)
OBJS += $(CXXFILES:%.cpp=$(BUILD_DIR)/%.o)
OBJS += $(CCFILES:%.cc=$(BUILD_DIR)/%.o)
OBJS += $(AFILES:%.S=$(BUILD_DIR)/%.o)
OBJS += $(MICROLITE_LIB_OBJS)

GENERATED_BINS = firmware.elf firmware.bin firmware.map


######################################
# Defines
######################################
CXX_DEFS :=  \
-DNDEBUG \
-DTF_LITE_STATIC_MEMORY \


######################################
# Compiler Flags
######################################
FLAGS :=\
-std=c++11 \
-fno-rtti \
-fmessage-length=0 \
-fno-exceptions \
-fno-unwind-tables \
-fno-builtin \
-ffunction-sections \
-fdata-sections \
-funsigned-char \
-fshort-wchar \
-MMD \
-std=gnu++11 \
-Wvla \
-Wall \
-Wextra \
-Wno-unused-parameter \
-Wno-missing-field-initializers \
-Wno-write-strings \
-Wno-sign-compare \
-Wno-narrowing \
-fno-delete-null-pointer-checks \
-fomit-frame-pointer \
-fpermissive \
-nostdlib \
-ggdb \

TGT_CFLAGS := $(ARCH_FLAGS) $(OPT) $(DEBUG) $(FLAGS) $(C_DEFS) $(INCLUDES) $(OPENCM3_DEFS)

TGT_CXXFLAGS += $(ARCH_FLAGS) $(OPT) $(DEBUG) $(FLAGS) $(CXX_DEFS) $(INCLUDES) $(OPENCM3_DEFS)
 
TGT_ASFLAGS := $(ARCH_FLAGS) $(OPT) $(DEBUG) $(FLAGS) $(AS_DEFS) $(INCLUDES) $(OPENCM3_DEFS)


######################################
# Linker Flags and Libs
######################################
#LIBS := -lc -lgcc -lm -lnosys
LIBS := -Wl,--start-group -lc -lgcc -lm -lnosys -Wl,--end-group
TGT_LDFLAGS += $(ARCH_FLAGS) \
-nostartfiles \
-specs=nano.specs \
-fno-rtti \
-fmessage-length=0 \
--specs=nosys.specs \
-fno-exceptions \
-fno-unwind-tables \
-fno-builtin \
-ffunction-sections \
-fdata-sections \
-funsigned-char \
-MMD \
-std=gnu++11 \
-Wvla \
-Wall \
-Wextra \
-Wno-unused-parameter \
-Wno-missing-field-initializers \
-Wno-write-strings \
-Wno-sign-compare \
-fno-delete-null-pointer-checks \
-fomit-frame-pointer \
-fpermissive \
-L$(OPENCM3_DIR)/lib \
-T$(LDSCRIPT) $(LIBS) \
-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
-Wl,--gc-sections \
-Wl,--no-wchar-size-warning

ifeq ($(V),99)
TGT_LDFLAGS += -Wl,--print-gc-sections
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
ifeq (,$(DEVICE))
LDLIBS += -l$(OPENCM3_LIB)
endif


######################################
# Extra
######################################
# Burn in legacy hell fortran modula pascal yacc idontevenwat
.SUFFIXES:
.SUFFIXES: .c .S .h .o .cxx .cpp .cc .elf .bin

# Bad make, never *ever* try to get a file out of source control by yourself.
%: %,v
%: RCS/%,v
%: RCS/%
%: s.%
%: SCCS/s.%


######################################
# Rules
######################################

all: $(BUILD_DIR)/firmware.elf $(BUILD_DIR)/firmware.bin
	@printf "  SIZE\t$<\n"
	$(Q)$(SIZE) $(BUILD_DIR)/firmware.elf

#For checking macros, to use it write make print-VARIABLE
print-%  : ; @echo $* = $($*)


$(BUILD_DIR)/%.o: %.c
	@printf "  CC\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.cxx
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.cpp
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.cc
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

$(BUILD_DIR)/%.o: %.S
	@printf "  AS\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(AS) $(TGT_ASFLAGS) $(ASFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

$(BUILD_DIR)/firmware.elf: $(OBJS) $(LDSCRIPT) $(LIBDEPS)
	@printf "  LD\t$@\n"
	$(Q)$(LD) $(OBJS) $(TGT_LDFLAGS) $(INCLUDES) $(LDLIBS) -o $@

$(BUILD_DIR)/firmware.bin: $(BUILD_DIR)/firmware.elf
	@printf "  OBJCOPY\t$@\n"
	$(Q)$(OBJCOPY) -O binary -S $< $@

# It is expected that a openocd.cfg file is in project folder
flash: $(BUILD_DIR)/firmware.bin
	@printf "  SIZE\t$<\n"
	$(Q)$(SIZE) $(BUILD_DIR)/firmware.elf
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

.PHONY: all clean flash
-include $(OBJS:.o=.d)

