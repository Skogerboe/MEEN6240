#ifndef UTILITIES__H__
#define UTILITIES__H__

#include <xc.h>                     // processor SFR definitions
#include <sys/attribs.h>            // __ISR macro

#define IDLE	0
#define PWM		1
#define ITEST	2
#define HOLD	3
#define TRACK	4


int op_mode;

typedef struct {
	int pwm;
	int duty_p;
	int direction;	
	int motoren;
	int mode;
}modevars;

typedef struct {
	float P;
	float I;
	float D;
}GAINS;

GAINS CurrCtrl;

int set_mode(modevars *modevar);	
int get_mode(modevars *modevar);
void init_ControlLoop(void);
void config_PWM(void);
void config_T4(void);

#endif // UTILITIES__H__
