#include <msp.h>
#include "fate.h"
#include "BSP.h"

int time = 0;

void LCD_Print_Exec_Time(){
	BSP_LCD_SetCursor(10, 7);
	BSP_LCD_OutUDec(time, BSP_LCD_Color565(0, 255, 0)); // number printed is in system ticks (10 ms)
	time = 0;
}

void LCD_Print_Count(){
	static int count = 0;
	BSP_LCD_SetCursor(10, 5);
	BSP_LCD_OutUDec(count, BSP_LCD_Color565(255, 0, 255));
	count++;
	LCD_Print_Exec_Time();
	Task_stop((uint32_t)LCD_Print_Count);
}

int main()
{
	TA1CTL &= (uint16_t)(~((1<<5)|(1<<4))); //stop clock
	TA1CTL &= (uint16_t)(~(1<<9));	//ACLK
	TA1CTL |= (uint16_t)((1<<8));  //ACLK 
	TA1CCR0 = (uint16_t)(328); //interrupt every 0.01s
	TA1CTL |= (uint16_t)((0<<5)|(1<<4)); //UP mode
	TA1CTL &= (uint16_t)(~(1<<0)); // clear IFG
	TA1CTL |= (uint16_t)((1<<1)); // enable interrupts
	NVIC_SetPriority(TA1_N_IRQn, 2);
	NVIC_EnableIRQ(TA1_N_IRQn);
	
	Task_list_init();
	BSP_LCD_Init();
	Task_add((uint32_t)LCD_Print_Count,(uint32_t)200,(uint32_t)1);
	Task_schedule();
	
	return 0;
}
void TA1_N_IRQHandler(void){
	TA1CTL &= (uint16_t)(~(1<<0)); //clear any existing flags
	time++;
}