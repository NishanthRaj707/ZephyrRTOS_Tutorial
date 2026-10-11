#include<zephyr/kernel.h>
#include<zephyr/logging/log.h>

#include<tensorflow/lite/micro/micro_interpreter.h>
#include<tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include<tensorflow/lite/schema/schema_generated.h>
#include<tensorflow/lite/micro/system_setup.h>

#include<mcc_model.h>

LOG_MODULE_REGISTER(tflite_mcc,LOG_LEVEL_INF);

constexpr int kTensorArenaSize = 4096;
alignas(16) static uint8_t tensor_arena[kTensorArenaSize];

int main(void)
{
    LOG_INF("Starting MCC Inference");
    tflite::InitializeTarget();
    const tflite::Model *model = tflite::GetModel(g_model);

    if(model->version() != TFLITE_SCHEMA_VERSION)
    {
        LOG_ERR("Model version mismatch");
        return 0;
    }
    static tflite::MicroMutableOpResolver<1> resolver;
    if(resolver.AddFullyConnected() != kTfLiteOk)
    {
        LOG_ERR("Failed to add operations");
        return 0;
    }
    tflite::MicroInterpreter interpreter(model,resolver,tensor_arena,kTensorArenaSize);

    if(interpreter.AllocateTensors() != kTfLiteOk)
    {
        LOG_ERR("Failed to allocate tensors");
        return 0;
    }

    TfLiteTensor *input = interpreter.input(0);
    TfLiteTensor *output = interpreter.output(0);

    const float in_scale = input->params.scale;
    const int in_zero_point = input->params.zero_point;
    const float out_scale = output->params.scale;
    const int out_zero_point = output->params.zero_point;

    float input_x =0.0f ;
    float input_y =1.0f ;
    float input_z =0.0f ;
    while(1){
        input->data.int8[0]=(input_x/in_scale)+in_zero_point;
        input->data.int8[1]=(input_y/in_scale)+in_zero_point;
        input->data.int8[2]=(input_z/in_scale)+in_zero_point;
        if(interpreter.Invoke()!=kTfLiteOk)
        {
            LOG_ERR("Failed to invoke interpreter");
            break;
        }
         
        int max_score=-128;
        int best_class = 0;
        for(int i=0;i<6;i++)
        {
            if(output->data.int8[i]>max_score)
            {
                max_score=output->data.int8[i];
                best_class=i;
            }
        }
    }
    
    return 0;
}