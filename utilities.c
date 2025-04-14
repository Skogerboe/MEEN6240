#include "utilities.h"  
#include "ADC.h"

int set_mode(modevars *modevar) {
	int Itestsp, Itestcount, Itestval, Itestout;
	float Itestdt;
	
	
	switch(modevar->mode) {
		
		case IDLE:
		{
			modevar->duty_p = 0;
		
			break;
		}
		case PWM:
		{
			if(modevar->pwm < 0) {
				LATCbits.LATC14 = 0;
			}else{
				LATCbits.LATC14 = 1;
			}
			
			modevar->duty_p = (abs(modevar->pwm) * (PR2 + 1)) / 100;			//duty cycle = OCxR/(PRy + 1) x 100%.
			break;
		}
		case ITEST:
		{
			Itestdt = dt_CurrCtrl;
			if(Itestcount == 0) {
				Itestsp = 200;
			}
			if ((Itestcount == 25) || (Itestcount == 75) || (Itestcount == 50)) {
				Itestsp = -1 * Itestsp;
			}
			Itestval = ADC_ma(read_ADC());
			Itestout = PID_Out(&CurrCtrl, (float)Itestsp, (float)Itestval, Itestdt);
			
			if (Itestout > 100) {
				Itestout = 100;
			}
			if (Itestout < -100) {
				Itestout = -100;
			}

			if (Itestcount == 99) {	
				Itestcount = 0;
				modevar->mode = IDLE;
			}								
			
			modevar->duty_p = Itestout;
			break;
		}
		case HOLD:
		{
			
			
			break;
		}
		case TRACK:
		{
			
			
			break;
		}
		default:
		{
			
			
			break;
		}
		
		
	}
	
	return modevar->duty_p;
}

float PID_Out(GAINS *pidctrl, float setpoint, float real_val, float dt) {
	
	float e, ed, out;
	
	e = setpoint - real_val;
	pidctrl->eint += e*dt;
	ed = (e - pidctrl->eprev) / dt;
	
	out = pidctrl->kp * e + pidctrl->ki * pidctrl->eint + pidctrl.kd * ed;
	
	pidctrl->eprev = e;
	
	
	return out;	
}

int get_mode(modevars *modevar) {
	
	return modevar->mode;
	
}

void init_ControlLoop(void) {
	
	config_PWM();
	
	TRISCbits.TRISC14 = 0;		//Output bit for motor direction
	
	config_T4();
	
}

void config_PWM(void) {
	
	//TRISDbits.TRISD0 = 0;		//Initialize OC1 on RD0
	
	PR2 = 3999;
	//TMR2 = 0;
/*	T2CONbits.TCKPS = 0;
	T2CONCLR = 2; 
	T2CONbits.TGATE = 0; */
	
	OC1CONbits.ON = 0;
	OC1CONbits.OCTSEL = 0;
	OC1CONbits.OCM = 0b110;
	OC1R = 1000;
	OC1RS = 1000;
	
	T2CONbits.ON = 1;
	OC1CONbits.ON = 1;
	
/*	IPC2bits.T2IP = 5;              // INT step 4: priority
    IPC2bits.T2IS = 0;              //             subpriority
    IFS0bits.T2IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T2IE = 1;    */          // INT step 6: enable interrupt 
	
	IPC1bits.OC1IP = 5;              // INT step 4: priority
    IPC1bits.OC1IS = 1;              //             subpriority
    IFS0bits.OC1IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.OC1IE = 1;              // INT step 6: enable interrupt
	
}

void config_T4(void) {
	
	
	PR4 = 15999;
	TMR4 = 0;
	T4CONbits.TCKPS = 0;
	T4CONCLR = 2;  
	T4CONbits.TGATE = 0; 
	T4CONbits.ON = 1;			//Timer Initialize for 5kHz current controller.
	
	IPC4bits.T4IP = 5;              // INT step 4: priority
    IPC4bits.T4IS = 2;              //             subpriority
    IFS0bits.T4IF = 0;              // INT step 5: clear interrupt flag
    IEC0bits.T4IE = 1;              // INT step 6: enable interrupt
	
}
