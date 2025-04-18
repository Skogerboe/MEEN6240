                                                                            **ME EN 6240 Final Motor Control Project**

This repository holds the code and results of the MEEN 6240  Motor control Project:

- ADC.c/.h: Contains code for initializing the ADC for Current monitoring, and functions to obtain ADC counts and convert to current.
- Encoder.c/.h: Contains code for initializing the encoder for reading motor position in both counts and degrees.
- main.c: Contains the main file and includes other header files for all source files.
- utilities.c/.h: Contains code initialing timer peripherals, PWM, and sets up a state machine for both the 200Hz and 5kHz ISR where the mode (IDLE, PWM, ITEST, HOLD, TRACK) is the input. These files also contain some global variables and structures, as well as functions related to setting and getting the mode, and a general PID control function that takes a struct and a float reference value.

Current loop gain tests:

kp: 900
ki: 0.0009
![image](https://github.com/user-attachments/assets/707c0e5f-183b-428a-86e3-81fd11ac4226)

kp: 10000
ki: 0.001
![image](https://github.com/user-attachments/assets/89590322-3746-4aa9-96eb-b7314dab0e65)

Position loop control gains were tested and the most consistent and accurate gains were as follows:

kp: 6
ki: 0.00001
kd: 0

There is a small bit of oscillation and overshoot, but is very quick. Other gain combinations resulted in higher oscillation and inaccuracy.

