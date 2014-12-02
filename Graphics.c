#include "Graphics.h"
#include "math.h"

char sinData[20] = {0}; 				// sinusoidal waveform sinData
char sinFlat[40] = {0}; 				// array to hold new linear sinData representataion (4x2 view flattened to 8x1)
char sin2D[8][8] = {0};					// sine sinData converted to 8x8 char representation
signed char defaultVal = -1;						// default value for initSinFlat function, global because it's most likely hard-coded (not dynamic)


//char period = 80; 							// 
//char dAngle = 0; 								// value for incrementing X, a function of period
//char angle = 0;									// angle of the sinusoidal - variable so later we can have it as a function of time (angular velocity)
//char amplitude = 8;							// 


// Finds a sinusoidal wave for a given amplitude and 
// Inputs: period - period of the sinusoidal period=1/f, where f is frequency
// amplitude - amplitude of the sinusoidal
// angle - angle of the sinusoidal - used as variable so we can have it as a function of time (angular velocity)
// Ouptuts: none directly, updates a global array "sinsinData"
void findWave(char period, char angle, char amplitude) {
	char dAngle=(-6.28 / period*2);										// delta_Angle - angular velocity constant (calculated by dividing twoPI by period)
  int i = 0;																				// itterator - goes through every original data value
	angle++;																					// increment angle
  for (i = 0; i < 20; i++) {												// for every x value, calculate a y value
    sinData[i] = (char)((sin(angle)+1)*amplitude);	// sin() returns values [-1 to 1] so we shift that to be [0 to 2] and increase by variable "amplitude". Char cast used because sin() returns a float by default
    angle+=dAngle;
  }
}

// Initialises the global "sinFlat" array with -1 values so our algorithm knows this is default value which can be processed accordingly
// Inputs: val - value to initialise with
// Outputs: None directly, but updates the global "sinFlat" array
void initSinFlat(signed char val){
	int i=0;
  for (i=0; i<40; i++) {
    sinFlat[i]=val;
  }
}

// Builds new linear sinData representation
// Inputs: None
// Outputs: None directly, but updates the global "sinFlat" array
void buildSinFlat(void){
	int i = 0;
  for (i=0; i<20; i++) {
    if (sinData[i]<8) {             // if sinData is less than 8 (bottom vertical half)
      sinFlat[i+20] = sinData[i];   // then put it into new sinData's second linear half
    } else if (sinData[i]<16) {			// if sinData is over 8, but still under 16 (for safety)
      sinFlat[i] = sinData[i]-8;    // put it regularly but make sure sinData is still within 0 to 7 (the -8 transforms [8 to 15] into [0 to 7])
    }
  }
}

// Clears the sin2D array from previous values, sets all chars inside to 0
// Inputs: None
// Outputs: None directly, but updates the global "sin2D" array
void clearSin2D(void){
	int i,j = 0;																						// i - characters, j - rows of each character
	for(i=0; i<8; i++){																			// for every character
		for(j=0; j<8; j++){																		// for every line in a character
			sin2D[i][j] = 0;																		// set that byte to 0, hence 0b 0000 0000, thus no bits are 'on'
		}
	}
}

// Calculates the sin2D array values from "sinFlat" array
// Inputs: None directly, but uses "sinFlat" array
// Outputs: None directly, but updates the global "sin2D" array
void calculateSin2D(void){
	int i,j,t =0; 																					// i - itterates through characters, t - global itterator (goes from 0 to 39), j - itterates for every bit of every byte
  for (i=0; i<8; i++) {         			 								    // for every character
    for (j=0; j<5; j++) {          												// for every row
      if (sinFlat[t] != defaultVal) {											// check for default value, and in case it is - simply ignore
        // below is the 'heart' of the algorithm: row is set by "i" and hence defines which character we change
				// sinFlat[t] gives us a value a sine value at that x (because it's linear)
				// then for every bit, that we detect at that 'altitude' we OR (accumulate bits) it with..
				// .."1" that is shifted from left by 'inverted' j amount (4-j because we want to start at the other end of bit length) - we could also have for loop to go back from 5 rather than start from 0 and then have no subtraction
				// finally we and that with dec31 (bin0001 1111), to get rid of anything from last three bits (we are only interested in first 5 for every character)
				sin2D[i][sinFlat[t]] |= (char)((1<<(4-j))&0x11);
      }
      t++;																							//keep increasing global itterator so it goes from 0 to 39
    }
  }
}

// Algorithm to transform flat wave data into 2d char array, where each symbol is a character with bits representing pixels
// Inputs:
// Outputs:
void transformAlg(void){
	initSinFlat(defaultVal);	// initialise sinFlat array with known default value (-1)
	buildSinFlat();						// build te sinFlat data from "sinData" global array (make 4x2 representation into 8x1)
	clearSin2D();							// since sin2D is global, we clear it not to over-accumulate bits
	calculateSin2D();					// build the "sin2D" - actual algorithm is explained within the function
}
