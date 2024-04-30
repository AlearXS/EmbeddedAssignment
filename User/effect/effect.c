#include "./led/bsp_led.h"   
#include "./beep/bsp_beep.h"
#include "stm32f4xx.h"
#include "./dwt_delay/core_delay.h"   
#include <stdio.h>



void isRight(){
		LED_GREEN
		Delay_ms(500);
		LED_RGBOFF
}


void isWrong(){
		LED_RED
		BEEP_ON;
		Delay_ms(500);
		LED_RGBOFF
		BEEP_OFF
}

