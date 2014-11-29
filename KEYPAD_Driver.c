//////////////////////////////////////////////////////////
//   KEYPAD Driver
//////////////////////////////////////////////////////////

// Pre-processor Directives Section
#include "main.h"
#include "KEYPAD_Driver.h"
#include "tm4c123gh6pm.h"

unsigned long keypadKeysTotal = 0x00000000; //reset for scanning. Button data is accumulated in this variable

// Reads every row one by one and checks whether any column has current flowing through it. If it reads zero, it then means that either nothing is pressed or
// multiple columns are shorted. In this case it will then re-configure the I/O and swap Rows with Columns and do the same kind of "sweep" as before.
// Inputs: boolean type multikey - used as a flag to detect multiple keys or single keys
// Outputs: button value coded in 32-bit number (unsigned long)
unsigned long readKeypad(char multikey){
	int i = 0;
	unsigned long keypadReadIn;  														// input variable into which keys are read
	keypadKeysTotal = 0x00000000; 													// reset for scanning. Button data is accumulated in this variable
	
	//Horizontal Scanning Starts here!
	for(i=0;i<4;i++){
		enableRow(i);																					//enable appropriate row
			
		// Read the values into keypadReadIn variable. We read PA5 PA6 PA7 and PB4, and combine it all into one variable.
		// Notice the inversion of port data - that's because we are using internal pull-up resistors of the microcontroller.
		keypadReadIn = ((~GPIO_PORTA_DATA_R)&0xE0)|((~GPIO_PORTB_DATA_R)&0x10);
			  
		// If not zero, therefore button was pressed, combine the data read which represents row information (e.g. 1010 would mean buttons 1 and 3 are pressed)
		if(keypadReadIn){
			keypadReadIn >>= 4; 																	// shift by one nibble-worth (one word) to make space for row data (e.g. 0x80-->0x08, 0x40-->0x04)
			keypadReadIn |= (i+1) << 4;														// add the incremented (for ease of read) i value to the whole result (e.g. 0x08-->0xi8, where i is a value from 1 to 4)
			keypadKeysTotal |= keypadReadIn<<(i*8); 							// add this new reading to the total variable where we stack information for multiple press recognition. Shift it to make space for other readings.
			if(!multikey){
				SysTick_Wait_ms(50);																// de-bounce
				break;							// if user does not wish to detect multiple keys, the first match will be sufficient enough, hence no further execution is needed
			}
		}
	}
			
	// keypadKeysTotal is zero, therefore we either had a short in columns and need to re-configure.
	// If user asked to detect multiple-keys, code below will re-configure the I/O
	// and then re-scan again to detect multiple presses
	if(!keypadKeysTotal && multikey){

		//Vertical Scanning Starts here!	
		keypadFlip(4,4,'C'); 																		// flip rows and column (change I/O-related register values)
		for(i=0;i<4;i++){
			enableColumn(i); 																			// enable appropriate column
			
			//read flipped data from PB0 PB1 PB4 PB5 (PE is shifted by 2 to have a consistent result of 0000 XXXX instead of 00XX 00XX)
			keypadReadIn = (((~GPIO_PORTE_DATA_R)&0x30)>>2)|((~GPIO_PORTB_DATA_R)&0x03);
			
			if(keypadReadIn){																			// if multiple keys on the same column were pressed..
				keypadReadIn |= (i+5) << 4;													// add 5 in order to distinguish between values scanned horizontally
				keypadKeysTotal |= keypadReadIn;										// accumulate results
			}
		}
		keypadFlip(4,4,'R');  																	// flip rows and column back to defaults (change I/O-related register values)
	}		
	SysTick_Wait_ms(50);																			// de-bounce
	return (unsigned long)keypadKeysTotal;										// return the final result
}

// Decodes the button value obtained from readKeypad() function.
// Input: button value in the form of OxMN, where M - column(1,2,3,5) and N - row(1,2,3,5) 
// Unsinged Long datatype instead of char because we can detect multiple keys and 256 values are not enough
// Output char of ASCII representation of the keypad value (e.g. 1/2/3/A/4/5/6/B and etc.)
char decodeKeyPress(unsigned long k){
	
	switch (k){
		case 0x18:
			return '1';
		
		case 0x14:
			return '2';

		case 0x12:
			return '3';
		
		case 0x11:
			return 'A';
		
		case 0x2800:
			return '4';
		
		case 0x2400:
			return '5';

		case 0x2200:
			return '6';
		
		case 0x2100:
			return 'B';
			
		case 0x380000:
			return '7';
			
		case 0x340000:
			return '8';
	
		case 0x320000:
			return '9';		
		
		case 0x310000:
			return 'C';	
		
		case 0x48000000:
			return '*';	
		
		case 0x44000000:
			return '0';
	
		case 0x42000000:
			return '#';
			
		case 0x41000000:
			return 'D';
		
		default:
			return 0; 		// in case there was a mismatch and no key was recognised properly, return 0
	}
}

