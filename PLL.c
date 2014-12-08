#include "main.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"

// Initialise PLL 
// Inputs: None
// Outputs: None
void PLL_Init() {
	// 0) Use RCC2
  SYSCTL_RCC2_R |=  0x80000000;  // USERCC2
  // 1) bypass PLL while initializing
  SYSCTL_RCC2_R |=  0x00000800;  // BYPASS2, PLL bypass
  // 2) select the crystal value and oscillator source
  SYSCTL_RCC_R = (SYSCTL_RCC_R &~0x000007C0)   // clear XTAL field, bits 10-6
                 + 0x00000540;   // 10101, configure for 16 MHz crystal
  SYSCTL_RCC2_R &= ~0x00000070;  // Configure for main oscillator source
  // 3) activate PLL by clearing PWRDN
  SYSCTL_RCC2_R &= ~0x00002000;
  // 4) set the desired system divider
  SYSCTL_RCC2_R |= 0x40000000;   // use 400 MHz PLL
  SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~ 0x1FC00000)  // clear system clock divider
                  + (39 << 22);      // configure for 10 MHz clock 	
  // 5) wait for the PLL to lock by polling PLLLRIS
  while((SYSCTL_RIS_R & 0x00000040)==0){};  // wait for PLLRIS bit
  // 6) enable use of PLL by clearing BYPASS
  SYSCTL_RCC2_R &= ~0x00000800;
}

// Initialise SysTick 
// Inputs: None
// Outputs: None
void SysTick_Init() {
	NVIC_ST_CTRL_R = 0;            // disable SysTick during setup
	NVIC_ST_RELOAD_R = 0x00FFFFFF; // maximum reload value (24bits)
	NVIC_ST_CURRENT_R = 0;         // Clear CURRENT counter value             
	NVIC_ST_CTRL_R = 0x00000005;   // enable SysTick with core clock
	NVIC_SetPriority(SysTick_IRQn, 0);
}

// Wait for microseconds
// Inputs: time in us
// Outputs: None
// Maximum delay is 1677721 us
void SysTick_Wait_us(unsigned int delay_us) {
	NVIC_ST_RELOAD_R = delay_us * 10;									// number of counts to wait, 10 ticks = 1 us (at 10Mhz)
  NVIC_ST_CURRENT_R = 0;  													// Clear CURRENT counter value
  while((NVIC_ST_CTRL_R & 0x00010000)==0){} 				// wait for count flag
}

// Wait for milliseconds
// Inputs: time in ms
// Outputs: None
// Maximum delay is 1677 ms
void SysTick_Wait_ms(unsigned int delay_ms) {
	NVIC_ST_RELOAD_R = delay_ms * 10000;  			// number of counts to wait, 10000 ticks = 1 ms (at 10Mhz)
  NVIC_ST_CURRENT_R = 0;  										// Clear CURRENT counter value
  while((NVIC_ST_CTRL_R & 0x00010000)==0){} 	// wait for count flag
}