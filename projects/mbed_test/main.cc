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

#include "printf.h"
#include "sys_init.h"
#include "utility.h"
#include "full_quant_model.h"
#include "images/images.h"
#include "model_settings.h"
#include <libopencm3/cm3/dwt.h>
#include "fastfast.h"

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
const int kTensorArenaSize = 200 * 1024;
static uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
int main() {
  SCB_EnableICache();
  SCB_EnableDCache();
  __HAL_FLASH_ART_ENABLE();
  __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  clock_setup();
  //systick_setup();
  usart_setup();
  gpio_setup();

  DWT_LAR |= 0xC5ACCE55;           
  dwt_enable_cycle_counter();
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;
  TF_LITE_REPORT_ERROR(error_reporter, "We are alive and well");
  TF_LITE_REPORT_ERROR(error_reporter, "We are alive and well");
  TF_LITE_REPORT_ERROR(error_reporter, "We are alive and well");
  TF_LITE_REPORT_ERROR(error_reporter, "We are alive and well");
  TF_LITE_REPORT_ERROR(error_reporter, "We are alive and well");

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(full_quant_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return 1;
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
        return 1;
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
