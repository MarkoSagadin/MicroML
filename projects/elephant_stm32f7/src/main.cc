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
#include "fastflash.h"
#include "sys_init.h"
#include "utility.h"
#include "model/full_quant_model.h"
#include "images/images.h"
#include "model/model_settings.h"
#include <libopencm3/cm3/dwt.h>

// Globals, used for compatibility with Arduino-style sketches.
namespace {
    tflite::ErrorReporter* error_reporter = nullptr;
    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;

    // An area of memory to use for input, output, and intermediate arrays.
    const int kTensorArenaSize = 200 * 1024;
    static uint8_t tensor_arena[kTensorArenaSize];
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
    printf("\n%s\n", title);
    printf("[[%f %f %f %f]]\n", output->data.f[0],
                                output->data.f[1],
                                output->data.f[2],
                                output->data.f[3]);
    printf("Inference time: %d ms\n", duration);
}


int main() 
{
    clock_setup();
    //systick_setup();
    usart_setup();
    gpio_setup();
    enable_fastflash();

    // Enable DWT counter for timing
    DWT_LAR |= 0xC5ACCE55;           
    dwt_enable_cycle_counter();

    // Not really used, but we need it to pass it to 
    // MicroInterpreter constructor.
    // For debug output we are using our printf implementation,
    // if we want to use ErrorReporter we need to implement it
    tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(full_quant_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) 
    {
        TF_LITE_REPORT_ERROR(error_reporter, 
            "Model provided is schema version %d not equal to supported version %d.",
            model->version(), TFLITE_SCHEMA_VERSION);
    }

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
        return 0;
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

    print_result(error_reporter, "Image 0", output, dwt_cycles_to_ms(end-start));

    load_data(image1, input);
    start = dwt_read_cycle_counter();
    interpreter->Invoke();
    end = dwt_read_cycle_counter();
    output = interpreter->output(0);
    print_result(error_reporter, "Image 1", output, dwt_cycles_to_ms(end-start));

    load_data(image2, input);
    start = dwt_read_cycle_counter();
    interpreter->Invoke();
    end = dwt_read_cycle_counter();
    output = interpreter->output(0);
    print_result(error_reporter, "Image 2", output, dwt_cycles_to_ms(end-start));

    load_data(image3, input);
    start = dwt_read_cycle_counter();
    interpreter->Invoke();
    end = dwt_read_cycle_counter();
    output = interpreter->output(0);
    print_result(error_reporter, "Image 3", output, dwt_cycles_to_ms(end-start));

    load_data(image4, input);
    start = dwt_read_cycle_counter();
    interpreter->Invoke();
    end = dwt_read_cycle_counter();
    output = interpreter->output(0);
    print_result(error_reporter, "Image 4", output, dwt_cycles_to_ms(end-start));

    load_data(image5, input);
    start = dwt_read_cycle_counter();
    interpreter->Invoke();
    end = dwt_read_cycle_counter();
    output = interpreter->output(0);
    print_result(error_reporter, "Image 5", output, dwt_cycles_to_ms(end-start));

    while(1)
    {}    

    return 0;
}

