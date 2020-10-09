/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/


#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "images/images.h"
#include "main_functions.h"
#include "constants.h"
#include "full_quant_model.h"
#include "output_handler.h"

#define PPBI_BASE                       (0xE0000000U)
#define SCS_BASE                        (PPBI_BASE + 0xE000)
#define MMIO32(addr)		(*(volatile uint32_t *)(addr))
#define SCS_DEMCR		MMIO32(SCS_BASE + 0xDFC)
#define SCS_DEMCR_TRCENA	(1 << 24)

#define DWT_CTRL			MMIO32(DWT_BASE + 0x00)
#define DWT_BASE                        (PPBI_BASE + 0x1000)
#define DWT_CTRL_NOCYCCNT		(1 << 25)

#define DWT_CYCCNT			MMIO32(DWT_BASE + 0x04)
#define DWT_BASE                        (PPBI_BASE + 0x1000)

#define DWT_CTRL			MMIO32(DWT_BASE + 0x00)
#define DWT_CTRL_CYCCNTENA		(1 << 0)

#define DWT_LAR			MMIO32(DWT_BASE + CORESIGHT_LAR_OFFSET)
#define CORESIGHT_LAR_OFFSET	0xfb0

bool dwt_enable_cycle_counter(void)
{
	/* Note TRCENA is for 7M and above*/
	SCS_DEMCR |= SCS_DEMCR_TRCENA;
	if (DWT_CTRL & DWT_CTRL_NOCYCCNT) {
		return false;		/* Not supported in implementation */
	}

	DWT_CYCCNT = 0;
	DWT_CTRL |= DWT_CTRL_CYCCNTENA;
	return true;
}

uint32_t dwt_read_cycle_counter(void)
{
	if (DWT_CTRL & DWT_CTRL_CYCCNTENA) {
		return DWT_CYCCNT;
	} else {
		return 0;		/* not supported or enabled */
	}
}

void load_data(const signed char * data, TfLiteTensor * input)
{
    for (int i = 0; i < input->bytes; ++i)
    {
        input->data.int8[i] = data[i];
    }
}

void print_result(tflite::ErrorReporter* error_reporter, 
                  const char * title, 
                  TfLiteTensor * output, 
                  uint32_t duration)
{
    TF_LITE_REPORT_ERROR(error_reporter, "\n%s", title);
    TF_LITE_REPORT_ERROR(error_reporter, "[[%f %f %f %f]]", output->data.f[0],
                                                              output->data.f[1],
                                                              output->data.f[2],
                                                              output->data.f[3]);
                                

    TF_LITE_REPORT_ERROR(error_reporter, "Inference time:           %d ms", duration);
}

uint32_t dwt_cycles_to_ms(uint32_t dwt_cycles)
{
    // 48 represents the clock frequency in MHz
    return dwt_cycles * (1.0f / (218 * 1000.0f)) ;
}

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;

// Create an area of memory to use for input, output, and intermediate arrays.
// Minimum arena size, at the time of writing. After allocating tensors
// you can retrieve this value by invoking interpreter.arena_used_bytes().
//
//const int kModelArenaSize = 2468;
//// Extra headroom for model + alignment + future interpreter changes.
//const int kExtraArenaSize = 560 + 16 + 100;
//const int kTensorArenaSize = kModelArenaSize + kExtraArenaSize;
//uint8_t tensor_arena[kTensorArenaSize];

const int kTensorArenaSize = 200 * 1024;
static uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

volatile uint32_t mscount = 0;
void onMillisecondTicker(void)
{
    mscount++;
}

// The name of this function is important for Arduino compatibility.
void setup() {

  DWT_LAR |= 0xC5ACCE55;           
  dwt_enable_cycle_counter();
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(full_quant_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
  TF_LITE_REPORT_ERROR(error_reporter, "We are alive and well");
  TF_LITE_REPORT_ERROR(error_reporter, "We are alive and well");
  TF_LITE_REPORT_ERROR(error_reporter, "We are alive and well");

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  //static tflite::AllOpsResolver resolver;

    static tflite::MicroMutableOpResolver<8> micro_op_resolver;
    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddMaxPool2D();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddFullyConnected();
    micro_op_resolver.AddSoftmax();
    micro_op_resolver.AddDequantize();
    micro_op_resolver.AddMul();
    micro_op_resolver.AddAdd();
    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(model, 
                                                       micro_op_resolver, 
                                                       tensor_arena,
                                                       kTensorArenaSize, 
                                                       error_reporter);

    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) 
    {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return ;
    }

    // Get information about the memory area to use for the model's input.
    TfLiteTensor* input = interpreter->input(0);

    TF_LITE_REPORT_ERROR(error_reporter, "Input:\n");
    TF_LITE_REPORT_ERROR(error_reporter, "Dimension: %d",         input->dims->size);
    TF_LITE_REPORT_ERROR(error_reporter, "First Dimension: %d",   input->dims->data[0]);
    TF_LITE_REPORT_ERROR(error_reporter, "Rows: %d",              input->dims->data[1]);
    TF_LITE_REPORT_ERROR(error_reporter, "Columns: %d",           input->dims->data[2]);
    TF_LITE_REPORT_ERROR(error_reporter, "Channels: %d",          input->dims->data[3]);
    TF_LITE_REPORT_ERROR(error_reporter, "Input type: %d",        input->type);

    load_data(image0, input);
    uint32_t start = dwt_read_cycle_counter();
    // Run the model on this input and make sure it succeeds.
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
    }
    uint32_t end = dwt_read_cycle_counter();

    // Get the output from the model, and make sure it's the expected size and
    // type.
    TfLiteTensor* output = interpreter->output(0);

    TF_LITE_REPORT_ERROR(error_reporter, "\nOutput:");
    TF_LITE_REPORT_ERROR(error_reporter, "Dimension size: %d",  output->dims->size);
    TF_LITE_REPORT_ERROR(error_reporter, "First Dimension: %d", output->dims->data[0]);
    TF_LITE_REPORT_ERROR(error_reporter, "Rows: %d",            output->dims->data[1]);
    TF_LITE_REPORT_ERROR(error_reporter, "Output type: %d",     output->type);

    print_result(error_reporter, "Picture 0", output, dwt_cycles_to_ms(end-start));

    while(1)
    {
        load_data(image1, input);
        start = dwt_read_cycle_counter();
        interpreter->Invoke();
        end = dwt_read_cycle_counter();
        output = interpreter->output(0);

        print_result(error_reporter, "Image 1", output, dwt_cycles_to_ms(end-start));
    }
}
