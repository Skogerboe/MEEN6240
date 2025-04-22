**ME EN 6240 Final Motor Control Project**

This repository holds the code and results of the MEEN 6240  Motor control Project:

- ADC.c/.h: Contains code for initializing the ADC for Current monitoring, and functions to obtain ADC counts and convert to current.
- Encoder.c/.h: Contains code for initializing the encoder for reading motor position in both counts and degrees.
- main.c: Contains the main file and includes other header files for all source files.
- utilities.c/.h: Contains code initialing timer peripherals, PWM, and sets up a state machine for both the 200Hz and 5kHz ISR where the mode (IDLE, PWM, ITEST, HOLD, TRACK) is the input. These files also contain some global variables and structures, as well as functions related to setting and getting the mode, and a general PID control function that takes a struct and a float reference value.

Schematic/Diagram/Tables:
![image](https://github.com/user-attachments/assets/450453da-f9c4-45b6-83bf-7bbb437d8e7d)
![image](https://github.com/user-attachments/assets/346b52d8-2aad-4f6f-8f1f-4fb2bd7b7b4b)

![image](https://github.com/user-attachments/assets/c5d82180-69a7-4850-8394-544ebc1938a6)



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
After further testing, position control gains were improved to minimize oscillation. There are still some oscillation, but cubic and step trajectories are followed fairly well.
The new Position gains tuned to get the best reponses are as follows:

kp: 15
ki: 0.000006
kd: 0.08

For the current loop gains of kp = 10000 and ki = 0.001, and position loop gains of kp = 15, ki = 0.000006, kd = 0.08, the following responses are recorded:

![image](https://github.com/user-attachments/assets/5bf26eb0-ac59-4f8d-bac3-b1d02ddecd5b)

![image](https://github.com/user-attachments/assets/ece6d563-3e94-47b1-8cd2-01e62f745acc)



