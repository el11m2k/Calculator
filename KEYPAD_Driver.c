//////////////////////////////////////////////////////////
//   KEYPAD Driver
//////////////////////////////////////////////////////////

// Pre-processor Directives Section
#include "main.h"
#include "PLL.h"
#include "KEYPAD_Driver.h"
#include "tm4c123gh6pm.h"
#include "string.h"
#include "Speaker.h"

char currentState = 0x00;						// variable to store current keypad state
char  prevState = 0x00;							// variable to store previous keypad state

// Reads every row one by one and checks whether any column has current flowing through it. If it reads zero, it then means that either nothing is pressed or
// multiple columns are shorted. In this case it will then re-configure the I/O and swap Rows with Columns and do the same kind of "sweep" as before.
// Inputs: boolean type multikey - used as a flag to detect multiple keys or single keys
// Outputs: button value coded in 32-bit number (unsigned long)
unsigned char readKeypad(){
	int i = 0;																							// itterator for the FOR loop
	unsigned char keypadReadIn = 0x00;  										// input variable into which keys are read
	
	//Horizontal Scanning Starts here!
	for(i=0;i<4;i++){

		enableRow(i);																					//enable appropriate row
			
		// Read the values into keypadReadIn variable. We read PA5 PA6 PA7 and PB4, and combine it all into one variable.
		// Notice the inversion of port data - that's because we are using internal pull-up resistors of the microcontroller.
		keypadReadIn = ((GPIOB->DATA)&0x0F);
		// If not zero, therefore button was pressed, combine the data read which represents row information (e.g. 1010 would mean buttons 1 and 3 are pressed)
		if(keypadReadIn){
			keypadReadIn |= (i+1) << 4;														// add the incremented (for ease of read) i value to the whole result (e.g. 0x08-->0xi8, where i is a value from 1 to 4)
			break;															
		}
	}
	return keypadReadIn;
}

// Decodes the button value obtained from readKeypad() function.
// Input: button value in the form of OxMN, where M - column(1,2,3,5) and N - row(1,2,3,5) 
// Unsinged Long datatype instead of char because we can detect multiple keys and 256 values are not enough
// Output char of ASCII representation of the keypad value (e.g. 1/2/3/A/4/5/6/B and etc.)
char decodeKeyPress(unsigned char k){
		switch(k){ //switch for multi-key combos		
			case 0x18:
				return '1';
			
			case 0x14:
				return '2';

			case 0x12:
				return '3';
			
			case 0x11:
				return 'A';
			
			case 0x28:
				return '4';
			
			case 0x24:
				return '5';

			case 0x22:
				return '6';
			
			case 0x21:
				return 'B';
				
			case 0x38: // Was F8?
				return '7';
				
			case 0x34:
				return '8';
		
			case 0x32:
				return '9';		
			
			case 0x31:
				return 'C';	
			
			case 0x44:
				return '0';
		
			case 0x42:
				return '#';
				
			//case 0x41:
			//	return 'D';
			
			default:
				return 0; 		// in case there was a mismatch and no key was recognised properly, return 0
		}
}
// Interprets the key array to check for combo keys/ single presses
// Input : array of keys
// Output: single key found
unsigned char readKey(void){
	char returnValue = 0;																		// value to return
	
	currentState = readKeypad();														// read current keypad state		
	if(!currentState){
		prevState = 0;
		return 0;
	}
	else{
	if(prevState == 0x48){	
		switch(currentState){ // check what was the second key
			case 0x11:	
				returnValue = 104;
			break;
			case 0x21:	
				returnValue = 105;
			break;
			case 0x31:	
				returnValue = 106;
			break;
			case 0x12:
				returnValue = 108;
			break;
			case 0x22:
				returnValue = 109;
			break;
			case 0x32:
				returnValue = 110;
			break;
			case 0x14:
				returnValue = 111;
			break;
			case 0x24:
				returnValue = 112;
			break;
			case 0x34:
				returnValue = 113;
			break;
			
			default:
				returnValue = 0;
			break;
		}
	} else if(prevState == 0x41){	
		switch(currentState){	// check what was the second key
			case 0x12:
				returnValue = 97;
			break;
			case 0x22:
				returnValue = 98;
			break;
			case 0x14:
				returnValue = 99;
			break;
			case 0x24:
				returnValue = 100;
			break;
			case 0x34:
				returnValue = 101;
			break;
			case 0x28:
				returnValue = 102;
			break;
			case 0x18:
				returnValue = 103;
			break;
			case 0x38:
				returnValue = 107;
			break;
			case 0x32:
				returnValue = 114;
			break;
			
			
			default:
				returnValue = 0;
			break;
		}
	} else if(currentState == prevState){
		returnValue = 0;
	}
	else if(currentState && currentState!=prevState){
		returnValue = decodeKeyPress(currentState);
	}

	prevState = currentState;												// reset the previous state
	return returnValue;
}
}
		


