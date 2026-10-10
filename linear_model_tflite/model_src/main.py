#Creating a base model to predict y=3x+1

import numpy as np
import tensorflow as tf
from tensorflow import keras

x_train=np.random.uniform(-10,10,size=(1000,1)).astype(np.float32)
y_train=(3*x_train+1)+np.random.normal(0, 0.1, size=(1000, 1)).astype(np.float32)

model=keras.Sequential([
    keras.layers.Input(shape=(1,)),
    keras.layers.Dense(16,activation="relu"),
    keras.layers.Dense(1)
])

model.compile(optimizer="adam",loss="mse")

print("Starting the training of the model")

model.fit(x_train,y_train,epochs=200,batch_size=32,verbose=1)

def rep_dataset():
    for i in range(100):
        yield [x_train[i:i+1]]

print("Initialising the converter")

converter = tf.lite.TFLiteConverter.from_keras_model(model)

converter.optimizations = [tf.lite.Optimize.DEFAULT]

converter.representative_dataset = rep_dataset

converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]

converter.inference_input_type = tf.int8

converter.inference_output_type = tf.int8

tflite_model = converter.convert()

with open("model.tflite", "wb") as f:
    f.write(tflite_model)
print("Success: Saved model.tflite")

def generate_c_array(bytes_data, file_name, array_name):
    hex_array = [f"0x{b:02x}" for b in bytes_data]
    hex_str = ", ".join(hex_array)
    with open(file_name, "w") as f:
        f.write(f"const unsigned char {array_name}[] = {{{hex_str}}};\n")
        f.write(f"const unsigned int {array_name}_len = {len(bytes_data)};\n")

generate_c_array(tflite_model, "model_data.h", "g_model")
print(f"Success: Exported {len(tflite_model)} bytes to model_data.h")