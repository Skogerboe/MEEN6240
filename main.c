#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "utilities.h"
#include "ADC.h"
#include "encoder.h"
// include other header files here

#define BUF_SIZE 200


modevars modevar;

/*void __ISR(_TIMER_2_VECTOR, IPL5SOFT) TIMER2ISR(void) {
	//IFS0CLR = (1<<8);
	IFS0CLR = (1<<6);
	TMR2 = 0;
}*/

void __ISR(_OUTPUT_COMPARE_1_VECTOR, IPL5SOFT) OC1ISR(void) {
	IFS0CLR = (1<<5);
	
}

void __ISR(_TIMER_4_VECTOR, IPL5SOFT) TIMER4ISR(void) {
	IFS0CLR = (1<<16);
	OC1RS = set_mode(&modevar);//(int) modevar.duty_p;	
	TMR4 = 0;
}

int main() 
{
  char buffer[BUF_SIZE];
  int i;
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
		  modevar.mode = PWM;
		  sprintf(buffer, "%d\r\n", modevar.pwm);
		  NU32_WriteUART3(buffer);
		  break;
	  }
	  case 'p':
	  {
		  modevar.mode = IDLE;
		  
		  break;
	  }
	  case 'g':			//Set Current Gains
	  {
		  //modevar.mode =
		  CurrCtrl.kd = 0;
		  NU32_ReadUART3(buffer, BUF_SIZE);
		  sscanf(buffer, "%f %f", &CurrCtrl.kp, &CurrCtrl.ki);
		  sprintf(buffer, "%d\r\n", 1);
		  NU32_WriteUART3(buffer);
		  
		  break;
	  }
	  case 'h':			//Get Current Gains
	  {
		  //modevar.mode = ITEST;
		  sprintf(buffer, "%f %f\r\n", CurrCtrl.kp, CurrCtrl.ki);
		  NU32_WriteUART3(buffer);
		  
		  break;
	  }
	  case 'k':
	  {
		  
		  int i;
		  CurrCtrl.int_min = -100;
		  CurrCtrl.int_max = 100; 
		  modevar.mode = ITEST;
		  //sprintf(buffer, "%d\r\n", 100);
		  //NU32_WriteUART3(buffer);
		  break;
	  }
      case 'q':
      {
		  modevar.mode = IDLE;
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
	//modevar.duty_p = set_mode(&modevar);
	/*if (Itest_Data_f == 1){
		  for(i = 0; i < 100; i++) {
	  		  sprintf(buffer, "%d %d\r\n", Itest_ref[i], Itest_data_real[i]);
			  NU32_WriteUART3(buffer);	
		  }
		  Itest_Data_f = 0;  
	}*/
	
  }
  return 0;
}
