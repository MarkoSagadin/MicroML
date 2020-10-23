/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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
#include "tensorflow/lite/micro/testing/micro_test.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/c/common.h"

#include <stdio.h>
#include <time.h>
#include "full_quant_model.h"
#include "images.h"
#include "model_settings.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {
    tflite::ErrorReporter* error_reporter = nullptr;
    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;

    // An area of memory to use for input, output, and intermediate arrays.
    constexpr int kTensorArenaSize = 200 * 1024;
    static uint8_t tensor_arena[kTensorArenaSize];
}


clock_t start;
clock_t end;

void load_data(const signed char * data, TfLiteTensor * input)
{
    for (int i = 0; i < input->bytes; ++i)
    {
        input->data.int8[i] = data[i];
    }
}

void print_result(const char * title, TfLiteTensor * output, clock_t duration)
{
    printf("\n%s\n", title);
    printf("[[%f %f %f %f]]\n", output->data.f[0],
                                output->data.f[1],
                                output->data.f[2],
                                output->data.f[3]);


    printf("Inference time: %f ms", 
            ((double) (end - start)) / CLOCKS_PER_SEC * 1000);
}

TF_LITE_MICRO_TESTS_BEGIN


TF_LITE_MICRO_TEST(TestInvoke) {
    // Set up logging.
    tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(full_quant_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
                "Model provided is schema version %d not equal "
                "to supported version %d.\n",
                model->version(), TFLITE_SCHEMA_VERSION);
        return 0;
    }

    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.
    // An easier approach is to just use the AllOpsResolver, but this will
    // incur some penalty in code space for op implementations that are not
    // needed by this graph.
    //
    //tflite::ops::micro::AllOpsResolver micro_op_resolver;
    
    // Below approach with MicroMutableOpResolver is much better, we pull in only
    // operation implementations that we need, so we save space. 
    // To figure out which ops re needed just define micro_op_resolver and don't
    // call any AddBultin ops. Make sure that number in definition of 
    // micro_op_resolver matches the number of ops. Adding more versions of same
    // op will demand larger number.
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
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return 0;
    }

    // Get information about the memory area to use for the model's input.
    TfLiteTensor* input = interpreter->input(0);

    // Make sure the input has the properties we expect.
    TF_LITE_MICRO_EXPECT_NE(nullptr, input);
    TF_LITE_MICRO_EXPECT_EQ(4, input->dims->size);
    TF_LITE_MICRO_EXPECT_EQ(1, input->dims->data[0]);
    TF_LITE_MICRO_EXPECT_EQ(kNumRows, input->dims->data[1]);
    TF_LITE_MICRO_EXPECT_EQ(kNumCols, input->dims->data[2]);
    TF_LITE_MICRO_EXPECT_EQ(kNumChannels, input->dims->data[3]);
    TF_LITE_MICRO_EXPECT_EQ(kTfLiteInt8, input->type);
    
    printf("Input:\n");
    printf("Dimension: %d\n", input->dims->size);
    printf("First Dimension: %d\n", input->dims->data[0]);
    printf("Rows: %d\n", input->dims->data[1]);
    printf("Columns: %d\n", input->dims->data[2]);
    printf("Channels: %d\n", input->dims->data[3]);
    printf("Input type: %d\n", input->type);

    // Copy an image with a person into the memory area used for the input.
    // It has to be signed char, data is in int8 format
    load_data(image0, input);

    start = clock();
    // Run the model on this input and make sure it succeeds.
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
    }
    TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);
    end = clock();

    // Get the output from the model, and make sure it's the expected size and
    // type.
    TfLiteTensor* output = interpreter->output(0);
    TF_LITE_MICRO_EXPECT_EQ(2, output->dims->size);
    TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[0]);
    TF_LITE_MICRO_EXPECT_EQ(4, output->dims->data[1]);
    TF_LITE_MICRO_EXPECT_EQ(kTfLiteFloat32, output->type);

    printf("\nOutput:\n");
    printf("Dimension size: %d\n",  output->dims->size);
    printf("First Dimension: %d\n", output->dims->data[0]);
    printf("Rows: %d\n",            output->dims->data[1]);
    printf("Output type: %d\n",     output->type);

    print_result("Picture 0", output, end-start);

    load_data(image1, input);
    start = clock();
    interpreter->Invoke();
    end = clock();
    output = interpreter->output(0);
    print_result("Image 1", output, end-start);

    load_data(image2, input);
    start = clock();
    interpreter->Invoke();
    end = clock();
    output = interpreter->output(0);
    print_result("Image 2", output, end-start);

    load_data(image3, input);
    start = clock();
    interpreter->Invoke();
    end = clock();
    output = interpreter->output(0);
    print_result("Image 3", output, end-start);

    load_data(image4, input);
    start = clock();
    interpreter->Invoke();
    end = clock();
    output = interpreter->output(0);
    print_result("Image 4", output, end-start);

    load_data(image5, input);
    start = clock();
    interpreter->Invoke();
    end = clock();
    output = interpreter->output(0);
    print_result("Image 5", output, end-start);
}

TF_LITE_MICRO_TESTS_END
