#include<zephyr/kernel.h>
#include<zephyr/logging/log.h>

#include<tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include<tensorflow/lite/micro/micro_interpreter.h>
#include<tensorflow/lite/micro/system_setup.h>
#include<tensorflow/lite/schema/schema_generated.h>

#include<sine_model.h>

LOG_MODULE_REGISTER(tflite,LOG_LEVEL_INF);

constexpr int kTensorArenaSize = 2*2048;
alignas(16) static uint8_t tensor_arena[kTensorArenaSize];


int main(void)
{
    LOG_INF("Starting AI Model (sine)");
    
    tflite::InitializeTarget();

    const tflite::Model *model = tflite::GetModel(sine_mod);
    if(model->version() != TFLITE_SCHEMA_VERSION)
    { 
        LOG_ERR("Version outdated or mismatch");
        return 0;
    }

    static tflite::MicroMutableOpResolver<1> resolver;
    if(resolver.AddFullyConnected() != kTfLiteOk)
    {
        LOG_ERR("Failed to register operations");
        return 0;
    }
    
    tflite::MicroInterpreter interpreter(model, resolver,tensor_arena,kTensorArenaSize);
    if(interpreter.AllocateTensors() != kTfLiteOk)
    {
        LOG_ERR("Failed to allocate tensors");
        return 0;
    }

    TfLiteTensor *input = interpreter.input(0);
    TfLiteTensor *output = interpreter.output(0);

    float input_data = 0.0f;
    
    const float in_scale = input->params.scale;
    int in_zero_point = input->params.zero_point;
    const float out_scale = output->params.scale;
    int out_zero_point = output->params.zero_point;

    while(1)
    {
        int8_t input_quantized = (int8_t)(input_data / in_scale)+in_zero_point;

        if(input_quantized > 127)
            input_quantized = 127;

        if(input_quantized < -128)
            input_quantized = -128;
        
        input->data.int8[0] = input_quantized;

        if(interpreter.Invoke() != kTfLiteOk)
        {
            LOG_ERR("Failed to invoke interpreter");
            break;
        }

        int8_t output_quantized = output->data.int8[0];
        float output_data = (output_quantized - out_zero_point) * out_scale;

        LOG_INF("Input: %.2f | Quantized: %d ---> Output Quantized: %d | Real Prediction: %.2f",
                input_data,
                input_quantized,
                output_quantized,
                output_data);

        input_data += 0.1f;
        if(input_data > 2*3.14f)
            input_data = 0.0f;

        k_msleep(1000);
    }

    
    return 0;
}