//Initialises the keypad
//Input : N - rows, M- columns. Supports up to 255x255 keypad size.
void keypadInit(unsigned char N,unsigned char M){ 
	//4x4 Matrix Keypad
	if((N==4) && (M==4)){
		volatile unsigned long delay;
		SYSCTL->RCGC2 |= SYSCTL_RCGC2_GPIOB;					//enable clock for GPIOB
		delay = SYSCTL->RCGC2;          							// dummy delay   
		
		// AMSEL (disable analog function)
		GPIOB->AMSEL = 0x00;													// PortB
		
		// PCTL (GPIO clear bit PCTL)
		GPIOB->PCTL = 0x00;														// PortB
		
		// I/O (set inputs and outputs)
		GPIOB->DIR &=~((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// clear bits for PB0-3 to make them inputs 
		GPIOB->DIR |= ((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));		// set bits PB4-7 to make them outputs 
		
		// AFSEL (no alternate function)
		GPIOB->AFSEL = 0x00;													// PortB
		
		// PUR (enable internal pull-up resistors)
		GPIOB->PDR |= ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// PB0-3
		
		// DEN (enable digital pins)
		GPIOB->DEN |= ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3)|(1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));		// PB0-7

		// Set-up PortB interrupts
		GPIOB->IS &=~  ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// PB0-3 edge-sensitive
		GPIOB->IBE |= ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// PB0-3 not both edges
		//GPIOB->IEV |= ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// PB0-3 falling edge event
		GPIOB->ICR =   ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// PB0-3 clear flag4
		GPIOB->IM |=   ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// PB0-3 arm interrupt on PB4
		
		__set_BASEPRI(1);															// non-zero, hence ignore interrupts with lower priority
		
		//Set interrupt priority to 5 (any arbitrary number [0-7] is ok). Value has to be higher than SysTick (so SysTick has higher priority)
		NVIC_SetPriority(GPIOB_IRQn,5);								// PortB
		
		// Enable interrupts
		NVIC_EnableIRQ(GPIOB_IRQn);										// PortB
		__enable_irq();                 							// enable the global interrupt flag
		
	}
}


// This handler is called when PortB interrupt is detected on PB4
// Input: None
// Output: None
void GPIOB_Handler(void){
	SysTick_Wait_ms(10);																		// de-bounce
	key = 1;																									// flag the "key" variable (any value would do, as long it's not zero)
	GPIOB->ICR = ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));      	// acknowledge flags for PB0-3
}


// Flips the keypad Inputs and Outputs (used in multi-key detection)
// Input: N - rows, M - columns, orientation - either 'R' for ROWS (horizontal) or 'C' for COLUMNS (vertical) 
// Output: None
// Supports up to 255x255 keypad size.
void keypadFlip(unsigned char N,unsigned char M, char orientation){ 
	//4x4 Matrix Keypad
	if((N==4) && (M==4)){
		if(orientation == 'R'){

			// I/O directions
			GPIOB->DIR &=~((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// clear bits for PB0-3 to make them inputs 
			GPIOB->DIR |= ((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));		// set bits PB4-7 to make them outputs 
			// Pull-ups
			GPIOB->PDR |= ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// PB0-3 enable purs
			GPIOB->PDR &=~((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));		// PB4-7 disable purs
			
			
		}	
		if(orientation == 'C'){

			GPIOB->DIR |=  ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// set bits for PB0-3 to make them outputs 
			GPIOB->DIR &=~ ((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));		// clear bits PB4-7 to make them inputs 
			// Pull-ups
			GPIOB->PDR &=~ ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));		// PB0-3 disable purs
			GPIOB->PDR |=  ((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));		// PB4-7 enable purs

		}
	}
}

// Waits for user input from keyboard by putting processor to sleep (efficient way to detect user input)
// Inputs: None
// Outputs: None
// Once key is pressed it is written to the global variable "key", but before it is re-set down below for every read
void WaitForKey(void){
	key = 0; 																								// re-set the global flag value
	GPIOB->DATA |= ((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));		// clear PB0,PB1 so they are ready to take inputs (Inverted logic due to internal PULL-UPs)
 	__wfi();  																							// wait for interrupt
}

// Waits for user input from keyboard but does not put processor to sleep (less efficient but allows to update the LCD continuously)
// Inputs: None
// Outputs: None
// Once key is pressed it is written to the global variable "key", but before it is re-set down below for every read
void WaitForKeySoft(void){
	//key = 0; 																								// read the key value
	GPIOB->DATA |= ((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));		// clear PB0,PB1 so they are ready to take inputs (Inverted logic due to internal PULL-UPs)
}

// Sets a given row high
// Input: i - tells which row should be set active
// Ouptu: None
void enableRow(int i){
	GPIOB->DATA &=~ ((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));	// Reset the inputs
	GPIOB->DATA |= (1UL<<(4+i)); 														// Activate a row
}

// Sets a given column high
// Input: i - tells which column should be set active
// Ouptu: None
void enableColumn(int i){
	GPIOB->DATA &=~ ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));	// Reset the inputs
	GPIOB->DATA |= (1UL<<(3-i));														// activate the row
}
