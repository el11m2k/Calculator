//////////////////////////////////////////////////////////
//   KEYPAD Driver
//////////////////////////////////////////////////////////

// Pre-processor Directives Section
#include "main.h"
#include "KEYPAD_Driver.h"
#include "tm4c123gh6pm.h"
#include "string.h"

unsigned long keypadKeysTotal = 0x00000001; //reset for scanning. Button data is accumulated in this variable
int n = 0;
unsigned char keys[4] = {0};
char currentState = 0x00;
char  prevState = 0x00;

// Reads every row one by one and checks whether any column has current flowing through it. If it reads zero, it then means that either nothing is pressed or
// multiple columns are shorted. In this case it will then re-configure the I/O and swap Rows with Columns and do the same kind of "sweep" as before.
// Inputs: boolean type multikey - used as a flag to detect multiple keys or single keys
// Outputs: button value coded in 32-bit number (unsigned long)
unsigned char readKeypad(char prevStaten){
	int i = 0;
	unsigned char keypadReadIn = 0x00;  														// input variable into which keys are read
	keypadKeysTotal = 0x00; 													// reset for scanning. Button data is accumulated in this variable
	
	//Horizontal Scanning Starts here!
	for(i=0;i<4;i++){

		enableRow(i);																					//enable appropriate row
			
		// Read the values into keypadReadIn variable. We read PA5 PA6 PA7 and PB4, and combine it all into one variable.
		// Notice the inversion of port data - that's because we are using internal pull-up resistors of the microcontroller.
		keypadReadIn = ((GPIOB->DATA)&0x0F);
		// If not zero, therefore button was pressed, combine the data read which represents row information (e.g. 1010 would mean buttons 1 and 3 are pressed)
		if(keypadReadIn){
			keypadReadIn |= (i+1) << 4;														// add the incremented (for ease of read) i value to the whole result (e.g. 0x08-->0xi8, where i is a value from 1 to 4)
			//keypadKeysTotal |= keypadReadIn<<(i*8); 							// add this new reading to the total variable where we stack information for multiple press recognition. Shift it to make space for other readings.
			break;
			//if(!multikey) break;																	// if user does not wish to detect multiple keys, the first match will be sufficient enough, hence no further execution is needed
		}
	}
	// keypadKeysTotal is zero, therefore we had a short in columns and need to re-configure.
	// If user asked to detect multiple-keys, code below will re-configure the I/O
	// and then re-scan again to detect multiple presses
	if(!keypadReadIn&&FALSE){
		//Vertical Scanning Starts here!	
		keypadFlip(4,4,'C'); 																		// flip rows and column (change I/O-related register values)
		for(i=0;i<4;i++){
			enableColumn(i); 																			// enable appropriate column
			
			//read flipped data from PB0 PB1 PB4 PB5 (PE is shifted by 2 to have a consistent result of 0000 XXXX instead of 00XX 00XX)
			keypadReadIn = (GPIOB->DATA&0xF0)>>4;
			
			if(keypadReadIn){																			// if multiple keys on the same column were pressed..
				keypadReadIn |= (i+5) << 4;													// add 5 in order to distinguish between values scanned horizontally
				break;
				//keypadKeysTotal |= keypadReadIn;										// accumulate results
			}
		}
		keypadFlip(4,4,'R');  																	// flip rows and column back to defaults (change I/O-related register values)

	}		
	return keypadReadIn;
	//return keypadKeysTotal;										// return the final result
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
				
			case 0x38:
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
				
			case 0x41:
				return 'D';
			
			default:
				return 0; 		// in case there was a mismatch and no key was recognised properly, return 0
		}
}

unsigned char readKey(void){
	char localKeys[4] = {0};												// local array for returning
	
	SysTick_Wait_ms(40);													// de-bounce
	currentState = readKeypad(prevState);
	if (currentState && (currentState != prevState)){
		prevState = currentState;
		keys[n] = currentState;
		n++;
	}
	else if(!(currentState)){
		prevState = 0x00;
		n = 0;
		memcpy(&localKeys,&keys,sizeof(keys));			// make a copy of "keys"
		memset(&keys[0], 0, sizeof(keys));					// set whole "keys" array to 0
		return interpretKeys(localKeys);
	}
	return 0;	// left for good programming practise
}
	
unsigned char interpretKeys(unsigned char* keyar){
	if(keyar[1] == 0){													// one key pressed only return it
		return decodeKeyPress(keyar[0]);
	}
	else if(keyar[0] == 0x48){	//if star was pressed first it acts as 2nd function key
		switch(keyar[1]){
			case 0x11:
				return '(';
			case 0x21:
				return ')';
			case 0x31:
				return '.';
			case 0x49:
				return '^';
		}
	}
	else{
		return 0;	//	no defined combination found
	}
	return 0;	//	no defined combination found
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
		
		__set_BASEPRI(0);															// non-zero, hence ignore interrupts with lower priority
		
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
	//SysTick_Wait_ms(10);	
	// de-bounce
	key = 1;																	// flag the "key" variable (any value would do, as long it's not zero)
	GPIOB->ICR = ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));      // acknowledge flags for PB0-3
	//GPIOB->DATA=GPIOB->RIS;
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
	key = 0; 																				// re-set the global flag value
	GPIOB->DATA |= ((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7)); 						// clear PB0,PB1 so they are ready to take inputs (Inverted logic due to internal PULL-UPs)
 	__wfi();  																			// wait for interrupt
}

// Sets a given row high
// Input: i - tells which row should be set active
// Ouptu: None
void enableRow(int i){
	GPIOB->DATA &=~ ((1UL<<4)|(1UL<<5)|(1UL<<6)|(1UL<<7));
	GPIOB->DATA |= (1UL<<(4+i)); 
}

// Sets a given column high
// Input: i - tells which column should be set active
// Ouptu: None
void enableColumn(int i){
	GPIOB->DATA &=~ ((1UL<<0)|(1UL<<1)|(1UL<<2)|(1UL<<3));
	GPIOB->DATA |= (1UL<<(3-i));
}
