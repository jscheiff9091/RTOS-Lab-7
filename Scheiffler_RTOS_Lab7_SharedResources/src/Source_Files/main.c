/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3402A
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "bsp.h"

#include "gpio.h"
#include "slider.h"
#include "capsense.h"
#include "cmu.h"
#include "main.h"
#include "lcd.h"

#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"

#include  <bsp_os.h>

#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os.h>
#include  <kernel/include/os_trace.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"

#define HFRCO_FREQ 		40000000

// RTOS Global variables
OS_TCB startTaskTCB;
OS_TCB idleTaskTCB;
OS_TCB vehicleMonTaskTCB;

CPU_STK startTaskStack[START_STACK_SIZE];
CPU_STK idleTaskStack[IDLE_STACK_SIZE];
CPU_STK vehicleMonTaskStack[VEH_MON_STACK_SIZE];

OS_FLAG_GRP vehMonFlags;
OS_TMR	vehTurnTimeout;

/* Main */
int main(void)
{
//	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
//	CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
//
//	/* Chip errata */
//	CHIP_Init();
//
//	/* Init DCDC regulator and HFXO with kit specific parameters */
//	/* Init DCDC regulator and HFXO with kit specific parameters */
//	/* Initialize DCDC. Always start in low-noise mode. */
//	//EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
//	EMU_DCDCInit(&dcdcInit);
//	//em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
//	//EMU_EM23Init(&em23Init);
//	CMU_HFXOInit(&hfxoInit);
//
//	/* Switch HFCLK to HFRCO and disable HFRCO */
//	//CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
//	//CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
//	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
//	CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
//
//
//	//CMU_RouteGPIOClock();       //Enable GPIO Clock
//
//
//	/* Initialize tasks, OS, etc. */
//	//RTOS_ERR err;
///*
//	CPU_Init();								//Example Code called these functions...why not???
//	BSP_SystemInit();
//	OS_TRACE_INIT();
//
//	OSInit(&err);							//Initialize kernel
//	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
//
//	OSTaskCreate(&startTaskTCB,     		//Create start task
//				 "Start Task",
//				 StartTask,
//				 DEF_NULL,
//				 START_TASK_PRIO,
//				 &startTaskStack[0],
//				 (START_STACK_SIZE / 10u),
//				 START_STACK_SIZE,
//				 10u,
//				 0u,
//				 DEF_NULL,
//				 OS_OPT_TASK_STK_CLR,
//				 &err);
//
//	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
//
//	OSStart(&err);                       		//Start the kernel, LET'S DO THIS!!!!
//	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
//
//*/
//	DISPLAY_Init();

//	if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) {
//	    while (1);
//	}
//
//	printf("Welcome to the \ntextdisplay example!");
//	while(1);

	  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
	  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;

	  /* Chip errata */
	  CHIP_Init();

	  /* Init DCDC regulator and HFXO with kit specific parameters */
	  EMU_DCDCInit(&dcdcInit);
	  CMU_HFXOInit(&hfxoInit);

	  /* Switch HFCLK to HFXO and disable HFRCO */
	  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

	  /* Initialize LED driver */
	  BSP_LedsInit();

	  /* Setup SysTick Timer for 1 msec interrupts  */
	  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
	    while (1) ;
	  }

	  /* Initialize the display module. */
	  DISPLAY_Init();

	  /* Retarget stdio to a text display. */
	  if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) {
	    while (1) ;
	  }

	  printf("FUCK OFF");

	  while(1);
}


