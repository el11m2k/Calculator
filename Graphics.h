//////////////////////////////////////////////////////////
//   Function Prototypes
//////////////////////////////////////////////////////////

void findWave(float period, float angle, char amplitude, char sincos);
void initSinFlat(signed char val);
void buildSinFlat(unsigned char defaultValue);
void clearSin2D(unsigned char array[8][8]);
void calculateSin2D(unsigned char array[8][8], unsigned char defaultValue);
void getSinwave(char period, float angle, char amplitude, unsigned char defaultValue, char sincos, unsigned char array[8][8]);
