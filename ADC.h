#ifndef ADC__H__
#define ADC__H__

#include <xc.h>                     // processor SFR definitions
#include <sys/attribs.h>            // __ISR macro

#define		ADC_PIN		10
#define 	ADC_MA(x)		0.6893*(x-105) - 273
//#define 	ADC_MA(x)		0.8515*(x-150) - 290

void init_ADC(void);
unsigned int read_ADC(void);
int ADC_ma(unsigned int ticks);

#endif // ADC__H__
