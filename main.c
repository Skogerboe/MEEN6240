#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "utilities.h"
#include "ADC.h"
#include "encoder.h"
// include other header files here

#define BUF_SIZE 200


volatile int Itest_Data_f, Track_Data_f;
GAINS CurrCtrl, PosCtrl;
volatile int Itest_data_real[100], Itest_ref[100];
int Ival, Iref, PosVal, Posref;
modevars modevar;
int trajectory_pos, trajectory_size;
int Track_data_ref[MAX_TRAJ_SIZE], Track_data_real[MAX_TRAJ_SIZE];


void __ISR(_TIMER_3_VECTOR, IPL4SOFT) TIMER3ISR(void) {
	
	IFS0CLR = (1<<12);
	TMR3 = 0;
	Iref = state_200Hz(&modevar);
	//NU32_LED1 = !NU32_LED1;
}

void __ISR(_OUTPUT_COMPARE_1_VECTOR, IPL5SOFT) OC1ISR(void) {
	IFS0CLR = (1<<5);
}

void __ISR(_TIMER_4_VECTOR, IPL5SOFT) TIMER4ISR(void) {
	IFS0CLR = (1<<16);
	OC1RS = state_5kHz(&modevar);
	if(modevar.pwm < 0) {				//Set direction bit. Clockwise for negative PWM, CCW for Positive PWM.
		LATCbits.LATC14 = 0;
	}else{
		LATCbits.LATC14 = 1;
	}
	TMR4 = 0;
}

