#ifndef ENCODER__H__
#define ENCODER__H__

#include <xc.h>                     // processor SFR definitions
#include <sys/attribs.h>            // __ISR macro
#include <stdio.h>
#include <stdint.h>

#define ENCODER_ZERO 32768
#define COUNT_TO_DEG 360 / 1024


int encoder_counts(void);
void encoder_init(void);
int encoder_reset(void);
int count_to_deg(int counts);
#endif // ENCODER__H__
