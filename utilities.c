#include "utilities.h"  
#include "ADC.h"
#include "encoder.h"


int state_5kHz(modevars *modevar) {
	int Itestval, Itestout, Imon;
	float Itestdt;
	
	
	switch(modevar->mode) {
		
		case IDLE:
		{
			modevar->duty_p = 0;
		
			break;
		}
		case PWM:
		{
			/*if(modevar->pwm < 0) {				//Set direction bit. Clockwise for negative PWM, CCW for Positive PWM.
				LATCbits.LATC14 = 0;
			}else{
				LATCbits.LATC14 = 1;
			}*/
			
			modevar->duty_p = (abs(modevar->pwm) * (PR2 + 1)) / 100;			//duty cycle = OCxR/(PRy + 1) x 100%.
			break;
		}
		case ITEST:
		{
			static volatile int Itestcount;
			
			CurrCtrl.dt = dt_CurrCtrl;		//Itest sampling period
			if ((Itestcount == 25) || (Itestcount == 50) || (Itestcount == 75)) {		//Every 25 iterations through ISR, Flip sign of Itest ref
				CurrCtrl.ref = (-1)*CurrCtrl.ref;
			}
			
			Itestval = ADC_ma(read_ADC());		//Read ADC value in mA
			modevar->pwm = (int)PID_Out(&CurrCtrl, (float)Itestval);	//Set PWM to Control signal output.
		
			modevar->duty_p = (abs(modevar->pwm) * (PR2 + 1)) / 100;	//Duty cycle period calculation.
			
			Itest_data_real[Itestcount] = Itestval;		//Store Itest reference in this array
			Itest_ref[Itestcount] = CurrCtrl.ref;			//Store Itest real value in this array
			Itestcount++;								//Increment counter	
			if (Itestcount > 99) {						//Reset Itest count, set mode to IDLE, and set Itest Data send flag.
				CurrCtrl.eint = 0;
				Itestcount = 0;
				set_modee(modevar, IDLE);
				Itest_Data_f = 1;
			}
					
			
			break;
		}
		case HOLD:
		{
			PosCtrl.dt = dt_PosCtrl;
			Imon = ADC_ma(read_ADC());
			CurrCtrl.ref = Iref;
			modevar->pwm = (int)PID_Out(&CurrCtrl, (float)Imon);
			
			modevar->duty_p = (abs(modevar->pwm) * (PR2 + 1)) / 100;	//Duty cycle period calculation.
			break;
		}
		case TRACK:
		{
			
			PosCtrl.dt = dt_PosCtrl;
			Imon = ADC_ma(read_ADC());
			CurrCtrl.ref = Iref;
			modevar->pwm = (int)PID_Out(&CurrCtrl, (float)Imon);
			
			modevar->duty_p = (abs(modevar->pwm) * (PR2 + 1)) / 100;	//Duty cycle period calculation.
			
			break;
		}
		default:
		{
			
			
			break;
		}
		
		
	}
	
	return modevar->duty_p;
}

int state_200Hz(modevars *modevar) {
	int Angle;
	static volatile int trackcount=0;
	
	switch(modevar->mode) {
		case IDLE:
		{
			break;
		}
		case PWM:
		{
			break;
		}
		case ITEST:
		{
			break;
		}
		case HOLD:
		{
			//PosCtrl.ref = (-1) * PosCtrl.ref;
			Angle = count_to_deg(encoder_counts());
			PosCtrl.ref = modevar->pos;
			modevar->current = PID_Out(&PosCtrl, Angle);
			
			break;
		}
		case TRACK:
		{
			if(trackcount < trajectory_size) {
				Angle = count_to_deg(encoder_counts());
				Track_data_real[trackcount] = Angle;
				PosCtrl.ref = Track_data_ref[trackcount];
				modevar->current = PID_Out(&PosCtrl, Angle);
				trackcount++;
			}else {
				Angle = count_to_deg(encoder_counts());
				modevar->pos = Track_data_ref[trajectory_size-1];
			
				Track_Data_f = 1;
				trackcount = 0;
				CurrCtrl.eint = 0;
				CurrCtrl.eprev = 0;
				PosCtrl.eint = 0;
				PosCtrl.eint = 0;
				set_modee(modevar, HOLD);
			}
			break;
		}
		default:
		{
			
			break;
		}
		
		
	}
	return modevar->current;
}

float PID_Out(GAINS *pidctrl, float real_val) {
	
	float e=0;
	float ed = 0;
	float out=0;	
	
	e = pidctrl->ref - real_val;		//Set error
	
	pidctrl->eint += e;//*dt;			//Set integration of error
	if(pidctrl->eint > pidctrl->int_max) {		//Integration Anti-Windup
		pidctrl->eint = pidctrl->int_max;
	}
	if(pidctrl->eint < pidctrl->int_min) {
		pidctrl->eint = pidctrl->int_min;
	}
	
	ed = (e - pidctrl->eprev) / pidctrl->dt;			//Derivative term
	pidctrl->eprev = e;						//Set eprev value for derivative term.
	out = (pidctrl->kp * e) + (pidctrl->ki * pidctrl->eint) + (pidctrl->kd * ed);			//Control signal output
	
	if(out > pidctrl->int_max) {		//Clamp controller output to limits.
		out = pidctrl->int_max;
	}
	if(out < pidctrl->int_min) {
		out = pidctrl->int_min;
	}
	
	return out;	
}

void set_modee(modevars *modevar, int mode) {
	modevar->mode = mode;	
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

void config_T3(void) {
	
	PR3 = 6249;
	TMR3 = 0;
	T3CONbits.TCKPS = 6;
	T3CONCLR = 2;  
	T3CONbits.TGATE = 0; 
	T3CONbits.ON = 1;			//Timer Initialize for 5kHz current controller.
	
	IPC3bits.T3IP = 4;
	IPC4bits.T4IS = 0;
	IFS0bits.T3IF = 0;
	IEC0bits.T3IE = 1;	
}	

void array_clr(int *array) {
	int size = sizeof(array);
	int c;
	
	for (c = 0; c < size; c++) {
			array[c] = 0;
	}
	
}