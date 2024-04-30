#include "./led/bsp_led.h"   
#include "./beep/bsp_beep.h"
#include "stm32f4xx.h"
#include "./dwt_delay/core_delay.h"   



void isRight(){
		
		LED_GREEN
		 Delay_ms(1000000);
		LED_RGBOFF
}


void isWrong(){
		
		LED_RED
		Delay_ms(1000000);
		LED_RGBOFF
		
		BEEP_TOGGLE;
		Delay_ms(1000000);
	
}

