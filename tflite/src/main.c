#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"


#include <model_data.h>

LOG_MODULE_REGISTER(tflite,LOG_LEVEL_INF);

constexpr int kTensorArenaSize = 2 * 1024; 
alignas(16) static uint8_t tensor_arena[kTensorArenaSize];

int main(void)
{
    LOG_INF("STARTING THE AI MODELS");
    
    tflite::InitializeAgent();

    const tflite::Model* model = tflite::GetModel(g_model);
    if(model->version() != TFLITE_SCHEMA_VERSION)
    {
        LOG_INF("Version outdated or mismatch");
    }

    tflite::MicroMutableOpResolver <1> resolver;
    if (resolver.AddFullyConnected() != kTfLiteOk) {
        LOG_ERR("Failed to register operations.");
        return 0;
    }

    tflite::MicroInterpreter interpreter(model,resolver,tensor_arena,kTensorArenaSize);
    if(interpreter.AllocateTensors() != kTfLiteOk)
    {
        LOG_ERR("Failed to allocate tensors");
        return 0;
    }

    TfLiteIntArray* input=interpreter.input(0);
    TfLiteIntArray* output=interpreter.output(0);

    int current_sensor_val=0;

    while(1)
    {
        input->data.f[0]=current_sensor_val;
        
        if (interpreter.Invoke() != kTfLiteOk) {
            LOG_ERR("Inference failed!");
            break;
        }

        float prediction = output->data.f[0];


    }
    return 0;
}