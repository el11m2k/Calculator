//////////////////////////////////////////////////////////
//   KEYPAD Pins
//////////////////////////////////////////////////////////
#define KP_1										(*((volatile unsigned long *)0x40004200)) //PA7 col1
#define KP_2										(*((volatile unsigned long *)0x40004100)) //PA6 col2
#define KP_3										(*((volatile unsigned long *)0x40004080)) //PA5 col3
#define KP_4										(*((volatile unsigned long *)0x40005040)) //PB4 col4
#define KP_5										(*((volatile unsigned long *)0x40024080)) //PE5 row1
#define KP_6										(*((volatile unsigned long *)0x40024040)) //PE4 row2
#define KP_7										(*((volatile unsigned long *)0x40005008)) //PB1 row3
#define KP_8										(*((volatile unsigned long *)0x40005004)) //PB0 row4

//////////////////////////////////////////////////////////
//   Function Prototypes
//////////////////////////////////////////////////////////
void keypadInit(unsigned char N, unsigned char M);
void keypadFlip(unsigned char N,unsigned char M, char orientation);
char decodeKeyPress(unsigned long k);
unsigned long readKeypad(char multikey);
void WaitForKey(void);
void enableRow(int i);
void enableColumn(int i);

