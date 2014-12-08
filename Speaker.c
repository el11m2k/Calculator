#include "main.h"
#include "Speaker.h"
#include "PLL.h"

// Initialises PortA, which is used for Speaker
void speakerInit(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x01;     
	delay = SYSCTL_RCGC2_R;
  GPIO_PORTA_DIR_R = 0x10;  	// PA4 speaker pin, set as output
  GPIO_PORTA_AFSEL_R = 0x00;
	GPIO_PORTA_AMSEL_R = 0x00;
	GPIO_PORTA_DEN_R = 0x10;		// Enable digital pin (PA4)
	GPIO_PORTA_PCTL_R = 0x00; 		
}	

// Function to produce a low pitch sound for "equals" key button press
// Input: none
// Output: none
void beepANS(void){
	int i=0;													 // Value used for itterating through a loop to produce a pulse train
	for ( i = 0; i <= 10; i++) {			
			GPIO_PORTA_DATA_R |=0x10;      // Set PA4 high
			SysTick_Wait_ms(4);            // Wait a given time
			GPIO_PORTA_DATA_R &=~(0x10);   //	Set PA4 low
			SysTick_Wait_ms(4);            // Wait a given time
	}
}
 
// Function to produce a high pitch sound for regular keypad button press
// Input: none
// Output: none
void beepINS(void){
	int i=0;														  // Value used for itterating through a loop to produce a pulse train
	for ( i = 0; i <= 25; i++) {            
			GPIO_PORTA_DATA_R |=0x10;         // Set PA4 high
			SysTick_Wait_us(700);             // Wait a given time
			GPIO_PORTA_DATA_R &=~(0x10);      // Set PA4 low
			SysTick_Wait_us(700);             // Wait a given time  
	}
}