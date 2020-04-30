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

// #include "tensorflow/lite/c/common.h"
#include "cifar_model.h"
#include "pictures.h"
#include "model_settings.h"
#include "tensorflow/lite/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/testing/micro_test.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"

#include "stdio.h"

constexpr int tensor_arena_size = 50 * 1024;
uint8_t tensor_arena[tensor_arena_size];

void load_data(const signed char * data, TfLiteTensor * input)
{
    for (int i = 0; i < input->bytes; ++i)
    {
        input->data.int8[i] = data[i];
    }
}

void print_result(const char * title, TfLiteTensor * output)
{
    printf("\n%s\n", title);
    printf("[[%f %f %f]]\n", output->data.f[0],
                             output->data.f[1],
                             output->data.f[2]);
}

TF_LITE_MICRO_TESTS_BEGIN


TF_LITE_MICRO_TEST(TestInvoke) {
    // Set up logging.
    tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const tflite::Model* model = ::tflite::GetModel(cifar_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
                "Model provided is schema version %d not equal "
                "to supported version %d.\n",
                model->version(), TFLITE_SCHEMA_VERSION);
    }

    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.
    // An easier approach is to just use the AllOpsResolver, but this will
    // incur some penalty in code space for op implementations that are not
    // needed by this graph.
    //
    tflite::ops::micro::AllOpsResolver micro_op_resolver;
    
    // Somehow below micro_op_resolver does not work, there is some error
    // with CONV2D version
    //tflite::MicroOpResolver<5> micro_op_resolver;
    //micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
    //                             tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
    //micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
    //                             tflite::ops::micro::Register_CONV_2D());
    //micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_AVERAGE_POOL_2D,
    //                             tflite::ops::micro::Register_AVERAGE_POOL_2D());
    //micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_QUANTIZE,
    //                             tflite::ops::micro::Register_QUANTIZE());
    //micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_DEQUANTIZE,
    //                             tflite::ops::micro::Register_DEQUANTIZE());


    // Build an interpreter to run the model with.
    tflite::MicroInterpreter interpreter(model, 
                                        micro_op_resolver, 
                                        tensor_arena,
                                        tensor_arena_size, 
                                        error_reporter);
    interpreter.AllocateTensors();

    // Get information about the memory area to use for the model's input.
    TfLiteTensor* input = interpreter.input(0);

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
    // But we still have to feed it into network as float, this is TODO
    

    load_data(picture0, input);

    // Run the model on this input and make sure it succeeds.
    TfLiteStatus invoke_status = interpreter.Invoke();
    if (invoke_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
    }
    TF_LITE_MICRO_EXPECT_EQ(kTfLiteOk, invoke_status);

    // Get the output from the model, and make sure it's the expected size and
    // type.
    TfLiteTensor* output = interpreter.output(0);
    TF_LITE_MICRO_EXPECT_EQ(2, output->dims->size);
    TF_LITE_MICRO_EXPECT_EQ(1, output->dims->data[0]);
    TF_LITE_MICRO_EXPECT_EQ(3, output->dims->data[1]);
    TF_LITE_MICRO_EXPECT_EQ(kTfLiteFloat32, output->type);

    printf("\nOutput:\n");
    printf("Dimension size: %d\n",  output->dims->size);
    printf("First Dimension: %d\n", output->dims->data[0]);
    printf("Rows: %d\n",            output->dims->data[1]);
    printf("Output type: %d\n",     output->type);

    print_result("Picture 0", output);

    // Picture 1
    load_data(picture1, input);
    interpreter.Invoke();
    output = interpreter.output(0);
    print_result("Picture 1", output);

    // Picture 2
    load_data(picture2, input);
    interpreter.Invoke();
    output = interpreter.output(0);
    print_result("Picture 2", output);

    // Picture 3
    load_data(picture3, input);
    interpreter.Invoke();
    output = interpreter.output(0);
    print_result("Picture 3", output);

    // Picture 4
    load_data(picture4, input);
    interpreter.Invoke();
    output = interpreter.output(0);
    print_result("Picture 4", output);

    // Picture 5
    load_data(picture5, input);
    interpreter.Invoke();
    output = interpreter.output(0);
    print_result("Picture 5", output);
}

TF_LITE_MICRO_TESTS_END
