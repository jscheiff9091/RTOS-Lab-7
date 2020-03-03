/*
 * lcd.h
 *
 *  Created on: Feb 28, 2020
 *      Author: Jacob S
 */

#ifndef LCD_H_
#define LCD_H_
#include  <kernel/include/os.h>

// ----- Macros -----
#define LCD_DISP_TASK_PRIO		21u
#define LCD_DISP_STACK_SIZE		1000u

// ----- Global Variables -----
extern OS_TCB LCDDispTaskTCB;                          		/**< Task control block for the LCD Display Task  */
extern CPU_STK LCDDisplayTaskStack[LCD_DISP_STACK_SIZE];	/**< LCD Display task stack  */


// ----- Function Prototypes -----

/// @brief Task to update LCD display with speed and direction information
///
/// @param[in] Task arguments
void LCDDisplayTask(void* p_args);

#endif /* LCD_H_ */
