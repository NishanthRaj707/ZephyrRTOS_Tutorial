import tensorflow as tf
from tensorflow import keras
import numpy as np

np.random.seed(42)
NUM_SAMPLES = 2000

x_values = np.random.uniform(0, 2 * np.pi, NUM_SAMPLES).astype(np.float32)
np.random.shuffle(x_values)

y_values = np.sin(x_values).astype(np.float32)
y_values += 0.1 * np.random.randn(*y_values.shape)

split = int(0.8 * NUM_SAMPLES)
x_train, x_validate = x_values[:split], x_values[split:]
y_train, y_validate = y_values[:split], y_values[split:]


model=keras.Sequential(
    [
        keras.layers.Input(shape=(1,)),
        keras.layers.Dense(16,activation="relu"),
        keras.layers.Dense(16,activation="relu"),
        keras.layers.Dense(1)
    ]
)

model.compile(optimizer="adam",loss="mse",metrics=["mae"])

EPOCHS=250
BATCH_SIZE=32

history=model.fit(x_train,y_train,epochs=EPOCHS,batch_size=BATCH_SIZE,validation_data=(x_validate,y_validate),verbose=1)

def representative_dataset():
    for i in range(100):
        yield [x_train[i:i+1]]

converter=tf.lite.TFLiteConverter.from_keras_model(model)

converter.optimizations=[tf.lite.Optimize.DEFAULT]

converter.representative_dataset = representative_dataset

converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]

converter.inference_input_type = tf.int8

converter.inference_output_type = tf.int8

tflite_model = converter.convert()

with open("sine.tflite","wb") as f:
    f.write(tflite_model)


def generate_c_array(array,filename,arrayname):
    hex_array=[f"0x{b:02x}" for b in array]
    hex_str=','.join(hex_array)
    with open(f"{filename}.h","w") as f:
        f.write(f"const unsigned char {arrayname}[] = {{{hex_str}}};\n")
        f.write(f"const unsigned int {arrayname}_len = {len(array)};\n")

generate_c_array(tflite_model,"sine_model.h","sine_mod")
print(f"Success: Exported {len(tflite_model)} bytes to model_data.h")
    

