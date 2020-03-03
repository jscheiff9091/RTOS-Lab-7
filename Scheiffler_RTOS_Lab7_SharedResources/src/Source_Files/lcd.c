/*
 * lcd.c
 *
 *  Created on: Feb 28, 2020
 *      Author: Jacob S
 */

#include "lcd.h"

OS_TCB LCDDispTaskTCB;

CPU_STK LCDDisplayTaskStack[LCD_DISP_STACK_SIZE];

