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

#For checking macros, to use it write make print-VARIABLE
print-%  : ; @echo $* = $($*)

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O3


#######################################
# paths
#######################################
# Build path
BUILD_DIR = mx_build

######################################
# source
######################################
# C and CC sources
SOURCES :=  \
cubemx_test/main.cc \
cubemx_test/printf.c \
cubemx_test/debug_log.cc \
cubemx_test/full_quant_model.cc \
cubemx_test/model_settings.cc \
cubemx_test/images/images.cc \
cubemx_test/stm32f7xx_it.c \
cubemx_test/stm32f7xx_hal_msp.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_exti.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart_ex.c \
cubemx_test/system_stm32f7xx.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c \
cubemx_test/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c 

TENSORFLOW_SOURCES_PATH := tensorflow/tensorflow/lite
SOURCES += \
$(TENSORFLOW_SOURCES_PATH)/micro/all_ops_resolver.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/memory_helpers.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/micro_allocator.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/micro_error_reporter.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/micro_interpreter.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/micro_optional_debug_tools.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/micro_profiler.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/micro_string.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/micro_utils.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/recording_micro_allocator.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/recording_simple_memory_allocator.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/simple_memory_allocator.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/test_helpers.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/benchmarks/keyword_scrambled_model_data.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/activations.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/arg_min_max.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/ceil.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/circular_buffer.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/comparisons.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/concatenation.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/dequantize.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/elementwise.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/ethosu.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/floor.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/l2norm.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/logical.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/logistic.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/maximum_minimum.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/neg.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/pack.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/pad.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/prelu.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/quantize.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/reduce.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/reshape.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/resize_nearest_neighbor.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/round.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/split.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/strided_slice.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/sub.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/svdf.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/tanh.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/unpack.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/memory_planner/greedy_memory_planner.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/memory_planner/linear_memory_planner.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/testing/test_conv_model.cc \
$(TENSORFLOW_SOURCES_PATH)/c/common.c \
$(TENSORFLOW_SOURCES_PATH)/core/api/error_reporter.cc \
$(TENSORFLOW_SOURCES_PATH)/core/api/flatbuffer_conversions.cc \
$(TENSORFLOW_SOURCES_PATH)/core/api/op_resolver.cc \
$(TENSORFLOW_SOURCES_PATH)/core/api/tensor_utils.cc \
$(TENSORFLOW_SOURCES_PATH)/kernels/internal/quantization_util.cc \
$(TENSORFLOW_SOURCES_PATH)/kernels/kernel_util.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/testing/test_utils.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/tools/make/downloads/cmsis/CMSIS/DSP/Source/BasicMathFunctions/arm_mult_q15.c \
$(TENSORFLOW_SOURCES_PATH)/micro/tools/make/downloads/cmsis/CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_q15.c \
$(TENSORFLOW_SOURCES_PATH)/micro/tools/make/downloads/cmsis/CMSIS/DSP/Source/TransformFunctions/arm_rfft_q15.c \
$(TENSORFLOW_SOURCES_PATH)/micro/tools/make/downloads/cmsis/CMSIS/DSP/Source/TransformFunctions/arm_cfft_q15.c \
$(TENSORFLOW_SOURCES_PATH)/micro/tools/make/downloads/cmsis/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix4_q15.c \
$(TENSORFLOW_SOURCES_PATH)/micro/tools/make/downloads/cmsis/CMSIS/DSP/Source/CommonTables/arm_const_structs.c \
$(TENSORFLOW_SOURCES_PATH)/micro/tools/make/downloads/cmsis/CMSIS/DSP/Source/CommonTables/arm_common_tables.c \
$(TENSORFLOW_SOURCES_PATH)/micro/tools/make/downloads/cmsis/CMSIS/DSP/Source/StatisticsFunctions/arm_mean_q15.c \
$(TENSORFLOW_SOURCES_PATH)/micro/tools/make/downloads/cmsis/CMSIS/DSP/Source/StatisticsFunctions/arm_max_q7.c \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/cmsis-nn/add.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/cmsis-nn/conv.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/cmsis-nn/depthwise_conv.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/cmsis-nn/fully_connected.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/cmsis-nn/mul.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/cmsis-nn/pooling.cc \
$(TENSORFLOW_SOURCES_PATH)/micro/kernels/cmsis-nn/softmax.cc \



