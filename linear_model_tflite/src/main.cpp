#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/system_setup.h>
#include <tensorflow/lite/schema/schema_generated.h>

#include <model_data.h>

LOG_MODULE_REGISTER(tflite, LOG_LEVEL_INF);

constexpr int kTensorArenaSize = 2 * 1024;
alignas(16) static uint8_t tensor_arena[kTensorArenaSize];

int main(void)
{
    LOG_INF("STARTING THE AI MODELS");

    tflite::InitializeTarget();

    const tflite::Model *model = tflite::GetModel(g_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        LOG_ERR("Version outdated or mismatch (Model: %d, Schema: %d)",
                model->version(), TFLITE_SCHEMA_VERSION);
        return 0;
    }

    static tflite::MicroMutableOpResolver<1> resolver;
    if (resolver.AddFullyConnected() != kTfLiteOk) {
        LOG_ERR("Failed to register operations.");
        return 0;
    }

    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, kTensorArenaSize);
    if (interpreter.AllocateTensors() != kTfLiteOk) {
        LOG_ERR("Failed to allocate tensors");
        return 0;
    }

    TfLiteTensor *input = interpreter.input(0);
    TfLiteTensor *output = interpreter.output(0);

    float current_sensor_val = 1.0f;

    float in_scale = input->params.scale;
    int in_zero_point = input->params.zero_point;

    float out_scale = output->params.scale;
    int out_zero_point = output->params.zero_point;

    while (1) {
        int32_t quantized_input = (int32_t)(current_sensor_val / in_scale) + in_zero_point;

        if (quantized_input > 127) quantized_input = 127;
        if (quantized_input < -128) quantized_input = -128;

        input->data.int8[0] = (int8_t)quantized_input;

        if (interpreter.Invoke() != kTfLiteOk) {
            LOG_ERR("Inference failed");
            break;
        }

        int8_t quantized_output = output->data.int8[0];

        float prediction = (quantized_output - out_zero_point) * out_scale;

        LOG_INF("Real Input: %.2f | Quantized: %d ---> Output Quantized: %d | Real Prediction: %.2f",
                (double)current_sensor_val,
                input->data.int8[0],
                output->data.int8[0],
                (double)prediction);

        current_sensor_val += 1.5f;
        k_msleep(2000);
    }

    return 0;
}
