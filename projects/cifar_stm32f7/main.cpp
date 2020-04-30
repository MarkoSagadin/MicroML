
// Includes connected with Tensorflow 
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/testing/micro_test.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"

// Includes connected with micro
#include "sys_init.h"
#include "utility.h"
#include "cifar_model.h"
#include "pictures/pictures.h"
#include "model_settings.h"


constexpr int tensor_arena_size = 45 * 1024;
uint8_t tensor_arena[tensor_arena_size];

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
    printf("[[%f %f %f]]\n", output->data.f[0],
                             output->data.f[1],
                             output->data.f[2]);
    printf("Inference time: %d ms", duration);
}

int main() 
{
    clock_setup();
    systick_setup();
    usart_setup();
    gpio_setup();
    //i2c_setup();
    //spi_setup();

    printf("System setup done!\n");

    tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const tflite::Model* model = ::tflite::GetModel(cifar_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("Model provided is schema version %d not equal "
               "to supported version %d.\n",
               model->version(), TFLITE_SCHEMA_VERSION);
    }

    tflite::MicroOpResolver <6> micro_op_resolver;
    micro_op_resolver.AddBuiltin(
            tflite::BuiltinOperator_CONV_2D,
            tflite::ops::micro::Register_CONV_2D(), 
            3
    );

    micro_op_resolver.AddBuiltin(
            tflite::BuiltinOperator_MAX_POOL_2D,
            tflite::ops::micro::Register_MAX_POOL_2D(),
            2
    );
    micro_op_resolver.AddBuiltin(
            tflite::BuiltinOperator_RESHAPE, 
            tflite::ops::micro::Register_RESHAPE()
    );
    micro_op_resolver.AddBuiltin(
            tflite::BuiltinOperator_FULLY_CONNECTED, 
            tflite::ops::micro::Register_FULLY_CONNECTED(), 
            4
    );
    micro_op_resolver.AddBuiltin(
            tflite::BuiltinOperator_SOFTMAX,
            tflite::ops::micro::Register_SOFTMAX(), 
            2
    );
    micro_op_resolver.AddBuiltin(
            tflite::BuiltinOperator_DEQUANTIZE, 
            tflite::ops::micro::Register_DEQUANTIZE(), 
            2
    );

    // Build an interpreter to run the model with.
    tflite::MicroInterpreter interpreter(model, 
                                        micro_op_resolver, 
                                        tensor_arena,
                                        tensor_arena_size, 
                                        error_reporter);
    interpreter.AllocateTensors();

    // Get information about the memory area to use for the model's input.
    TfLiteTensor* input = interpreter.input(0);

    printf("Input:\n");
    printf("Dimension: %d\n", input->dims->size);
    printf("First Dimension: %d\n", input->dims->data[0]);
    printf("Rows: %d\n", input->dims->data[1]);
    printf("Columns: %d\n", input->dims->data[2]);
    printf("Channels: %d\n", input->dims->data[3]);
    printf("Input type: %d\n", input->type);

    load_data(picture0, input);

    uint32_t start = millis();
    // Run the model on this input and make sure it succeeds.
    TfLiteStatus invoke_status = interpreter.Invoke();
    if (invoke_status != kTfLiteOk)
    {
        printf("Invoke failed\n");
    }
    uint32_t end = millis();

    // Get the output from the model, and make sure it's the expected size and
    // type.
    TfLiteTensor* output = interpreter.output(0);

    printf("\nOutput:\n");
    printf("Dimension size: %d\n",  output->dims->size);
    printf("First Dimension: %d\n", output->dims->data[0]);
    printf("Rows: %d\n",            output->dims->data[1]);
    printf("Output type: %d\n",     output->type);

    print_result("Picture 0", output, end-start);

    // Picture 1
    load_data(picture1, input);
    start = millis();
    interpreter.Invoke();
    end = millis();
    output = interpreter.output(0);
    print_result("Picture 1", output, end-start);

    // Picture 2
    load_data(picture2, input);
    start = millis();
    interpreter.Invoke();
    end = millis();
    output = interpreter.output(0);
    print_result("Picture 2", output, end-start);

    // Picture 3
    load_data(picture3, input);
    start = millis();
    interpreter.Invoke();
    end = millis();
    output = interpreter.output(0);
    print_result("Picture 3", output, end-start);

    // Picture 4
    load_data(picture4, input);
    start = millis();
    interpreter.Invoke();
    end = millis();
    output = interpreter.output(0);
    print_result("Picture 4", output, end-start);

    // Picture 5
    
    load_data(picture5, input);
    start = millis();
    interpreter.Invoke();
    end = millis();
    output = interpreter.output(0);
    print_result("Picture 5", output, end-start);


    while(1)
    {
    }
    
    return 0;
}

