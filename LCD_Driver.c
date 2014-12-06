#include "main.h"
#include "LCD_Driver.h"
#include "Graphics.h"

// Numbers correspond to CGRAM addresses
//{,0x01,0x02,0x03,0x04,0x05,0x06,0x07}
unsigned char gameGraphics[64];

// note: 0x10 is used as an empty symbol

unsigned char gameField[2][17] = { 
	{0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x01},
	{0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10}
};

void displayFrame(void) {
	
	int i,j,z;
	
	unsigned char current, prev;
	
	// Move Cursor from left to right
	lcdWriteCommand(0x06);
	
	// Write Game Graphics into CGRAM
	setCGRAMaddr(0);
	
	//getSinwave(80,0,8);
	
	for (i = 0; i < 64; i++) {
		//for (j = 0; j < 8; j++) {
			lcdWriteData(gameGraphics[i]);
	//	}
	}
	
	// Move Cursor from right to left
	lcdWriteCommand(0x06);
	
	lcdGoto(0, 0);
	lcdWriteData(0x00);
	lcdWriteData(0x01);
	lcdWriteData(0x02);
	lcdWriteData(0x03);
	lcdGoto(1,0);
	lcdWriteData(0x04);
	lcdWriteData(0x05);
	lcdWriteData(0x06);
	lcdWriteData(0x07);
	
		
}

/** Configure Ports **/
void portInit(void) {

	volatile unsigned long delay;
	// Enable clock on PortA, PortB, PortD, PortE, PortF
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOA + SYSCTL_RCGC2_GPIOB + SYSCTL_RCGC2_GPIOD + SYSCTL_RCGC2_GPIOE + SYSCTL_RCGC2_GPIOF;
	// delay   
	delay = SYSCTL_RCGC2_R;
	
	// Disable analog
	GPIO_PORTA_AMSEL_R &= 0x00;
	GPIO_PORTB_AMSEL_R &= 0x00;
	GPIO_PORTD_AMSEL_R &= 0x00;
	GPIO_PORTE_AMSEL_R &= 0x00;
	GPIO_PORTF_AMSEL_R &= 0x00;
	
	// Set GPIO
	GPIO_PORTA_PCTL_R &= 0x00;
	GPIO_PORTB_PCTL_R &= 0x00;
	GPIO_PORTD_PCTL_R &= 0x00;
	GPIO_PORTE_PCTL_R &= 0x00;
	GPIO_PORTF_PCTL_R &= 0x00;
	
	// Inputs or Outputs
	GPIO_PORTA_DIR_R = 0x1F; // PA7, PA6, PA5 are inputs
	GPIO_PORTB_DIR_R = 0x0F; //PB0, PB1 are outputs and PB4 is input
	GPIO_PORTD_DIR_R |= 0xFF; // All outputs
	GPIO_PORTE_DIR_R |= 0xFF; // All outputs
	GPIO_PORTF_DIR_R |= 0xFF; // All outputs
	
	// Clear alternative functions
	GPIO_PORTA_AFSEL_R &= 0x00;
	GPIO_PORTB_AFSEL_R &= 0x00;
	GPIO_PORTD_AFSEL_R &= 0x00;
	GPIO_PORTE_AFSEL_R &= 0x00;
	GPIO_PORTF_AFSEL_R &= 0x00;
	
	// Enable Digital
	GPIO_PORTA_DEN_R |= 0xFF;		
	GPIO_PORTB_DEN_R |= 0xFF;
	GPIO_PORTD_DEN_R |= 0xFF;
	GPIO_PORTE_DEN_R |= 0xFF;
	GPIO_PORTF_DEN_R |= 0xFF;
	
	// Disable pullup resistors
	GPIO_PORTA_PUR_R &= 0x00;
	GPIO_PORTB_PUR_R &= 0x00;
	GPIO_PORTD_PUR_R &= 0x00;
	GPIO_PORTE_PUR_R &= 0x00;
	GPIO_PORTF_PUR_R &= 0x00;
}

/** Enable pulse to latch the instruction **/
void lcdENPulse(void) {
	// Set Enable (PE2) to 1 
	LCD_ENABLE = 0x04;
	// Wait for 1ms
	//SysTick_Wait_ms(1);
	SysTick_Wait_us(250); // Force it a little bit
	
	// Set Enable (PE2) to 0
	LCD_ENABLE = 0x00;
	// Wait for 1ms
  SysTick_Wait_ms(1);
	//
	SysTick_Wait_us(250); // Force it a little bit
}

