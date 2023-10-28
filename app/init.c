/**
 * @file init.c
 * @author brimonzzy (zzybrimon@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "init.h"


/* temp temp temp temp */
int fputc(int ch, FILE *f)
{
    uint8_t temp[1] = {ch};
    HAL_UART_Transmit(&huart1, temp, 1, 2);  /* huart1 */
    return ch;
}





/**
 * @brief All tasks and hardwares initial here, it's the begin of the project.
 *
 * @param argument 
 */
void StartDefaultTask(void const * argument)
{



    printf("MasRobo-Firmware by BriMonzZY\n\n");
	
	
    while(1) {
        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        osDelay(200);
    }
}

