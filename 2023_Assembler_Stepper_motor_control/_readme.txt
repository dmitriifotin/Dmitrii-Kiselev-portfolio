ARM processor stepper motor control

The goal of the project was to further research how to make the selected ARM processor communicate with external devices instead of just onboard LEDs, which involved researching data transmit requirements between devices, encoding and register setup.
The requirement was to make the BEAGL-BONE-BLACK send a sequence of commands that make a stepper motor rotate. The commands had to pass through a motor driver, which had particular requirements for how to initiate a data transfer and receive confirmation. The complicated part was to translate all info for the processor and the motor in terms of the motor driver architecture, as it acted as a conduit.

MCU: BEAGL-BONE-BLACK
Script: Assembly
Components: BEAGL-BONE-BLACK, motor driver, stepper motor
Timeline: 2 months