#$(TENSORFLOW_SOURCES_PATH)/micro/kernels/conv.cc \
#$(TENSORFLOW_SOURCES_PATH)/micro/kernels/add.cc \
#$(TENSORFLOW_SOURCES_PATH)/micro/kernels/portable_optimized/depthwise_conv.cc \
#$(TENSORFLOW_SOURCES_PATH)/micro/kernels/fully_connected.cc \
#$(TENSORFLOW_SOURCES_PATH)/micro/kernels/mul.cc \
#$(TENSORFLOW_SOURCES_PATH)/micro/kernels/pooling.cc \
#$(TENSORFLOW_SOURCES_PATH)/micro/kernels/softmax.cc \

CMSIS_NN_PATH := tensorflow/tensorflow/lite/micro/tools/make/downloads/cmsis//CMSIS/NN/Source
SOURCES += \
${CMSIS_NN_PATH}/BasicMathFunctions/arm_elementwise_mul_s8.c \
${CMSIS_NN_PATH}/BasicMathFunctions/arm_elementwise_add_s8.c \
${CMSIS_NN_PATH}/FullyConnectedFunctions/arm_fully_connected_q7_opt.c \
${CMSIS_NN_PATH}/FullyConnectedFunctions/arm_fully_connected_s8.c \
${CMSIS_NN_PATH}/FullyConnectedFunctions/arm_fully_connected_q15_opt.c \
${CMSIS_NN_PATH}/FullyConnectedFunctions/arm_fully_connected_q15.c \
${CMSIS_NN_PATH}/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15.c \
${CMSIS_NN_PATH}/FullyConnectedFunctions/arm_fully_connected_q7.c \
${CMSIS_NN_PATH}/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15_opt.c \
${CMSIS_NN_PATH}/ConcatenationFunctions/arm_concatenation_s8_z.c \
${CMSIS_NN_PATH}/ConcatenationFunctions/arm_concatenation_s8_x.c \
${CMSIS_NN_PATH}/ConcatenationFunctions/arm_concatenation_s8_w.c \
${CMSIS_NN_PATH}/ConcatenationFunctions/arm_concatenation_s8_y.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_nn_depthwise_conv_s8_core.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_depthwise_conv_u8_basic_ver1.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_s8.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_1x1_s8_fast.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_HWC_q7_fast.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15_reordered.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_HWC_q7_RGB.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_nn_mat_mult_s8.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_1x1_HWC_q7_fast_nonsquare.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_HWC_q7_basic.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7_nonsquare.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_HWC_q15_fast_nonsquare.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_depthwise_conv_s8_opt.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_depthwise_conv_3x3_s8.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_nn_mat_mult_kernel_s8_s16.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_HWC_q15_basic.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_HWC_q15_fast.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_HWC_q7_fast_nonsquare.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_nn_mat_mult_kernel_s8_s16_reordered.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_depthwise_conv_s8.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_1_x_n_s8.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_HWC_q7_basic_nonsquare.c \
${CMSIS_NN_PATH}/ConvolutionFunctions/arm_convolve_wrapper_s8.c \
${CMSIS_NN_PATH}/PoolingFunctions/arm_pool_q7_HWC.c \
${CMSIS_NN_PATH}/PoolingFunctions/arm_max_pool_s8_opt.c \
${CMSIS_NN_PATH}/PoolingFunctions/arm_avgpool_s8.c \
${CMSIS_NN_PATH}/PoolingFunctions/arm_max_pool_s8.c \
${CMSIS_NN_PATH}/ActivationFunctions/arm_relu_q15.c \
${CMSIS_NN_PATH}/ActivationFunctions/arm_relu_q7.c \
${CMSIS_NN_PATH}/ActivationFunctions/arm_nn_activations_q7.c \
${CMSIS_NN_PATH}/ActivationFunctions/arm_relu6_s8.c \
${CMSIS_NN_PATH}/ActivationFunctions/arm_nn_activations_q15.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_add_q7.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nntables.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_mult_q7.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_mult_q15.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_q7_to_q15_with_offset.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.c \
${CMSIS_NN_PATH}/NNSupportFunctions/arm_q7_to_q15_no_shift.c \
${CMSIS_NN_PATH}/SoftmaxFunctions/arm_softmax_s8.c \
${CMSIS_NN_PATH}/SoftmaxFunctions/arm_softmax_q15.c \
${CMSIS_NN_PATH}/SoftmaxFunctions/arm_softmax_u8.c \
${CMSIS_NN_PATH}/SoftmaxFunctions/arm_softmax_q7.c \
${CMSIS_NN_PATH}/SoftmaxFunctions/arm_softmax_with_batch_q7.c \
${CMSIS_NN_PATH}/ReshapeFunctions/arm_reshape_s8.c \

