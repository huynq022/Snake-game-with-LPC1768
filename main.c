#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <GPIO_LPC17xx.h>
#include <GLCD_Config.h>
#include <Board_GLCD.h>
#include <GLCD_Fonts.h>
#include <stdlib.h> 
#include <Driver_SPI.h>
#define xLim 200
#define	yLim 120
#define PRESCALE (50000-1)
void delayms(unsigned int milliseconds);
void Timer0_Init(void);
const int static snakeSize = 5;
#define Joystick_key_up 23
#define Joystick_key_down 25
#define Joystick_key_left 26
#define Joystick_key_right 24
int static length = 3;
int static x[30] = {5, 10, 15, 20, 25, 30, 35, 40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int static y[30] = {5, 5, 5, 5, 5, 5, 5, 5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int static mode = 1;
int static xF = 0;
int static yF = 0;
int static state = 1;
char myString[]={"You're death !!!"};
void printSnakeB(){
for(int i =0; i<length;i++){
GLCD_SetForegroundColor	(GLCD_COLOR_BLACK);
GLCD_DrawRectangle((uint32_t)x[i],(uint32_t)y[i],snakeSize,snakeSize);	
	}
}
void rePrintFruit(){
GLCD_SetForegroundColor	(GLCD_COLOR_WHITE);
GLCD_DrawRectangle((uint32_t)xF,(uint32_t)yF,snakeSize,snakeSize);
GLCD_SetForegroundColor	(GLCD_COLOR_BLACK);
GLCD_DrawRectangle((uint32_t)xF,(uint32_t)yF,snakeSize,snakeSize);	
}
void printSnakeW(){
for(int i =0; i<length;i++){
GLCD_SetForegroundColor	(GLCD_COLOR_WHITE);
GLCD_DrawRectangle((uint32_t)x[i],(uint32_t)y[i],snakeSize,snakeSize);	
	}	
}
void printFruit(){
GLCD_SetForegroundColor	(GLCD_COLOR_WHITE);
GLCD_DrawRectangle((uint32_t)xF,(uint32_t)yF,snakeSize,snakeSize);	
xF = (rand() % 36 )*5;
yF = (rand() % 25 )*5;
GLCD_SetForegroundColor	(GLCD_COLOR_BLACK);
GLCD_DrawRectangle((uint32_t)xF,(uint32_t)yF,snakeSize,snakeSize);	
}
void updateSnake(int xNew, int yNew) {
	printSnakeW();
	for(int i=0; i<length-1;i++){
		x[i] = x[i+1];
		y[i] = y[i+1];
	}
	x[length-1] = xNew;
	y[length-1] = yNew;
	printSnakeB();
}
void checkDeath(){
if(x[length-1] > 315 || x[length-1] < 4 || y[length-1] >235 || y[length-1] < 0)
	state =0;
}
void moveLeft(){
updateSnake(x[length-1]-5,y[length-1]);
}
void moveRight(){
updateSnake(x[length-1]+5,y[length-1]);
}
void moveUp(){
updateSnake(x[length-1],y[length-1]-5);
}
void moveDown(){
updateSnake(x[length-1],y[length-1]+5);
}
void move(){
switch (mode){
	case 0:
		moveLeft();
		break;
	case 1:
		moveRight();
		break;
	case 2:
		moveUp();
		break;
	case 3:
		moveDown();
		break;
	default:
		break;
}
}
void checkEat(){
for (int i =0; i<length;i++){
if (xF == x[i] && yF == y[i]){ 
	printFruit();
	x[length] = x[length-1];
	y[length] = y[length-1];
	length++;
}
}
}

int main( void ) {
	LPC_GPIO1->FIODIR &= ~((1<<Joystick_key_up)|(1<<Joystick_key_down)|(1<<Joystick_key_left)|(1<<Joystick_key_right));
	Timer0_Init();
	GLCD_Initialize()	;      
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24)	;
	GLCD_SetBackgroundColor	(GLCD_COLOR_BLACK) ;
	GLCD_SetBackgroundColor	(GLCD_COLOR_WHITE);
	printFruit();
	while ( state ) {
		
		for (int i =0; i <10 ; i++){
			if (i==5){
		move();
		rePrintFruit();
		checkEat();}
		delayms(10);
		checkDeath();
			if((LPC_GPIO1->FIOPIN & (1<<Joystick_key_up))==0){
				delayms(10);
				if (mode == 3) mode =3; else mode =2;
				delayms(10);			
			}
		if((LPC_GPIO1->FIOPIN & (1<<Joystick_key_down))==0){
				delayms(10);
				if (mode == 2) mode =2; else mode =3;
				delayms(10);
			}
	
		if((LPC_GPIO1->FIOPIN & (1<<Joystick_key_left))==0){
				delayms(10);
				if (mode == 1) mode =1; else mode =0;
				delayms(10);
		
			}
		if((LPC_GPIO1->FIOPIN & (1<<Joystick_key_right))==0){
				delayms(10);
				if (mode == 0) mode =0; else mode =1;
				delayms(10);
			}
		if (state == 0) 
		  GLCD_DrawString	(5,200,myString);	
		}
	}
}

void Timer0_Init(void)
{
	LPC_SC->PCONP |= (1<<23); // Turn on the power for timer3
    LPC_SC->PCLKSEL0 |= (1<<3); // PCLKSEL0[3:2]="10" mean system clock /2 
                                // 100Mhz/2 = 50MHz/50.000 => 1000HHz => T=1ms
	LPC_TIM0->CTCR = 0x0;
	LPC_TIM0->PR = PRESCALE;
 
	LPC_TIM0->TCR = 0x02;
}
 
void delayms(unsigned int milliseconds)
{
	LPC_TIM0->TCR = 0x02;
	LPC_TIM0->TCR = 0x01;
 
	while(LPC_TIM0->TC < milliseconds);
 
	LPC_TIM0->TCR = 0x00;
}
