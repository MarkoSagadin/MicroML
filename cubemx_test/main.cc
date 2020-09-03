// Includes connected with Tensorflow 
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/testing/micro_test.h>
#include <tensorflow/lite/micro/kernels/micro_ops.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>
#include <tensorflow/lite/c/common.h>

#include "main.h"
#include "printf.h"
#include "CMSIS/Include/core_cm7.h"
#include "full_quant_model.h"
#include "images/images.h"
#include "model_settings.h"

UART_HandleTypeDef huart3;

void SystemClock_Config(void);
uint32_t dwt_cycles_to_ms(uint32_t dwt_cycles);
static void MX_USART3_UART_Init(void);

void _putchar(char character)
{
    uint8_t c = (uint8_t)character;
    HAL_UART_Transmit(&huart3, &c, 1, 0xffffff);
}


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
    for (uint32_t i = 0; i < input->bytes; ++i)
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
                                

    TF_LITE_REPORT_ERROR(error_reporter, "Inference time: %d ms", duration);
}

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_USART3_UART_Init();

    // Start DWT counter
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->LAR = 0xC5ACCE55; 
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


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

    uint32_t start = DWT->CYCCNT;
    // Run the model on this input and make sure it succeeds.
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
    }
    uint32_t end = DWT->CYCCNT;

    // Get the output from the model, and make sure it's the expected size and
    // type.
    TfLiteTensor* output = interpreter->output(0);

    TF_LITE_REPORT_ERROR(error_reporter, "\nOutput:");
    TF_LITE_REPORT_ERROR(error_reporter, "Dimension size: %d",  output->dims->size);
    TF_LITE_REPORT_ERROR(error_reporter, "First Dimension: %d", output->dims->data[0]);
    TF_LITE_REPORT_ERROR(error_reporter, "Rows: %d",            output->dims->data[1]);
    TF_LITE_REPORT_ERROR(error_reporter, "Output type: %d",     output->type);

    print_result(error_reporter, "Picture 0", output, dwt_cycles_to_ms(end-start));
    print_result(error_reporter, "Picture 0", output, dwt_cycles_to_ms(end-start));
    print_result(error_reporter, "Picture 0", output, dwt_cycles_to_ms(end-start));
    print_result(error_reporter, "Picture 0", output, dwt_cycles_to_ms(end-start));
    print_result(error_reporter, "Picture 0", output, dwt_cycles_to_ms(end-start));
    print_result(error_reporter, "Picture 0", output, dwt_cycles_to_ms(end-start));

    while(1)
    {
        load_data(image1, input);
        start = DWT->CYCCNT;
        interpreter->Invoke();
        end = DWT->CYCCNT;
        output = interpreter->output(0);
        print_result(error_reporter, "Image 1", output, dwt_cycles_to_ms(end-start));
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure LSE Drive Capability
    */
    HAL_PWR_EnableBkUpAccess();
    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 216;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Activate the Over-Drive mode
    */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
        |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{
    /* USER CODE END USART3_Init 1 */
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart3) != HAL_OK)
    {
        Error_Handler();
    }
}

/*!
 * @brief                   Converts dwt cycles to milliseconds
 *
 * @param[in] dwt_cycles
 *
 * @return                  TIme in milliseconds
 */
uint32_t dwt_cycles_to_ms(uint32_t dwt_cycles)
{
    // 48 represents the clock frequency in MHz
    return dwt_cycles * (1.0f / (216 * 1000.0f)) ;
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    printf("Error Handler");
}
