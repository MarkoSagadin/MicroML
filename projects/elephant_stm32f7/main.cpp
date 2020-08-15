// Includes connected with Tensorflow 
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/testing/micro_test.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/c/common.h"

// Includes connected with micro
#include "sys_init.h"
#include "utility.h"
#include "full_quant_model.h"
#include "images/images.h"
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


void load_data(const signed char * data, TfLiteTensor * input)
{
    for (int i = 0; i < input->bytes; ++i)
    {
        input->data.int8[i] = data[i];
    }
}

void print_result(const char * title, TfLiteTensor * output, uint32_t duration)
{
    printf("\n%s\n", title);
    printf("[[%f %f %f %f]]\n", output->data.f[0],
                                output->data.f[1],
                                output->data.f[2],
                                output->data.f[3]);
                                

    printf("Inference time: %d ms", duration);
}

int main() 
{
    clock_setup();
    systick_setup();
    usart_setup();
    gpio_setup();

    // Not really used, but we need it to pass it to 
    // MicroInterpreter constructor.
    // For debug output we are using our printf implementation,
    // if we want to use ErrorReporter we need to implement it
    tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(full_quant_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("Model provided is schema version %d not equal "
               "to supported version %d.\n",
               model->version(), TFLITE_SCHEMA_VERSION);
    }

    static tflite::MicroMutableOpResolver<6> micro_op_resolver;
    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddMaxPool2D();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddFullyConnected();
    micro_op_resolver.AddSoftmax();
    micro_op_resolver.AddDequantize();

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
        printf("AllocateTensors() failed");
        return 0;
    }

    // Get information about the memory area to use for the model's input.
    TfLiteTensor* input = interpreter->input(0);

    printf("Input:\n");
    printf("Dimension: %d\n", input->dims->size);
    printf("First Dimension: %d\n", input->dims->data[0]);
    printf("Rows: %d\n", input->dims->data[1]);
    printf("Columns: %d\n", input->dims->data[2]);
    printf("Channels: %d\n", input->dims->data[3]);
    printf("Input type: %d\n", input->type);

    load_data(image0, input);

    uint32_t start = millis();
    // Run the model on this input and make sure it succeeds.
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        printf("Invoke failed\n");
    }
    //uint32_t end = millis();

    // Get the output from the model, and make sure it's the expected size and
    // type.
    TfLiteTensor* output = interpreter->output(0);

    printf("\nOutput:\n");
    printf("Dimension size: %d\n",  output->dims->size);
    printf("First Dimension: %d\n", output->dims->data[0]);
    printf("Rows: %d\n",            output->dims->data[1]);
    printf("Output type: %d\n",     output->type);

    //print_result("Picture 0", output, end-start);


    load_data(image1, input);
    start = millis();
    interpreter->Invoke();
    //end = millis();
    uint32_t dur0 = millis() - start;
    //output = interpreter->output(0);
    //print_result("Image 1", output, end-start);

    load_data(image2, input);
    start = millis();
    interpreter->Invoke();
    //end = millis();
    uint32_t dur1 = millis() - start;
    //output = interpreter->output(0);
    //print_result("Image 2", output, end-start);

    load_data(image3, input);
    start = millis();
    interpreter->Invoke();
    //end = millis();
    uint32_t dur2 = millis() - start;
    //output = interpreter->output(0);
    //print_result("Image 3", output, end-start);

    load_data(image4, input);
    start = millis();
    interpreter->Invoke();
    //end = millis();
    uint32_t dur3 = millis() - start;
    //output = interpreter->output(0);
    //print_result("Image 4", output, end-start);

    load_data(image5, input);
    start = millis();
    interpreter->Invoke();
    //end = millis();
    uint32_t dur4 = millis() - start;
    //output = interpreter->output(0);
    //print_result("Image 5", output, end-start);


    printf("Inference time: %d ms\n", dur0);
    printf("Inference time: %d ms\n", dur1);
    printf("Inference time: %d ms\n", dur2);
    printf("Inference time: %d ms\n", dur3);
    printf("Inference time: %d ms\n", dur4);
    while(1)
    {
    }
    
    return 0;
}

