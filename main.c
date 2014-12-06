// Documentation Section 
// main.c
// Runs on TM4C123
// Authors: XXX
// Date: XXX

// Pre-processor Directives Section
#include "main.h"
#include "math.h"
#include "string.h"
#include "KEYPAD_Driver.h"
#include "LCD_Driver.h"
#include "tm4c123gh6pm.h"

//////////////////////////////////////////////////////////
//   Declarations Section
//////////////////////////////////////////////////////////

//Global variables
unsigned char key = 0;							// this is a global vvariable that stores the key(s) pressed
unsigned char testkey = 0;

//Local variables
STATES state = ST_INIT;							// default state is for initialisation (ST_INIT)  
unsigned char test = 0;							// DELETE ME - shows the key pressed
int ic = 0;													// DELETE ME - tracks number of interrupts
float numbers[80];										// stores numbers for calculations
char operations[80];								// stores operations to be performed on numbers. There's very little of operations therefore datatype - char.
int numberSize = 0;									// needed in order to accumulate a full number into multiple-digit integer from keys pressed
int operationSize = 0;							// keep track of number of operations
char testChar = 0;
char testChar1 = 0;
float floatTest = 0;


// Subroutines Section
// MAIN: Mandatory for a C Program to be executable
int main(void){

  
  while(1) {												// infinite loop - main execution
    switch(state){
      case ST_INIT:									// initialise all peripherals
				portInit();
				keypadInit(4,4); 						// initialise KEYPAD for 4x4 matrix keypad
				PLL_Init();									// initialise PLL
				SysTick_Init();							// initialise SysTick
				lcdInit();									// inititialise LCD
				lcdClearScreen();						// clear LCD
				//lcdWriteRamString("Hello, World!");
				state = ST_MAIN_MENU; 			// next state - main menu
			break;
			
      case ST_MAIN_MENU:						// present Main Menu
				state=(STATES)mainMenu(); 	// since mainMenu returns an int (not an enum) we cast STATES on it
      break;
			
			case ST_REGULAR_CALCULATION:	// regular Calculation menu option
				state=(STATES)regularCalculation(); 	// since regularCalculation() returns an int (not an enum) we cast STATES on it
      break;
			
			case ST_SINCOS:								// sine/Cosine menu option
				//Sine/Cosine menu option
      break;
			
			case ST_FLOATING_MATHS:				// floating point maths menu option
				//Floating point maths menu option
      break;
			
			case ST_SNAKE:								// snake game menu option
				//Snake game menu option
      break;
			
			default:
				state = ST_MAIN_MENU; 			// in case of uknown state - go back to main menu
    }
  }
}

// Performs regular numeric calculation
// Inputs: None
// Outputs: state 
int regularCalculation(){

	char* calculationString; 								//buffer used for itoa()
	WaitForKey();
	if(key){
		ic++;
		key = 0;	// read key and decode it, then store in global variable
		if(((key-48) >= 0) && ((key-48) < 10)){		// if key decoded is between X-48 and X-48+9 then it's a number (-48 is due to ASCII)		
			if(!numberSize && !(key-48)){ 
				// acount for 0 if it's been pressed in the beginning, hence if number pressed was 0 and there were no previous numbers - ignore.
			}
			else{
				//show key (0 to 9) on LCD !!!!
				lcdWriteData(key);
				if(numberSize) numbers[operationSize] *= 10;	// if our number is multi-digit (numberSize != 0) then multiply previous value by 10 and add our new digit to the back of the total number, which is located at the number array indexed by operations (more operations - more numbers)
				numbers[operationSize] += (key-48);
				numberSize++; //keeps track of decimal size (x*10,x*100 and etc)
			}
		}
		else{ //otherwise it's an operation hence account individually
			switch (key){
				case '*': //clear and return to Main Menu
					//clearOperationValues();					// clear operation values
					//return ST_MAIN_MENU;						// go back to Main Menu
				break;
				
				case '#': 	//calculate the resulting value
					lcdWriteData('=');
					operations[operationSize] = 0; //add 'terminating' symbol
					floatTest = calculateResult(numbers,operations);
					//calculationString = convertIntToString((int)floatTest); //find the final result
					lcdGoto(1,0);
					lcdWriteRamString(convertIntToString((int)floatTest));
					clearOperationValues();					// clear operation values
				break;
				
				case 'A': //add
					lcdWriteData('+');
					operations[operationSize] = 1; 	// let 1 be ADD, 2 - SUBTRACT, 3 - MULTIPLY, 4 - DIVIDE
					operationSize++;
				break;
				
				case 'B':
					lcdWriteData('-');
					operations[operationSize] = 2; 	// let 1 be ADD, 2 - SUBTRACT, 3 - MULTIPLY, 4 - DIVIDE
					operationSize++;
				break;
				
				case 'C':
					lcdWriteData('*');
					operations[operationSize] = 3;	 // let 1 be ADD, 2 - SUBTRACT, 3 - MULTIPLY, 4 - DIVIDE
					operationSize++;
				break;
				
				case 'D':
					lcdWriteData('/');
					operations[operationSize] = 4; 	// let 1 be ADD, 2 - SUBTRACT, 3 - MULTIPLY, 4 - DIVIDE
					operationSize++;
				break;
				
				case '(':
					lcdWriteData('(');
				break;
				
				case ')':
					lcdWriteData(')');
				break;		
				
				case '.':
					lcdWriteData('.');
				break;	
				
				case '^':
					lcdWriteData('^');
				break;		
				
				default:
					return ST_REGULAR_CALCULATION; 	// Return the state as INT (enums and integers are interchangable in this context)
			}	
		}
	}
	return ST_REGULAR_CALCULATION;
}

