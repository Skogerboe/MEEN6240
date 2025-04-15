#ifndef UTILITIES__H__
#define UTILITIES__H__

#include <xc.h>                     // processor SFR definitions
#include <sys/attribs.h>            // __ISR macro

#define IDLE	0
#define PWM		1
#define ITEST	2
#define HOLD	3
#define TRACK	4
#define dt_CurrCtrl		0.0002


int op_mode;
volatile int Itest_Data_f;
int Itest_data_real[100], Itest_ref[100];


typedef struct {
	int pwm;
	int duty_p;
	int direction;	
	int motoren;
	int mode;
}modevars;

typedef struct {
	float kp;
	float ki;
	float kd;
	float e;
	float eprev;
	float eint;
	float int_min;
	float int_max;
}GAINS;

GAINS CurrCtrl;

int set_mode(modevars *modevar);	
int get_mode(modevars *modevar);
void init_ControlLoop(void);
void config_PWM(void);
void config_T4(void);
float PID_Out(GAINS *pidctrl, float setpoint, float real_val, float dt);

#endif // UTILITIES__H__
