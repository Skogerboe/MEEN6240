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
#define dt_PosCtrl		0.005
#define ITEST_IREF		200
#define ITEST_DATAPOINTS	100
#define PWM_MIN		-100
#define PWM_MAX		100
#define IMIN		-300
#define IMAX		300
#define HOLD_DATAPOINTS		100


extern volatile int Itest_Data_f, Hold_Data_f;
extern volatile int Itest_data_real[100], Itest_ref[100];
extern int Ival, Iref, PosVal, Posref;


typedef struct {
	int pwm;
	int duty_p;
	int direction;	
	int motoren;
	int mode;
	int pos;
	int current;
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
	volatile float ref;
	float dt;
}GAINS;

extern GAINS CurrCtrl;
extern GAINS PosCtrl;

int set_mode(modevars *modevar);
void set_modee(modevars *modevar, int mode);
int state_5kHz(modevars *modevar);
int state_200Hz(modevars *modevar);	
int get_mode(modevars *modevar);
void init_ControlLoop(void);
void config_PWM(void);
void config_T4(void);
void config_T3(void);
float PID_Out(GAINS *pidctrl, float real_val);

#endif // UTILITIES__H__