//Initialises the keypad
//Input : N - rows, M- columns. Supports up to 255x255 keypad size.
void keypadInit(unsigned char N,unsigned char M){ 
	//4x4 Matrix Keypad
	if((N==4) && (M==4)){
		volatile unsigned long delay;
		SYSCTL->RCGC2 |= SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOB | SYSCTL_RCGC2_GPIOE; //enable clocks for GPIOA/GPIOB/GPIOE
		delay = SYSCTL->RCGC2;          							// dummy delay   
		
		// AMSEL (disable analog function)
		GPIOA->AMSEL = 0x00;													// PortA
		GPIOB->AMSEL = 0x00;													// PortB
		GPIOE->AMSEL = 0x00;													// PortE
		
		// PCTL (GPIO clear bit PCTL)
		GPIOA->PCTL = 0x00;														// PortA
		GPIOB->PCTL = 0x00;														// PortB
		GPIOE->PCTL = 0x00;														// PortE
		
		// I/O (set inputs and outputs)
		GPIOA->DIR &=~((1UL<<5)|(1UL<<6)|(1UL<<7));		// clear bits for PA5 PA6 PA7 to make them inputs 
		GPIOB->DIR &=~(1UL<<4);												// clear bit for PB4 to make it an input
		GPIOB->DIR |= ((1UL<<0)|(1UL<<1));						// set bits PB0 and PB1 to be outputs
		GPIOE->DIR |= ((1UL<<4)|(1UL<<5));						// set bits PE4 and PE5 to be outputs
		
		// AFSEL (no alternate function)
		GPIOA->AFSEL = 0x00;													// PortA
		GPIOB->AFSEL = 0x00;													// PortB
		GPIOE->AFSEL = 0x00;													// PortE
		
		// PUR (enable internal pull-up resistors)
		GPIOA->PUR |= ((1UL<<5)|(1UL<<6)|(1UL<<7));		// PA5 PA6 PA7
		GPIOB->PUR |= (1UL<<4);												// PB4
		
		// DEN (enable digital pins)
		GPIOA->DEN |= ((1UL<<5)|(1UL<<6)|(1UL<<7));		// PA5 PA6 PA7
		GPIOB->DEN |= ((1UL<<0)|(1UL<<1)|(1UL<<4));		// PB0 PB1 PB4
		GPIOE->DEN |= ((1UL<<4)|(1UL<<5));						// PE4 PE5
		
		// Set-up PortA interrupts
		GPIOA->IS  &=~((1UL<<5)|(1UL<<6)|(1UL<<7));		// PA5-7 is edge-sensitive
		GPIOA->IBE &=~((1UL<<5)|(1UL<<6)|(1UL<<7));		// PA5-7 is not both edges
		GPIOA->IEV &=~((1UL<<5)|(1UL<<6)|(1UL<<7));		// PA5-7 falling edge event (due to PULL-UPs & wanting to detect press event)
		GPIOA->ICR  = ((1UL<<5)|(1UL<<6)|(1UL<<7));		// clear flag5-7
		GPIOA->IM  |= ((1UL<<5)|(1UL<<6)|(1UL<<7));		// arm interrupt on PA5-7
		
		
		// Set-up PortB interrupts
		GPIOB->IS &=~ (1UL<<4);												// PB4 is edge-sensitive
		GPIOB->IBE &=~ (1UL<<4);											// PB4 is not both edges
		GPIOB->IEV &=~ (1UL<<4);											// PB4 falling edge event (due to PULL-UPs & wanting to detect press event)
		GPIOB->ICR = (1UL<<4);												// clear flag4
		GPIOB->IM |= (1UL<<4);												// arm interrupt on PF4
		__set_BASEPRI(1);
		//__regBasePri = 0x01;												// non-zero, hence ignore interrupts with lower priority
		
		//Set interrupt priorities to 5 (any arbitrary number [0-7] is ok). Value has to be higher than SysTick (so SysTick has higher priority)
		NVIC_SetPriority(GPIOA_IRQn,5);								// PortA
		NVIC_SetPriority(GPIOB_IRQn,5);								// PortB
		
		// Enable interrupts
		NVIC_EnableIRQ(GPIOA_IRQn);										// PortA
		NVIC_EnableIRQ(GPIOB_IRQn);										// PortB
		__enable_irq();                 //enable the global interrupt flag
		
			/*OLD
			volatile unsigned long delay;
			SYSCTL->RCGC2 |= SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOB | SYSCTL_RCGC2_GPIOE; //enable clocks for GPIOA/GPIOB/GPIOE
			delay = SYSCTL->RCGC2;          	// dummy delay   
			GPIO_PORTA_CR_R = 0xE0;           // allow changes to Port A 5,6,7     
			GPIO_PORTB_CR_R = 0x13;						// allow changes to Port B 1,2,4
			GPIO_PORTE_CR_R = 0x30;						// allow changes to Port E 4,5
			
			GPIO_PORTA_AMSEL_R = 0x00;        // disable analog function
			GPIO_PORTB_AMSEL_R = 0x00;        // disable analog function
			GPIO_PORTE_AMSEL_R = 0x00;        // disable analog function
			
			GPIO_PORTA_PCTL_R = 0x00000000;   // GPIO clear bit PCTL 
			GPIO_PORTB_PCTL_R = 0x00000000;   // GPIO clear bit PCTL
			GPIO_PORTE_PCTL_R = 0x00000000;   // GPIO clear bit PCTL
			
			GPIO_PORTA_DIR_R &=~0xE0;         // clear bits for PA7 PA6 PA5 to make them inputs 
			GPIO_PORTB_DIR_R &=~0x10;         // clear bit for PB4 to make it an input
			GPIO_PORTB_DIR_R |= 0x03;         // set bits PB0 and PB1 to be outputs
			GPIO_PORTE_DIR_R |= 0x30;         // set bits PE4 and PE5 to be outputs	
			
			GPIO_PORTA_AFSEL_R = 0x00;        // no alternate function
			GPIO_PORTB_AFSEL_R = 0x00;        // no alternate function
			GPIO_PORTE_AFSEL_R = 0x00;        // no alternate function
			
			
			GPIO_PORTA_PUR_R |=0xE0;          // enable pullup resistors for PA5-7
			GPIO_PORTB_PUR_R |=0x10;          // enable pullup resistor for PB4
			
			
			GPIO_PORTA_DEN_R = 0xE0;          // enable digital pins Port A 5,6,7 
			GPIO_PORTB_DEN_R = 0x13;          // enable digital pins Port B 0,1,4
			GPIO_PORTE_DEN_R = 0x30;          // enable digital pins Port E 4,5
			
			
			GPIO_PORTA_IS_R &=~ 0xE0;     // 		 PA5-7 is edge-sensitive
			GPIO_PORTA_IBE_R &=~ 0xE0;    //     PA5-7 is not both edges
			GPIO_PORTA_IEV_R &=~ 0xE0;    //     PA5-7 falling edge event
			GPIO_PORTA_ICR_R = 0xE0;      // 		 clear flag5-7
			GPIO_PORTA_IM_R |= 0xE0;      // 		 arm interrupt on PA5-7
			
			GPIO_PORTB_IS_R &=~ 0x10;     // 		 PB4 is edge-sensitive
			GPIO_PORTB_IBE_R &=~ 0x10;    //     PB4 is not both edges
			GPIO_PORTB_IEV_R &=~ 0x10;    //     PB4 falling edge event
			GPIO_PORTB_ICR_R = 0x10;      // 		 clear flag4
			GPIO_PORTB_IM_R |= 0x10;      // 		 arm interrupt on PF4
			
			//NVIC_PRI0_R = (NVIC_PRI0_R&0xFFF0000)|0x0000A0A0; // priority
			//NVIC_EN0_R = 0x00000003;      // 		 enable interrupt 1&2 in NVIC
			
			NVIC_EnableIRQ(GPIOA_IRQn);
			NVIC_EnableIRQ(GPIOB_IRQn);
			NVIC_SetPriority(GPIOA_IRQn,5);
			NVIC_SetPriority(GPIOB_IRQn,5);
			
			__enable_irq();                 //enable the interrupts
			*/
	}
}