/* Start Task */
void StartTask(void* p_arg) {
    RTOS_ERR  err;

    PP_UNUSED_PARAM(p_arg);       				//Prevent compiler warning.

    BSP_TickInit();                           	//Initialize Kernel tick source.
	Common_Init(&err);                          // Call common module initialization example.
	APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
	BSP_OS_Init();								//Initialize the kernel

#if defined(uCProbe)
	/* Call these functions to set up uC Probe */
	CPU_TS_TmrInit();							//Initialize timestamp source

	OSStatTaskCPUUsageInit(&err);   			//Call function to initialize stat task
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

	CPU_IntDisMeasMaxCurReset();				//Return something about interrupt timing

	OSStatReset(&err);							//Reset Stats? Also said to call this in Appendix B
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
#endif
	/* Create all mutual exclusion and communication contructs */

	//Create semaphore used to signal a button press occurred to the speed setpoint task
	OSSemCreate(&setptFifoSem, "Button Press Signal Semaphore", CNT_ZERO, &err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

	//Create event flag group to communicate with the vehicle monitor task
	OSFlagCreate(&vehMonFlags, "Vehicle Monitor Event Flags", VEH_MON_CLR_FLAGS, &err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

	//Create the event flag to notify the LED Driver task of LED State change
	OSFlagCreate(&LEDDriverEvent, "Vehicle Warning Event Flag", LED_WARN_CLR_FLAGS, &err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

	//Create mutex to protect the speed setpoint data
	OSMutexCreate(&setptDataMutex, "Speed Setpoint Data Mutex", &err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

	//Create mutex to protect the vehicle direction state variable
	OSMutexCreate(&vehDirMutex, "Vehicle Direction Mutex", &err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

	//Create the timer to schedule the vehicle direction task
	OSTmrCreate(&vehDirTimer,
				"Vehicle Direction Task Timer",
				NO_DLY,
				VEH_DIR_TMR_CNT,
				OS_OPT_TMR_PERIODIC,
				&SLD_TimerCallback,
				DEF_NULL,
				&err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

	//Create vehicle turning monitor timeout timer
	OSTmrCreate(&vehTurnTimeout,
				"Vehicle Turn Timeout Timer",
				VEH_TURN_TIMEOUT,
				0,
				OS_OPT_TMR_ONE_SHOT,
				&VehicleTurnTimeout,
				DEF_NULL,
				&err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

	/* Create all tasks */
	OSTaskCreate(&speedSetPTTaskTCB,     		//Create button input task
				 "Speed Setpoint Task",
				 SpeedSetpointTask,
				 DEF_NULL,
				 SPD_SETPT_TASK_PRIO,
				 &speedSetPTTaskStack[0],
				 (SPD_SETPT_STACK_SIZE / 2u),
				 SPD_SETPT_STACK_SIZE,
				 0u,
				 0u,
				 DEF_NULL,
				 OS_OPT_TASK_STK_CLR,
				 &err);

	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

	OSTaskCreate(&LEDDriverTaskTCB,     		//Create LED driver task
				 "LED Driver Task",
				 LEDDriverTask,
				 DEF_NULL,
				 LED_DRV_TASK_PRIO,
				 &LEDDriverTaskStack[0],
				 (LED_DRV_STACK_SIZE / 2u),
				 LED_DRV_STACK_SIZE,
				 0u,
				 0u,
				 DEF_NULL,
				 OS_OPT_TASK_STK_CLR,
				 &err);

	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&vehicleDirTaskTCB,     		//Create slider input task
				 "Vehicle Direction Monitor Task",
				 VehicleDirectionTask,
				 DEF_NULL,
				 VEH_DIR_TASK_PRIO,
				 &vehicleDirTaskStack[0],
				 (VEH_DIR_STACK_SIZE / 2u),
				 VEH_DIR_STACK_SIZE,
				 0u,
				 0u,
				 DEF_NULL,
				 OS_OPT_TASK_STK_CLR,
				 &err);

    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

    OSTaskCreate(&vehicleMonTaskTCB,     		//Create slider input task
				 "Vehicle Monitor Task",
				 VehicleMonitorTask,
				 DEF_NULL,
				 VEH_MON_TASK_PRIO,
				 &vehicleMonTaskStack[0],
				 (VEH_MON_STACK_SIZE / 2u),
				 VEH_MON_STACK_SIZE,
				 0u,
				 0u,
				 DEF_NULL,
				 OS_OPT_TASK_STK_CLR,
				 &err);

	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
/*
	OSTaskCreate(&LCDDispTaskTCB,     		//Create slider input task
				 "LCD Display Task",
				 LCDDisplayTask,
				 DEF_NULL,
				 LCD_DISP_TASK_PRIO,
				 &LCDDisplayTaskStack[0],
				 (LCD_DISP_STACK_SIZE / 2u),
				 LCD_DISP_STACK_SIZE,
				 0u,
				 0u,
				 DEF_NULL,
				 OS_OPT_TASK_STK_CLR,
				 &err);

	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);
*/
    __enable_irq();						//Global Enable Interrupts

    OSTaskCreate(&idleTaskTCB,     		//Create LED driver task
				 "Idle Task",
				 IdleTask,
				 DEF_NULL,
				 IDLE_TASK_PRIO,
				 &idleTaskStack[0],
				 (IDLE_STACK_SIZE / 2u),
				 IDLE_STACK_SIZE,
				 0u,
				 0u,
				 DEF_NULL,
				 OS_OPT_TASK_STK_CLR,
				 &err);

	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

	while(1);
}

/* Idle Task */
void IdleTask(void* p_args) {

	RTOS_ERR err;

	OSTaskDel(&startTaskTCB, &err);
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);

	while(1) {
		EMU_EnterEM1();
	}
}

void VehicleMonitorTask(void* p_args) {
	RTOS_ERR err;
	CPU_TS timestamp;
	OS_FLAGS flags;

	bool speedWarn = false;		//LED Currently signaling a spped warning
	bool turnWarn = false;		//LED Currently signaling a turn warning
	bool hardTurn = false;		//Currently in a hard turn? if timer expires before this is cleared, send a warning

	while(1) {
		//Wait for speed change, direction change, or hard left/right timeout
		flags = OSFlagPend(&vehMonFlags, VEH_MON_SET_FLAGS, 0, OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_BLOCKING, &timestamp, &err);
		APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
		OSFlagPost(&vehMonFlags, flags, OS_OPT_POST_FLAG_CLR, &err);

		if(flags & SPD_SETPT_FLAG) {																					//Speed change occurred
			//Critical section
			OSMutexPend(&setptDataMutex, 0, OS_OPT_PEND_BLOCKING, &timestamp, &err);
			if(setptData.speed > 70 && !speedWarn) {																	// LED Currently not warning about a speed violation?
				OSFlagPost(&LEDDriverEvent, LED_WARN_SPD_VIOLATION, OS_OPT_POST_FLAG_SET, &err);
				APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
				speedWarn = true;
			}
			else if(setptData.speed < 75 && speedWarn) {																//LED currently warning about a speed violation?
				OSFlagPost(&LEDDriverEvent, LED_WARN_CLR_SPD_VIOLATION, OS_OPT_POST_FLAG_SET, &err);
				APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
				speedWarn = false;
			}
			OSMutexPost(&setptDataMutex, OS_OPT_POST_NONE, &err);
		}

		if(flags & VEH_DIR_FLAG) {																						//Vehicle direction changed
			//Crtical section
			OSMutexPend(&vehDirMutex, 0, OS_OPT_PEND_BLOCKING, &timestamp, &err);
			if((vehicleDir == HardLeft || vehicleDir == HardRight) && !hardTurn) {										//Hard turn initiated
				hardTurn = true;																						//Set State variable
				OSTmrStart(&vehTurnTimeout, &err);																		//Start timeout timer
				APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
			}
			else if((vehicleDir == Left || vehicleDir == Straight || vehicleDir == Right) && turnWarn && hardTurn) {	//Hard turn stopped, turn warning on, need to be turned off
				hardTurn = false;																						//Set turning state variables
				turnWarn = false;
				OSFlagPost(&LEDDriverEvent, LED_WARN_CLR_TRN_VIOLATION, OS_OPT_POST_FLAG_SET, &err);					//Signal LED task to turn off turn warning light
				APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
			}
			else if((vehicleDir == Left || vehicleDir == Straight || vehicleDir == Right) && !turnWarn && hardTurn) {	//Hard turn stopped, timer has not expired yet
				hardTurn = false;
				OSTmrStop(&vehTurnTimeout, OS_OPT_TMR_NONE, DEF_NULL, &err);											//Stop timer
				APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
			}
			OSMutexPost(&vehDirMutex, OS_OPT_POST_NONE, &err);
		}

		if(flags & VEH_TURNTM_FLAG) {																					//Hard left/right timeout expired
			turnWarn = true;																							//Set turn warning state variable
			OSFlagPost(&LEDDriverEvent, LED_WARN_TRN_VIOLATION, OS_OPT_POST_FLAG_SET, &err);							//Signal LED task to turn on turn warning light
			APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
		}
	}
}

void VehicleTurnTimeout(void* tmr, void* p_args) {
	RTOS_ERR err;

	OSFlagPost(&vehMonFlags, VEH_TURNTM_FLAG, OS_OPT_POST_FLAG_SET, &err);	//Vehicle turn hard left/right timeout, notify Vehicle Monitor task
	APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
}