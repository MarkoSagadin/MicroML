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
#include "model/full_quant_model.h"
#include "model/model_settings.h"
#include "test_images/images.h"
#include "system_setup/utility.h"
#include "system_setup/printf.h"

#include "inference.h"

namespace {
    tflite::ErrorReporter* error_reporter = nullptr;
    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;
    TfLiteTensor* output = nullptr;

    // An area of memory to use for input, output, and intermediate arrays.
    const int kTensorArenaSize = 240000;
    alignas(16) uint8_t tensor_arena[kTensorArenaSize];
    uint32_t duration = 0;
}


static void load_test_data(const signed char * data, TfLiteTensor * input);
static void print_result(tflite::ErrorReporter* error_reporter,
                         const char * title,
                         TfLiteTensor * output, 
                         uint32_t duration);

bool inference_setup()
{
    tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

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
        return false;
    }
    TF_LITE_REPORT_ERROR(error_reporter, "Size of the used memory in bytes: %d\n", 
                                         interpreter->arena_used_bytes());


    input = interpreter->input(0);

    TF_LITE_REPORT_ERROR(error_reporter, "Input:\n");
    TF_LITE_REPORT_ERROR(error_reporter, "Dimension:        %d", input->dims->size);
    TF_LITE_REPORT_ERROR(error_reporter, "First Dimension:  %d", input->dims->data[0]);
    TF_LITE_REPORT_ERROR(error_reporter, "Rows:             %d", input->dims->data[1]);
    TF_LITE_REPORT_ERROR(error_reporter, "Columns:          %d", input->dims->data[2]);
    TF_LITE_REPORT_ERROR(error_reporter, "Channels:         %d", input->dims->data[3]);
    TF_LITE_REPORT_ERROR(error_reporter, "Input type:       %d", input->type);

    output = interpreter->output(0);
    
    TF_LITE_REPORT_ERROR(error_reporter, "\nOutput:");
    TF_LITE_REPORT_ERROR(error_reporter, "Dimension size:   %d", output->dims->size);
    TF_LITE_REPORT_ERROR(error_reporter, "First Dimension:  %d", output->dims->data[0]);
    TF_LITE_REPORT_ERROR(error_reporter, "Rows:             %d", output->dims->data[1]);
    TF_LITE_REPORT_ERROR(error_reporter, "Output type:      %d", output->type);

    return true;
}

bool run_inference()
{
    input = interpreter->input(0);
    printf("\nExecuting ML\n");
    load_test_data(image4, input);
    //uint32_t start = millis();
    interpreter->Invoke();
    //uint32_t end = millis();
    //output = interpreter->output(0);
    //uint32_t duration = end - start;
    return true;
}

void get_inference_results(char * buf, uint16_t max_len)
{
    //snprintf(buf, max_len, "ML: GOOD\n");
    //snprintf(buf, max_len, "ML: [[%f %f %f %f]], %ld\n", output->data.f[0],
    //                                                     output->data.f[1],
    //                                                     output->data.f[2],
    //                                                     output->data.f[3],
    //                                                     duration);
}

static void load_test_data(const signed char * data, TfLiteTensor * input)
{
    for (int i = 0; i < input->bytes; ++i)
    {
        input->data.int8[i] = data[i];
    }
}

static void print_result(tflite::ErrorReporter* error_reporter, 
                         const char * title, 
                         TfLiteTensor * output, 
                         uint32_t duration)
{
    printf("\n%s\n", title);
    printf("[[%f %f %f %f]]\n", output->data.f[0],
                                output->data.f[1],
                                output->data.f[2],
                                output->data.f[3]);
    printf("Inference time: %ld ms\n", duration);
}
