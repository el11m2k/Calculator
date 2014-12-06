//LCD Pins
#define LCD_DB7_TO_DB4 					(*((volatile unsigned long *)0x4000703C)) // PD3, PD2, PD1, PD0
#define LCD_RW									(*((volatile unsigned long *)0x40024004)) // PE0
#define LCD_RS									(*((volatile unsigned long *)0x40024008)) // PE1
#define LCD_ENABLE							(*((volatile unsigned long *)0x40024010)) // PE2

//LCD Functions

void portInit(void);
void lcdInit(void);
void lcdENPulse(void);
void lcdWriteCommand(unsigned char c);
void lcdClearScreen(void);
void lcdWriteData(char c);
void lcdGoto(unsigned char row, unsigned char column);
void lcdWriteRamString(char *string);
void welcomeMessage(void);
void displayFrame(void);

//CGRAM 
void setCGRAMaddr(unsigned char address);
