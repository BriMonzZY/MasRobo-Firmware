/**
 * @file board.c
 * @author brimonzzy (zzybrimon@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "board.h"






uint32_t get_time_us(void)
{
    return TIM9->CNT;
}

uint32_t get_time_ms(void)
{
    return HAL_GetTick();
}

float get_time_ms_us(void)
{
    return get_time_ms() + get_time_us() / 1000.0f;
}
