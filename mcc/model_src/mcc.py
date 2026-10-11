import tensorflow as tf
from tensorflow import keras
import numpy as np

standard_vector=[
    [0.0,0.0,1.0],[0.0,0.0,-1.0],   #Z-axis
    [0.0,1.0,0.0],[0.0,-1.0,0.0],   #Y-axis
    [1.0,0.0,0.0],[-1.0,0.0,0.0],   #X-axis
]
x_data=[]
y_data=[]

for label,vector in enumerate(standard_vector):
    for i in range(500):
        noisy_reading=np.array(vector)+np.random.normal(0,0.15,size=3)
        x_data.append(noisy_reading)
        y_data.append(label)

x_data=np.array(x_data,dtype=np.float32)
y_data=np.array(y_data,dtype=np.int32)

model = tf.keras.Sequential(
    [
        tf.keras.layers.Input(shape=(3,)),
        tf.keras.layers.Dense(16,activation='relu'),
        tf.keras.layers.Dense(6)
    ]
)

model.compile(optimizer="adam",loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),metrics=['accuracy'])

model.fit(x_data,y_data,epochs=150,batch_size=32,validation_split=0.2,verbose=1)

def conver_to_array(array,filename,arrayname):
    hex_array=[f"0x{b:02x}" for b in array]
    hex_array=",".join(hex_array)
    with open(f"{filename}.h",'w') as f:
        f.write(f"const unsigned char {arrayname}[]={{ {hex_array}}};\n")
        f.write(f"const unsigned int {arrayname}_len={len(array)};\n")
        

def representative_dataset():
    for i in range(100):
        yield [x_data[i:i+1]]

#Converting float32 to int32

converter=tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations=[tf.lite.Optimize.DEFAULT]
converter.target_spec.supported_ops=[tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
#Setting input and output type to int8
converter.inference_input_type=tf.int8
converter.inference_output_type=tf.int8
#Representative dataset for calibration(to find min-max values)
converter.representative_dataset=representative_dataset

tf_lite_model=converter.convert()
with open('mcc_model.tflite','wb') as f:
    f.write(tf_lite_model)

conver_to_array(tf_lite_model,"mcc_model","g_model")

print('TFLite model size:',len(tf_lite_model),'bytes')
    