// Documentation Section 
// main.c
// Runs on TM4C123
// Authors: XXX
// Date: XXX

// Pre-processor Directives Section
#include "main.h"
#include "PLL.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "KEYPAD_Driver.h"
#include "LCD_Driver.h"
#include "tm4c123gh6pm.h"
#include "Graphics.h"
#include "Speaker.h"
// For boolean
#include "stdbool.h"
// For calculation
#include "Evaluate_expression.h"

//////////////////////////////////////////////////////////
//   Declarations Section
//////////////////////////////////////////////////////////

//Global variables
unsigned char key = 0;							// this is a global vvariable that stores the key(s) pressed

//Local variables
STATES state = ST_INIT;							// default state is for initialisation (ST_INIT)  
//unsigned char test = 0;							// DELETE ME - shows the key pressed

//
bool raddeg = false;

// Subroutines Section
// MAIN: Mandatory for a C Program to be executable
int main(void){

  
  while(1) {												// infinite loop - main execution
    switch(state){
      case ST_INIT:									// initialise all peripherals
				portInit();
				keypadInit(4,4); 						// initialise KEYPAD for 4x4 matrix keypad
				speakerInit();							// initialise the speaker
				PLL_Init();									// initialise PLL
				SysTick_Init();							// initialise SysTick
				lcdInit();									// inititialise LCD
				lcdClearScreen();						// clear LCD

				state = ST_MAIN_MENU; 			// next state - main menu
			break;
			
      case ST_MAIN_MENU:						// present Main Menu
				state=(STATES)mainMenu(); 	// since mainMenu returns an int (not an enum) we cast STATES on it
      break;
			
			case ST_REGULAR_CALCULATION:	// regular Calculation menu option
				state=(STATES)regularCalculation(); 	// since regularCalculation() returns an int (not an enum) we cast STATES on it
      break;
			
			case ST_SINCOS:								// sine/Cosine menu option
				state=(STATES)waveformDisplay();
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

int waveformDisplay(void) {
	
	char period = 40;
	float angle = 0.0;
	float omega = 0;
	char amplitude = 8;
	char value[3] = {0};	// string that stores the value entered
	char mode = 0; 				// mode defines period/angle/amplitude
	
	// Disable cursor
	lcdWriteCommand(0x0C);
	lcdClearScreen();
	initFrame();
	//updateFrame(40, 0, 8, 255,'S');
	
	while(1) {
		updateFrame(period, angle, amplitude, 255, 'S');
		angle+=omega;
		WaitForKeySoft();
		if(key){
			key = readKeypad(); 
				switch(key){
					
					case 0x11:
						switch(mode){
							case 1: //period
								if(period<39)period+=20;
								lcdGoto(0,14);
								sprintf(value,"%d",period/2);
								lcdWriteRamString(value);
							break;
							case 2: //angular velocity
								omega+=0.1;									
								lcdGoto(0,12);
								if (omega>=0) lcdWriteData(' ');
								sprintf(value,"%.1f",omega);
								lcdWriteRamString(value);
							break;
							case 3: //amplitude
								if(amplitude<8)amplitude+=2;
								lcdGoto(0,15);
								sprintf(value,"%d",amplitude);
								lcdWriteRamString(value);
							break;
						}
					break;
						
					case 0x21:
						switch(mode){
							case 1: //period
								if(period>21)period-=20;
								lcdGoto(0,14);
								sprintf(value,"%d",period/2);
								lcdWriteRamString(value);
							break;
							case 2: //angular velocity
								omega-=0.1;
								lcdGoto(0,12);
								if (omega>=0) lcdWriteData(' ');
								sprintf(value,"%.1f",omega);
								lcdWriteRamString(value);
							break;
							case 3: //amplitude
								if(amplitude>0)amplitude--;
								lcdGoto(0,15);
								sprintf(value,"%d",amplitude);
								lcdWriteRamString(value);
							break;
						}
					break;	
						
					case 0x18:				// period
						clearWaveformSetting();
						lcdGoto(0,7);	//go to location		
						lcdWriteRamString("Period:");
						sprintf(value,"%d",period);
						lcdWriteRamString(value);
						mode = 1;
					break;
					
					case 0x14:				// angle
						clearWaveformSetting();
						lcdGoto(0,6);	//go to location		
						lcdWriteRamString("Omega: ");
						sprintf(value,"%.1f",omega);
						lcdWriteRamString(value);
						mode = 2;
					break;
					
					case 0x12:				// amplitude
						clearWaveformSetting();
						lcdGoto(0,11);	//go to location		
						lcdWriteRamString("Amp:");
						sprintf(value,"%d",amplitude);
						lcdWriteRamString(value);
						mode = 3;
					break;
					
					case 0x42:				// clear the text	
						//lcdClearScreen();
						clearWaveformSetting();
						mode = 0;
					break;
				}
			}
		}
	}
		
// Performs regular numeric calculation
// Inputs: None
// Outputs: state 
int regularCalculation(){
	char input[100], current_pos = 0;
	char input_processed[50];
	char output[50];
	char result[16];
	float arguments[50];
	int i = 0;
	bool delete_symbol = false;
	
	// Enable cursor
	lcdWriteCommand(0x0F);
	// Clear sreen
	lcdClearScreen();
	
	while(1) {
		
		WaitForKey();															// Re-initialises the "key" flag and waits for an interrupt (puts processor to sleep)
		if(key){																	// if key press detected
			key = readKey();
			if(key){
				beepINS();
				// if number
				if ((key >= '0') && (key <= '9')) {
					lcdWriteData(key);
					input[current_pos] = key; current_pos++;
				} else if (key == 'A') {
					lcdWriteData('+');
					input[current_pos] = '+'; current_pos++;
				} else if (key == 'B') { 
					lcdWriteData('-');
					input[current_pos] = '-'; current_pos++;
				} else if (key == 'C') {
					if (current_pos > 0) {
						current_pos--;
						delete_symbol = true;
					}	
					
					// remove previous symbol
					//lcdClearScreen(); // Clear screen
					
				} else if (key == 97) {
					lcdWriteData('(');
					input[current_pos] = '('; current_pos++;
				} else if (key == 98) {
					lcdWriteData(')');
					input[current_pos] = ')'; current_pos++;
				} else if (key == 99) {
					lcdWriteRamString("asin(");
					strcat(input, "asin("); current_pos += 5;
				} else if (key == 100) {
					lcdWriteRamString("acos(");
					strcat(input, "acos("); current_pos += 5;
				} else if (key == 101) {
					lcdWriteRamString("atan(");
					strcat(input, "atan("); current_pos += 5;
				} else if (key == 102) {
					lcdWriteRamString("ln(");
					strcat(input, "ln("); current_pos += 3;
				} else if (key == 103) {
					lcdWriteData(0xF7); // Pi Symbol
					input[current_pos] = 0xF7; current_pos++;
				} else if (key == 104) {
					lcdWriteData('*');
					input[current_pos] = '*'; current_pos++;
				} else if (key == 105) {
					lcdWriteData('/');
					input[current_pos] = '/'; current_pos++;
				} else if (key == 106) {
					current_pos = 0;
					memset(input, 0, sizeof(input)); // Clear input array
					lcdClearScreen(); // Clear screen
				} else if (key == 107) {
					return ST_MAIN_MENU; // Exit
				} else if (key == 108) {
					lcdWriteData('^');
					input[current_pos] = '^'; current_pos++;
				} else if (key == 109) {
					lcdWriteRamString("sqrt(");
					strcat(input, "sqrt("); current_pos += 5;
				} else if (key == 110) {
					lcdWriteRamString("log(");
					strcat(input, "log("); current_pos += 4;
				} else if (key == 111) {
					lcdWriteRamString("sin(");
					strcat(input, "sin("); current_pos += 4;
				} else if (key == 112) {
					lcdWriteRamString("cos(");
					strcat(input, "cos("); current_pos += 4;
				} else if (key == 113) {
					lcdWriteRamString("tan(");
					strcat(input, "tan("); current_pos += 4;
				} else if (key == 114) {
					lcdWriteData('.');
					input[current_pos] = '.'; current_pos++;
				}	else if (key == '#') {
					//lcdClearScreen();
					beepANS();

					lcdGoto(1,0);
										
					input[current_pos] = 0; // Termination
					// Calculate the result
					/* CALCULATION BIT */
					
					if (process_input(input, input_processed, arguments)) {
						//lcdWriteRamString(input_processed);
						if (shunting_yard(input_processed, output)) {
							//lcdWriteRamString(output);
							
							if (calculate(output, result, arguments, raddeg)) {
								lcdWriteData('=');
								lcdWriteRamString(result);
								
								
							} else {
								lcdWriteRamString("Error");
							}
							
						} else {
							lcdWriteRamString("Error");
						}
					} else {
						lcdWriteRamString("Error");
					}
					// clear previous answer
					lcdWriteRamString("               ");
					/* CALCULATION END */					
					
					//lcdWriteRamString(input);
					
				}
				
				lcdGoto(0,current_pos);
				
				input[current_pos] = 0; // Termination
				
				// if the end of the string reached do this
							
				if (current_pos >= 15) {
					lcdGoto(0,0);
					
					for (i = current_pos - 15; i < current_pos; i++) {
						// Enable cursor
						//lcdWriteCommand(0x0F);
						lcdWriteData(input[i]);
					}
					lcdWriteData(' ');
					lcdGoto(0,15);
				} else if (delete_symbol) {
					
					lcdGoto(0,current_pos);
					lcdWriteData(' ');
					lcdGoto(0,current_pos);
					
				}
				
			}
		}
	}
}

// Reads the button pressed and decides upon the next state
// Inputs: None
// Outputs: state
int mainMenu(){	
	// Disable cursor
	lcdWriteCommand(0x0C);
	lcdClearScreen();
	
	lcdGoto(0,0);
	lcdWriteRamString("1.CALCULATOR");
	lcdGoto(1,0);
	lcdWriteRamString("2.WAVEFORM");
	
	lcdGoto(0, 15);
	if (raddeg) {
		lcdWriteData('R');
	} else {
		lcdWriteData('D');
	}
	
	while(1) {
		
		WaitForKey();															// Re-initialises the "key" flag and waits for an interrupt (puts processor to sleep)
		if(key){																	// if key press detected
			key = readKey();
			if(key){
				lcdWriteData('p');
				
				switch (key){	// decide which menu option to go to
					case '1':
						//OPTION 1 - REGULAR NUMERIC CALCULATIONS
						return ST_REGULAR_CALCULATION;
					
					case '2':
						//OPTION 2
						return ST_SINCOS;
					break;
					
					case 'A':
						lcdGoto(0, 15);
						raddeg = !raddeg;
						if (raddeg) {
							lcdWriteData('R');
						} else {
							lcdWriteData('D');
						}
						break;
					default:
						return ST_MAIN_MENU; // Return the state as INT (enums and integers are interchangable in this context)
				}
			}
		}
	}
}

/*
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
*/