# ASM sources
SOURCES +=  \
cubemx_test/startup_stm32f767xx.s

#######################################
# tensorflow section
#######################################

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
LD = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m7

# fpu
FPU = -mfpu=fpv5-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F767xx

CXX_DEFS =  \
-DNDEBUG \
-DTF_LITE_STATIC_MEMORY \
-DGEMMLOWP_ALLOW_SLOW_SCALAR_FALLBACK 

# C includes
THIRD_PARTY_DIR = tensorflow/tensorflow/lite/micro/tools/make/downloads
INCLUDES =  \
-Icubemx_test \
-Icubemx_test/STM32F7xx_HAL_Driver/Inc \
-Icubemx_test/STM32F7xx_HAL_Driver/Inc/Legacy \
-Icubemx_test/CMSIS/Device/ST/STM32F7xx/Include \
-Icubemx_test/CMSIS/Include \
-Itensorflow \
-I$(THIRD_PARTY_DIR) \
-I$(THIRD_PARTY_DIR)/gemmlowp \
-I$(THIRD_PARTY_DIR)/flatbuffers/include \
-I$(THIRD_PARTY_DIR)/kissfft \
-I$(THIRD_PARTY_DIR)/ruy \
-I$(THIRD_PARTY_DIR)/cmsis/CMSIS/Core/Include/ \
-I$(THIRD_PARTY_DIR)/cmsis/CMSIS/DSP/Include/  \

#-Itensorflow/third_party/eigen3 \
#-Itensorflow/third_party \

# compile gcc flags
ASFLAGS = $(MCU) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(OPT) -Wall -fdata-sections -ffunction-sections -Wno-narrowing

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

CXXFLAGS = $(CFLAGS) $(CXX_DEFS) -std=c++11 -std=gnu++11 \
-fno-rtti -fpermissive -fno-threadsafe-statics -fno-use-cxa-atexit

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = cubemx_test/STM32F767ZITx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -specs=nosys.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/memory.map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/firmware.elf $(BUILD_DIR)/firmware.hex $(BUILD_DIR)/firmware.bin


#######################################
# List of objects
#######################################
OBJECTS := $(addprefix $(BUILD_DIR)/,$(patsubst %.cc,%.o,$(filter %.cc , $(SOURCES))))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(filter %.c , $(SOURCES))))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(patsubst %.s,%.o,$(filter %.s , $(SOURCES))))

vpath %.c $(sort $(dir $(SOURCES)))
vpath %.cc $(sort $(dir $(SOURCES)))
vpath %.s $(sort $(dir $(SOURCES)))

######################################
# Rules 
######################################

$(BUILD_DIR)/%.o: %.cc Makefile | $(BUILD_DIR)
	@printf "  CXX\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCLUDES) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cc=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@printf "  CC\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(CC) -c $(CFLAGS) $(INCLUDES) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@printf "  AS\t$<\n"
	@mkdir -p $(dir $@)
	$(Q)$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/firmware.elf: $(OBJECTS) Makefile | $(BUILD_DIR)
	$(LD) $(OBJECTS) $(LDFLAGS) $(INCLUDES) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

# It is expected that a openocd.cfg file is in project folder
flash: $(BUILD_DIR)/firmware.elf
	@printf "  OPENOCD\t$<\n"
	openocd

monitor: $(BUILD_DIR)/firmware.elf
	@printf "  OPENOCD\t$<\n"
	openocd
	@printf "  MINICOM\t$<\n"
	minicom

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
#-include $(wildcard $(BUILD_DIR)/*.d)
-include $(OBJECTS:.o=.d)
.PHONY: all clean flash monitor

# *** EOF ***