/** Initialise LCD **/
void lcdInit(void) {
	// Set Enable (PE2) pin to 0
	LCD_ENABLE = 0x00;	
	// Wait for 20ms
	SysTick_Wait_ms(20);
	// Initialisation instruction
	LCD_RW = 0x00; // (PE0)
	LCD_RS = 0x00; // (PE1)
	LCD_DB7_TO_DB4 = 0x03;
	// Latch Instruction (1)
	lcdENPulse();
	// Wait for 10ms
	SysTick_Wait_ms(10);
	// Latch Instruction (2)
	lcdENPulse();
	// Wait for 1ms
	SysTick_Wait_ms(1);
	// Latch Instruction (3)
	lcdENPulse();
	// Still Initialising
	LCD_DB7_TO_DB4 = 0x02;
	// Latch Instruction (4)
	lcdENPulse();
	// Settings instruction (4-bit, 2 lines, 5x8 dots)
	lcdWriteCommand(0x28);
	// Settings instruction (Display On, Cursor On, Blinking cursor On)
	lcdWriteCommand(0x0F);
	//lcdWriteCommand(0x0C); // No cursor	
	// Settings instruction (Cursor auto-increment, no shift)
	lcdWriteCommand(0x06);
}

/** Send instruction to the LCD Screen **/
void lcdWriteCommand(unsigned char c) {
	// RS (PE1) (0 to send an instruction, 1 to write data)
	LCD_RS = 0x00;
	// Give it some time (50 usec)
	SysTick_Wait_us(50);
	// Write high nibble first
	LCD_DB7_TO_DB4 = (c & 0xF0) >> 4;
	// Latch it
	lcdENPulse();
	// Delay (5ms)
	// SysTick_Wait_ms(5);	
	SysTick_Wait_us(100); // Force it a little bit
	// Write low nibble
	LCD_DB7_TO_DB4 = (c & 0x0F);
	// Latch it
	lcdENPulse();
	// Delay (5ms)
	// SysTick_Wait_ms(5);	
	SysTick_Wait_us(100); // Force it a little bit
}

/** Send data to be displayed on the LCD **/
void lcdWriteData(char c) {
	// RS (PE1) (0 to send an instruction, 1 to write data)
	LCD_RS = 0x02;
	// Give it some time (50 usec)
	SysTick_Wait_us(50);
	// Write high nibble first
	LCD_DB7_TO_DB4 = (c & 0xF0) >> 4;
	// Latch it
	lcdENPulse();
	// Delay (200us)
	SysTick_Wait_us(200);
	// Write low nibble
	LCD_DB7_TO_DB4 = (c & 0x0F);
	// Latch it
	lcdENPulse();
	// Delay (200us)
	// SysTick_Wait_us(200);
  // SysTick_Wait_us(100); // Force it a little bit	
}

/** Write string of characters on the LCD screen **/
void lcdWriteRamString(char *string) {
	while(*string) {
		if (*string != 7) lcdWriteData(*string);
		string++;
	}
}

/** Clear display and return cursor to the home position **/
void lcdClearScreen(void) {
	// Clear Display instruction
	lcdWriteCommand(0x01);
}

/** Move the position of the cursor to a specified DDRAM address **/
void lcdGoto(unsigned char row, unsigned char column) {
	//lcdWriteCommand(0x80 + address);
	
	if (row == 0) {
		lcdWriteCommand(0x80 + column);
	} else {
		lcdWriteCommand(0xC0 + column);
	}
	
	
}

/** Set CGRAM address **/
void setCGRAMaddr(unsigned char address) {
	lcdWriteCommand(0x40 + address);
}


/*
int main(void) {
	// Port Init
 	portInit();
	// PLL Init
	PLL_Init();
	//SysTick Init
	SysTick_Init();
	// LCD Init
	lcdInit();
	// Clear Screen
	lcdClearScreen();
	// Goto the first address in memory
	//lcdGoto(2,1);
	// Write a test string
  //lcdWriteRamString("Hello, world!");
	
	displayFrame();

	//
	//lcdGoto(0x00);
	//
	//lcdWriteData(2);
	//lcdWriteData(1);
	
	
	// Stop here
	while(1) {
		
			
			//GPIO_PORTF_DATA_R = 4;
			//SysTick_Wait_ms(1000);
			//GPIO_PORTF_DATA_R = 0;
			//SysTick_Wait_us(1000000);
			
			//displayFrame();
		
			//lcdClearScreen();
	
			//displayFrame();
		
	}
}
*/
