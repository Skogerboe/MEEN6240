#include "ADC.h"                   
#include <xc.h>

void init_ADC(void) {
	
	AD1PCFGbits.PCFG10 = 0;
	TRISB |= (1<<ADC_PIN);
	
	AD1CON3bits.ADCS = 3;
	AD1CON1bits.SSRC = 7; 									  
	AD1CON1bits.ADON = 1;                   // turn on A/D converter
}

unsigned int read_ADC(void) {
	int i, ticks_old;
	unsigned int ticks_avg = 0;
	
	AD1CHSbits.CH0SA = ADC_PIN;
	
	for(i=0;i<32;i++) {
		AD1CON1bits.SAMP = 1; 
		if(ADC1BUF0 <= (0.5*ticks_old)) {
			ticks_avg += ticks_old;
		}else {
			ticks_old = ADC1BUF0;
			ticks_avg += ADC1BUF0;
		}
	}
	
	return (ticks_avg)>>5;		//Divide ticks_avg sum of 4 samples by 4 to get an average count
}

int ADC_ma(unsigned int ticks) {
	
	return (int)(ADC_MA((int)ticks));	//Line equation determined in matlab.
	
}
