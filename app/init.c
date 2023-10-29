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



void board_init()
{
	usart1_manage_init();
}



/**
 * @brief All tasks and hardwares initial here, it's the begin of the project.
 *
 * @param argument 
 */
void StartDefaultTask(void const * argument)
{
	board_init();

    log_printf("MasRobo-Firmware by BriMonzZY\n\n");
	
    while(1) {
        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
        osDelay(200);
    }
}