int main() 
{
  char buffer[BUF_SIZE];
  static int i;
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;        
  
  modevar.mode = IDLE;
  Itest_Data_f = 0;
  
  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  encoder_init();
  init_ADC();
  init_ControlLoop();
  config_T3();
  __builtin_enable_interrupts();

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
	
	
    switch (buffer[0]) {
      case 'd':                      // Command for reading encoder counts in terms of degrees.
      {
		int degrees = 0;
		degrees = count_to_deg(encoder_counts());
        sprintf(buffer,"%d\r\n", degrees); // return encoder counts to degrees
        NU32_WriteUART3(buffer);
        break;
      }
	  case 'x':
	  {
		  int num1=0;
		  int num2=0;
		  int sum=0;
		  NU32_ReadUART3(buffer, BUF_SIZE);
		  sscanf(buffer, "%d %d", &num1, &num2);
		  sum = num1 + num2;
		  sprintf(buffer, "%d\r\n", sum);
		  NU32_WriteUART3(buffer);
		  break;
		  
	  }
	  case 'c':
	  {
		  sprintf(buffer,"%d\r\n", encoder_counts());
		  NU32_WriteUART3(buffer); // send encoder count to client
		  break;
	  }
	  case 'e':
	  {
		  sprintf(buffer, "%d\r\n", encoder_reset());
		  NU32_WriteUART3(buffer);
		  break;
	  }
	  case 'a':
	  {
		  int ticks = read_ADC();
		  sprintf(buffer, "%d\r\n", ticks);
		  NU32_WriteUART3(buffer);		//Send ADC ticks to client
		break;
	  }
	  case 'b':
	  {
		  sprintf(buffer, "%d\r\n", ADC_ma(read_ADC()));
		  NU32_WriteUART3(buffer);		//Send ADC ticks to client
		break;
	  }	
	  case 'r':
	  {
		  sprintf(buffer, "%d\r\n", get_mode(&modevar));
		  NU32_WriteUART3(buffer);
		  
		  break;
	  }
	  case 'f':
	  {		  
		  NU32_ReadUART3(buffer, BUF_SIZE);
		  sscanf(buffer, "%d", &modevar.pwm);
		  set_modee(&modevar, PWM);
		  sprintf(buffer, "%d\r\n", modevar.pwm);
		  NU32_WriteUART3(buffer);
		  break;
	  }
	  case 'p':
	  {
		  set_modee(&modevar, IDLE);
		  break;
	  }
	  case 'g':			//Set Current Gains
	  {
		  CurrCtrl.kd = 0;
		  NU32_ReadUART3(buffer, BUF_SIZE);
		  sscanf(buffer, "%f %f", &CurrCtrl.kp, &CurrCtrl.ki);
		  sprintf(buffer, "%d\r\n", 1);
		  NU32_WriteUART3(buffer);
		  
		  break;
	  }
	  case 'h':			//Get Current Gains
	  {
		  sprintf(buffer, "%f %f\r\n", CurrCtrl.kp, CurrCtrl.ki);
		  NU32_WriteUART3(buffer);
		  
		  break;
	  }
	  case 'k':
	  {
		  int i;
		  CurrCtrl.eint = 0;
		  CurrCtrl.ref = ITEST_IREF;
		  CurrCtrl.int_min = PWM_MIN;
		  CurrCtrl.int_max = PWM_MAX; 
		  Itest_Data_f = 0;
		  set_modee(&modevar, ITEST);
		  while(!Itest_Data_f);
		  sprintf(buffer, "%d\r\n", ITEST_DATAPOINTS);
		  NU32_WriteUART3(buffer);
		  
		  for(i=0;i<ITEST_DATAPOINTS;i++) {
			  sprintf(buffer, "%d %d\r\n", Itest_ref[i], Itest_data_real[i]);
			  NU32_WriteUART3(buffer);
		  }
		  break;
	  }
	  case 'i':
	  {
		  NU32_ReadUART3(buffer, BUF_SIZE);
		  sscanf(buffer, "%f %f %f", &PosCtrl.kp, &PosCtrl.ki, &PosCtrl.kd);
		  sprintf(buffer, "%d\r\n", 1);
		  NU32_WriteUART3(buffer);
	  
		  break;
	  }
	  case 'j':
	  {
		  sprintf(buffer, "%f %f %f\r\n", PosCtrl.kp, PosCtrl.ki, PosCtrl.kd);
		  NU32_WriteUART3(buffer);
		  
		  break;
	  }
	  case 'l':
	  {
		  PosCtrl.eint = 0;
		  CurrCtrl.eint = 0;
		  PosCtrl.eprev = 0;
		  PosCtrl.int_min = IMIN;
		  PosCtrl.int_max = IMAX;
		  NU32_ReadUART3(buffer, BUF_SIZE);
		  sscanf(buffer, "%d", (int)&modevar.pos);
		  
		  set_modee(&modevar, HOLD);
		  
		  //while(!Hold_Data_f);
		  
		  break;
	  }
	  case 'm':
	  {
		  int m;
		  array_clr(Track_data_ref);
		  NU32_ReadUART3(buffer, BUF_SIZE);
          sscanf(buffer, "%d", &trajectory_size);
		  for (m = 0; m < trajectory_size; m++){
			NU32_ReadUART3(buffer, BUF_SIZE);
			sscanf(buffer, "%d", &Track_data_ref[m]);
          }
		  sprintf(buffer, "%d\r\n", 1);
          NU32_WriteUART3(buffer);
		  break;
	  }
	  case 'n':
	  {
		  int n;
		  array_clr(Track_data_ref);
		  NU32_ReadUART3(buffer, BUF_SIZE);
		  sscanf(buffer, "%d", &trajectory_size);
		  for (n = 0; n < trajectory_size; n++) {
			  NU32_ReadUART3(buffer, BUF_SIZE);
			  sscanf(buffer, "%d", &Track_data_ref[n]);
		  }
		  sprintf(buffer, "%d\r\n", 1);
		  NU32_WriteUART3(buffer);
		  
		  break;
	  }
	  case 'o':
	  {
		  int k;
		  CurrCtrl.eint = 0;
		  CurrCtrl.eprev = 0;
		  PosCtrl.eint = 0;
		  PosCtrl.eprev = 0;
		  Track_Data_f = 0;
		  set_modee(&modevar, TRACK);
		  
		  while(!Track_Data_f);
		  sprintf(buffer, "%d\r\n", trajectory_size);
		  NU32_WriteUART3(buffer);
		  for(k=0; k<trajectory_size; k++) {
			  sprintf(buffer, "%d %d\r\n", Track_data_ref[k], Track_data_real[k]);
			  NU32_WriteUART3(buffer);
		  }
		  break;
	  }
      case 'q':
      {
		  set_modee(&modevar, IDLE);
        // handle q for quit. Later you may want to return to IDLE mode here. 
		  break;
      }
	  case 't':
	  {
		  sprintf(buffer, "%d\r\n", (int) modevar.duty_p);
		  NU32_WriteUART3(buffer);		
		  break;
	  }
      default:
      {
		//modevar.mode = IDLE;
		NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
	
  }
  return 0;
}