// This handler is called when PortA interrupt is detected on one of the following: PA5 PA6 PA7
// Input: None
// Output: None
void GPIOA_Handler(void){
	SysTick_Wait_ms(50);										// de-bounce
	key = 1; 																				// flag the "key" variable (any value would do, as long it's not zero)
	GPIOA->ICR = ((1UL<<5)|(1UL<<6)|(1UL<<7));      // acknowledge flags for PA5-7
	GPIOB->ICR = (1UL<<4);     											// acknowledge flag for PB4
}

// This handler is called when PortB interrupt is detected on PB4
// Input: None
// Output: None
void GPIOB_Handler(void){
	SysTick_Wait_ms(50);										// de-bounce
	key = 1; 																				// flag the "key" variable (any value would do, as long it's not zero)
	GPIOA->ICR = ((1UL<<5)|(1UL<<6)|(1UL<<7));      // acknowledge flags for PA5-7
	GPIOB->ICR = (1UL<<4);      										// acknowledge flag for PB4
}


// Flips the keypad Inputs and Outputs (used in multi-key detection)
// Input: N - rows, M - columns, orientation - either 'R' for ROWS (horizontal) or 'C' for COLUMNS (vertical) 
// Output: None
// Supports up to 255x255 keypad size.
void keypadFlip(unsigned char N,unsigned char M, char orientation){ 
	//4x4 Matrix Keypad
	if((N==4) && (M==4)){
		if(orientation == 'R'){
			// IRQ
			__enable_irq();																// re-enable interrupts
			// I/O
			GPIOA->DIR &=~((1UL<<5)|(1UL<<6)|(1UL<<7));		// clear bits for PA7 PA6 PA5 to make them inputs 
			GPIOB->DIR &=~(1UL<<4);												// clear bit for PB4 to make it an input
			GPIOB->DIR |= ((1UL<<0)|(1UL<<1));						// set bits PB0 and PB1 to be outputs
			GPIOE->DIR |= ((1UL<<4)|(1UL<<5));						// set bits PE4 and PE5 to be outputs
			// Pull-ups
			GPIOA->PUR |= ((1UL<<5)|(1UL<<6)|(1UL<<7));		// enable pullup resistors for PA5-7
			GPIOB->PUR |= (1UL<<4);												// enable pullup resistor for PB4
			// Enable
			GPIOA->DEN |= ((1UL<<5)|(1UL<<6)|(1UL<<7));		// enable digital pins Port A 5,6,7 
			GPIOB->DEN |= ((1UL<<0)|(1UL<<1)|(1UL<<4));		// enable digital pins Port B 0,1,4
			GPIOE->DEN |= ((1UL<<4)|(1UL<<5));						// enable digital pins Port E 4,5
			
			/* OLD
			GPIO_PORTA_DIR_R &=~0xE0;         // clear bits for PA7 PA6 PA5 to make them inputs 
			GPIO_PORTB_DIR_R &=~0x10;         // clear bit for PB4 to make it an input
			GPIO_PORTB_DIR_R |= 0x03;         // set bits PB0 and PB1 to be outputs
			GPIO_PORTE_DIR_R |= 0x30;         // set bits PE4 and PE5 to be outputs	
			
			GPIO_PORTA_PUR_R |=0xE0;          // enable pullup resistors for PA5-7
			GPIO_PORTB_PUR_R |=0x10;          // enable pullup resistor for PB4
			
			GPIO_PORTA_DEN_R = 0xE0;          // enable digital pins Port A 5,6,7 
			GPIO_PORTB_DEN_R = 0x13;          // enable digital pins Port B 1,2,4
			GPIO_PORTE_DEN_R = 0x30;          // enable digital pins Port E 4,5
			*/
		}	
		if(orientation == 'C'){
			// IRQ
			__disable_irq();															// temporarily disable interrupts to avoid unexpected behaviour in between of swapping I/O
			//I/O
			GPIOA->DIR |= ((1UL<<5)|(1UL<<6)|(1UL<<7));		// set bits for PA7 PA6 PA5 to make them outputs 
			GPIOB->DIR |= (1UL<<4);												// set bit for PB4 to make it an output
			GPIOB->DIR &=~((1UL<<0)|(1UL<<1));						// clear bits PB0 and PB1 to make them inputs
			GPIOE->DIR &=~((1UL<<4)|(1UL<<5));						// clear bits PE4 and PE5 to make them inputs
			//Pull-ups
			GPIOE->PUR |= ((1UL<<4)|(1UL<<5));						// enable pullup resistors for PE4 and PE5
			GPIOB->PUR |= ((1UL<<0)|(1UL<<1));						// enable pullup resistor for PB0 and PB1
			//Enable
			GPIOA->DEN |= ((1UL<<5)|(1UL<<6)|(1UL<<7));		// enable digital pins Port A 5,6,7 
			GPIOB->DEN |= ((1UL<<0)|(1UL<<1)|(1UL<<4));		// enable digital pins Port B 0,1,4
			GPIOE->DEN |= ((1UL<<4)|(1UL<<5));						// enable digital pins Port E 4,5
			
			/* OLD
			GPIO_PORTA_DIR_R |= 0xE0;         // clear bits for PA7 PA6 PA5 to make them inputs 
			GPIO_PORTB_DIR_R |= 0x10;         // clear bit for PB4 to make it an input
			GPIO_PORTB_DIR_R &= ~0x03;         // set bits PB0 and PB1 to be outputs
			GPIO_PORTE_DIR_R &= ~0x30;         // set bits PE4 and PE5 to be outputs	
			
			GPIO_PORTE_PUR_R |=0x30;          // enable pullup resistors for PA5-7
			GPIO_PORTB_PUR_R |=0x03;          // enable pullup resistor for PB4
			
			GPIO_PORTA_DEN_R = 0xE0;          // enable digital pins Port A 5,6,7 
			GPIO_PORTB_DEN_R = 0x13;          // enable digital pins Port B 1,2,4
			GPIO_PORTE_DEN_R = 0x30;          // enable digital pins Port E 4,5
			*/
		}
	}
}

