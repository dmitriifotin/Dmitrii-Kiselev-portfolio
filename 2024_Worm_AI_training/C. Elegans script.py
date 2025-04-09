#python
import random
import tensorflow as tf
from tensorflow.keras import layers, models
import numpy as np
import datetime
import os
import time

model_path = "worm_gen_3.keras"


def create_model(input_size, output_size):
    model = models.Sequential()
    model.add(layers.Dense(128, activation='relu', input_shape=(input_size,)))
    model.add(layers.Dense(128, activation='relu'))
    model.add(layers.Dense(64, activation='relu'))
    model.add(layers.Dense(output_size, activation='tanh'))
    model.compile(optimizer='adam', loss='mse')
    return model

def get_sensor_data(joint_handles, sensor_handles):
    sensor_data = []
    for joint in joint_handles:
        sensor_data.append(sim.getJointPosition(joint))
    for proximity_sensor in sensor_handles:
        result, distance, _, _, _ = sim.readProximitySensor(proximity_sensor)
        sensor_data.append(distance if result else -1.0)
    return np.array(sensor_data)

def set_actuator_commands(joint_handles, commands):
    for joint, command in zip(joint_handles, commands):
        print(f"Setting joint {joint} to position {command}")  # Debug line
        sim.setJointTargetPosition(joint, command)

def compute_reward(previous_position, current_position):
    forward_reward = np.linalg.norm(np.array(current_position[:2]) - np.array(previous_position[:2]))
    return forward_reward



def sysCall_init():
    sim = require('sim')
    global model, joint_handles, sensor_handles, robot_handle, previous_position, log_dir, tensorboard_callback, episode, total_reward, epsilon
                    
    log_dir = "logs/fit/" + datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
    tensorboard_callback = tf.keras.callbacks.TensorBoard(log_dir=log_dir, histogram_freq=1)

    # Load or create model
    if os.path.exists(model_path):
        model = tf.keras.models.load_model(model_path)
        print("Model loaded from disk.")
    else:
        input_size = 60  # Number of joints
        output_size = 37  # Number of actuators
        model = create_model(input_size, output_size)
        print("New model created.")
    
    robot_handle = sim.getObject(".")
    joint_handles = [sim.getObject(":/REV11"), 
                sim.getObject(':/REV11/Cuboid/PRIS1'),
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72/Cuboid/REV81'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72/Cuboid/REV81/Cuboid/PRIS8'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72/Cuboid/REV81/Cuboid/PRIS8/Cuboid/REV82'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72/Cuboid/REV81/Cuboid/PRIS8/Cuboid/REV82/Cuboid/REV91'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72/Cuboid/REV81/Cuboid/PRIS8/Cuboid/REV82/Cuboid/REV91/Cuboid/PRIS9'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72/Cuboid/REV81/Cuboid/PRIS8/Cuboid/REV82/Cuboid/REV91/Cuboid/PRIS9/Cuboid/REV92'), 
                sim.getObject(':/PRIS12'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/PRIS22'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/PRIS32'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/PRIS42'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/PRIS52'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/PRIS62'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/PRIS72'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72/Cuboid/PRIS82'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72/Cuboid/REV81/Cuboid/PRIS8/Cuboid/REV82/Cuboid/PRIS92'), 
                sim.getObject(':/REV11/Cuboid/PRIS1/Cuboid/REV12/Cuboid/REV21/Cuboid/PRIS2/Cuboid/REV22/Cuboid/REV31/Cuboid/PRIS3/Cuboid/REV32/Cuboid/REV41/Cuboid/PRIS4/Cuboid/REV42/Cuboid/REV51/Cuboid/PRIS5/Cuboid/REV52/Cuboid/REV61/Cuboid/PRIS6/Cuboid/REV62/Cuboid/REV71/Cuboid/PRIS7/Cuboid/REV72/Cuboid/REV81/Cuboid/PRIS8/Cuboid/REV82/Cuboid/REV91/Cuboid/PRIS9/Cuboid/REV92/Cuboid/PRIS102')]
    sensor_handles = [sim.getObjectHandle('FR'), 
                    sim.getObjectHandle('FL'), 
                    sim.getObjectHandle('R1'), 
                    sim.getObjectHandle('L1'), 
                    sim.getObjectHandle('R2'), 
                    sim.getObjectHandle('L2'), 
                    sim.getObjectHandle('R3'), 
                    sim.getObjectHandle('L3'), 
                    sim.getObjectHandle('R4'), 
                    sim.getObjectHandle('L4'), 
                    sim.getObjectHandle('R5'), 
                    sim.getObjectHandle('L5'), 
                    sim.getObjectHandle('R6'), 
                    sim.getObjectHandle('L6'), 
                    sim.getObjectHandle('R7'), 
                    sim.getObjectHandle('L7'), 
                    sim.getObjectHandle('R8'), 
                    sim.getObjectHandle('L8'), 
                    sim.getObjectHandle('R9'), 
                    sim.getObjectHandle('L9'), 
                    sim.getObjectHandle('R10'), 
                    sim.getObjectHandle('L10'), 
                    sim.getObjectHandle('B')]
    previous_position = sim.getObjectPosition(robot_handle, -1)
    episode = 0
    total_reward = 0
    epsilon = 1.0

def sysCall_actuation():
    global previous_position, total_reward, epsilon

    # Sensor data collection
    sensor_data = get_sensor_data(joint_handles, sensor_handles)
    sensor_data = sensor_data.astype(np.float32).reshape(1, -1)
    # Debugging: Print shape and data type of sensor data
    print(f"Sensor data shape: {sensor_data.shape}, data type: {sensor_data.dtype}")

    
    if random.uniform(0, 1) < epsilon:
        # Random action for exploration
        action = np.random.uniform(low=-1.0, high=1.0, size=(len(joint_handles),)).astype(np.float32)  # Ensure float32
        print(f"Random action: {action}")
    else:
        # Predict action using the model
        action = model.predict(sensor_data)
        print(f"Predicted action: {action}")
    
    action = action.reshape(-1)
    commands = action 
    print(f"Predicted action shape: {action.shape}, data type: {action.dtype}")
    print(f"Commands: {commands}")
    commands = np.clip(commands, -30.0, 30.0)
    # Set actuator commands
    set_actuator_commands(joint_handles, commands)

    # Compute reward
    current_position = sim.getObjectPosition(robot_handle, -1)
    reward = compute_reward(previous_position, current_position)
    total_reward += reward

    # Update previous position
    previous_position = current_position

    try:
        target = reward + 0.99 * np.amax(model.predict(sensor_data))
        target_f = model.predict(sensor_data)
        target_f[0][np.argmax(action)] = target
        model.fit(sensor_data, target_f, epochs=1, verbose=0, callbacks=[tensorboard_callback])
    except Exception as e:
        print(f"Training error: {e}")  # Debug line

    # Reduce epsilon (less exploration over time)
    if epsilon > 0.01:
        epsilon *= 0.995
    
def sysCall_sensing():
    # put your sensing code here
    pass

def sysCall_cleanup():
    global episode, total_reward
    
    # Save the model after each episode
    model.save(model_path)
    print(f"Model saved after episode {episode}. Total Reward: {total_reward}")

    # Increment episode count and reset total reward
    episode += 1
    total_reward = 0