// Reads the button pressed and decides upon the next state
// Inputs: None
// Outputs: state
int mainMenu(){
	WaitForKey();															// Re-initialises the "key" flag and waits for an interrupt (puts processor to sleep)
	if(key){																	// if key press detected
		testkey = readKey();		
		ic++;																		// track the number of times interrupt was triggered (DEBUGGING)!!
		//readKey();			// read key and decode it, then store in global variable
		switch (key){														// decide which menu option to go to
			case '1':
				//OPTION 1 - REGULAR NUMERIC CALCULATIONS
				return ST_REGULAR_CALCULATION;
			
			case '2':
				//OPTION 2
			break;
			
			case '3':
			//OPTION 3
			break;
			
			case '4':
				//OPTION 4
			break;
			
			case '5':
				//OPTION 5
			break;
			
			case '6':
				//OPTION 6
			break;
			
			case '7':
				//OPTION 7
			break;
			
			case '8':
				//OPTION 8
			break;
			
			case '9':
				//OPTION 9
			break;
			
			case '0':
				//OPTION 0
			break;
			
			case '*':
				//OPTION *
			break;
			
			case '#':
				//OPTION #
			break;
			
			case 'A':
				//OPTION A
			break;
			
			case 'B':
				//OPTION B
			break;
			
			case 'C':
				//OPTION C
			break;
			
			case 'D':
				//option D
			break;
						
			default:
				return ST_MAIN_MENU; // Return the state as INT (enums and integers are interchangable in this context)
		}
	}
	return ST_MAIN_MENU;	// no button was pressed, hence continue looping in Main Menu
												// return the state as INT (enums and integers are interchangable in this context)
}


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

// Clears operation-related values
// Inputs: None
// Outputs: None
void clearOperationValues(void){
	//lcdClearScreen();
	memset(&numbers[0], 0, sizeof(numbers));					// set whole "numbers" array to 0
	memset(&operations[0], 0, sizeof(operations));		// set whole "operations" array to 0
	numberSize = 0;																		// re-set numberSize
	operationSize = 0;																// re-set operationSize
}

// For given number and operation arrays, calculates the resulting value
// Inputs: array of ints for numbers and array of chars for operations
// Outputs: resulting value
float calculateResult(float* numbers, char* operations){
	int i = 0;
	float result = numbers[i];
	for(i=0; *operations; i++,operations++){ //loop until operations[x] reach 0, every loop increment pointer to operations and also increment i
		switch(*operations){
			case 1: 						// add
				result += numbers[i+1];
			break;
			
			case 2:							// subtract
				result -= numbers[i+1];
			break;
			
			case 3:							// multiply
				result *= numbers[i+1];
			break;
			
			case 4:							// divide
				result /= numbers[i+1];
			break;
		}
	}

	numbers[0] = result;
	
	return result; 					// once we itterate through all operations, return the resulting value
}

// Converts a given integer to a string (array of chars)
// Inputs: value to convert
// Outputs: representing string
char* convertIntToString(int val){
	static char str[16] = {7};					// initialise a static buffer with all 'X' (anything but -, that does not override each time function is called
	
	int i = 15;														// start with string length-1 (last one is /0)
	boolean negative = FALSE;							// flag for adding "minus" sign to the number if it's negative
	
	memset(&str[0], 7, sizeof(str));			// set whole string to have X's
	
	if (val < 0) {
		val = val * (-1); 													// make number positive
		negative = TRUE;
	}
	
	for(i=15; val && i; i--, val /= 10){ 	// itterate through all characters of the string except the last (which is /0)
		str[i] = "0123456789"[val % 10];		// for a given decimal value, find the remainder and match that to a constant string of 0-9
	}
	
	if(negative) str[i] = '-';
	
	return str;	
}

// Converts a given floating point number to a string (array of chars)
// Inputs: value to convert
// Outputs: representing string
char* convertFloatToString(double val, int decimalPlaces){
	static char str[16] = {7};									// initialise a static buffer with all 'X' (anything but -, that does not override each time function is called
	int intVal = 0;
	int decimalPow;
	char stringDigit;
	boolean decPointFlag = FALSE;

	int i = 15;																		// start with string length-1 (last one is /0)
	boolean negative = FALSE;											// flag for adding "minus" sign to the number if it's negative
	
	decimalPow = pow(10, decimalPlaces);
	
	memset(&str[0], 7, sizeof(str));						// set whole string to have X's
	
	if (val < 0) {
		val = val * (-1); 													// make number positive
		negative = TRUE;
	}
	
	//val = roundf(val * decimalPow) / decimalPow; // Round to x decimal places, X = decimalPlaces
	
	intVal = (int)(val * decimalPow); //convert the float value to int and multiply by 10^x, where x is the decimal places we want
	
	for(i=15; intVal && i; i--, intVal /= 10){ 			// itterate through all characters of the string except the last (which is /0)
		stringDigit = "0123456789"[intVal % 10];
				
		if(i==(15-decimalPlaces)){									//if we have reached the decimal place, put a decimal point at that cell
			str[i] = '.';
			i--;
		}
		
		str[i] = stringDigit;					// for a given decimal value, find the remainder and match that to a constant string of 0-9
		
	}

	
	if(negative) str[i] = '-';
	
	return str;	
}