// Waits for user input from keyboard by putting processor to sleep (efficient way to detect user input)
// Inputs: None
// Outputs: None
// Once key is pressed it is written to the global variable "key", but before it is re-set down below for every read
void WaitForKey(void){
	key = 0; 																				// re-set the global flag value
	GPIOB->DATA &=~ ((1UL<<0)|(1UL<<1)); 						// clear PB0,PB1 so they are ready to take inputs (Inverted logic due to internal PULL-UPs)
	GPIOE->DATA &=~ ((1UL<<4)|(1UL<<5));						// clear PE4,PE5 so they are ready to take inputs (Inverted logic due to internal PULL-UPs)
	__wfi();  																			// wait for interrupt
}

// Sets a given row high
// Input: i - tells which row should be set active
// Ouptu: None
void enableRow(int i){
	if(i==0){													// i==0
		GPIOE->DATA |= (1UL<<(4));			// re-set the previous pin
		GPIOE->DATA &=~ (1UL<<(5)); 		// PE5. Inverted because of PULL-UPs
		GPIOB->DATA = 0xFF; 						// PortB inactive. Inverted because of PULL-UPs
	}
	else if(i==1){
		GPIOE->DATA |= (1UL<<(5));			// re-set the previous pin
		GPIOE->DATA &=~ (1UL<<(4)); 		// PE4. Inverted because of PULL-UPs
		GPIOB->DATA = 0xFF; 						// PortB inactive. Inverted because of PULL-UPs
	}
	else if (i>1 && i<4){							// i==2 or i==3
		GPIOB->DATA =~ (1UL<<(3-i));		// Either PB0 or PB1. If i==2 then 1, if i==3 then 0. Inverted because of PULL-UPs
		//GPIOE->DATA |= ((1UL<<5) | (1UL<<4)) ; 						// PortE inactive. Inverted because of PULL-UPs
	}
}

// Sets a given column high
// Input: i - tells which column should be set active
// Ouptu: None
void enableColumn(int i){
	if(i>=0 && i<3){									// i==0 or 1 or 2
		GPIOA->DATA =~ (1UL<<(7-i)); 		// Either PA5 or PA6 or PA7. If i==0 then 7, if 1 then 6, if 2 then 5.Inverted because of PULL-UPs
		GPIOB->DATA = 0xFF; 						// PortB inactive. Inverted because of PULL-UPs
	}
	else if (i==3){										// i==3
		GPIOB->DATA =~ (1UL<<4);				// PB4. Constant 4. Inverted because of PULL-UPs
		GPIOA->DATA = 0xFF; 						// PortE inactive. Inverted because of PULL-UPs
	}
}
