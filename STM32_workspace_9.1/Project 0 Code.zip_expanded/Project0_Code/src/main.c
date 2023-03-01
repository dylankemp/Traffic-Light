/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"



/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 100

#define ADC_Port			GPIOC

#define Traffic_Red_Pin  	GPIO_Pin_0
#define Traffic_Yellow_Pin  GPIO_Pin_1
#define Traffic_Green_Pin  	GPIO_Pin_2

#define SHIFT1_REG_Port		GPIOC
#define SHIFT1_REG_PIN		GPIO_Pin_6
#define SHIFT1_CLK_PIN		GPIO_Pin_7

//#define SHIFT2_REG_Port		GPIOC
//#define SHIFT2_REG_PIN		GPIO_PIN_6
//#define SHIFT2_CLK_PIN		GPIO_PIN_7
//
//#define SHIFT3_REG_Port		GPIOC
//#define SHIFT3_REG_PIN		GPIO_PIN_6
//#define SHIFT3_CLK_PIN		GPIO_PIN_7
//
//#define SHIFT1_PIN_2	GPIO_PIN_6
//#define SHIFT1_PIN_3	GPIO_PIN_6
//#define SHIFT1_PIN_4	GPIO_PIN_6
//#define SHIFT1_PIN_5	GPIO_PIN_6
//#define SHIFT1_PIN_6	GPIO_PIN_6
//#define SHIFT1_PIN_7	GPIO_PIN_6
//#define SHIFT1_PIN_8	GPIO_PIN_6

#define Prio_Task_Traffic_Flow		( tskIDLE_PRIORITY + 1 )
#define Prio_Task_Traffic_Create	( tskIDLE_PRIORITY + 2 )
#define Prio_Task_Traffic_Light		( tskIDLE_PRIORITY + 2 )
#define Prio_Task_Traffic_Display	( tskIDLE_PRIORITY )

// Can't use global variables

// Initialize global variables
// Global light color 0: red, 1: green
uint16_t global_flowrate;
uint16_t global_light_color;
uint16_t global_car_value;

#define LED_CLK_C RCC_AHB1Periph_GPIOC


#define amber_led	LED3
#define green_led	LED4
#define red_led		LED5
#define blue_led	LED6



SemaphoreHandle_t		xMutexFlow;
SemaphoreHandle_t		xMutexTrafficLight;
SemaphoreHandle_t		xMutexCarPosition;


/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
void prvSetupHardware( void );
void ShiftRegisterValuePreLight( uint16_t value );
//static void Traffic_Flow_Task( void *pvParameters);
//static void Traffic_Generator_Task( void *pvParameters);
//static void Traffic_Light_State_Task( void *pvParameters);
static void System_Display_Task( void *pvParameters);


/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */
//static void Manager_Task( void *pvParameters );
//static void Blue_LED_Controller_Task( void *pvParameters );
//static void Green_LED_Controller_Task( void *pvParameters );
//static void Red_LED_Controller_Task( void *pvParameters );
//static void Amber_LED_Controller_Task( void *pvParameters );

xQueueHandle xQueue_handle = 0;


/*-----------------------------------------------------------*/

struct TRAFFIC_Struct {
	uint16_t flow;
	uint16_t car;
	uint16_t light_state;

}

int main(void)
{

	/* Initialize LEDs */
//	STM_EVAL_LEDInit(amber_led);
//	STM_EVAL_LEDInit(green_led);
//	STM_EVAL_LEDInit(red_led);
//	STM_EVAL_LEDInit(blue_led);
	struct TRAFFIC_Struct *TRAFFIC_Init;

	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */
	prvSetupHardware();

	xQueue_handle = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint16_t));

	// Create mutexes to access all the global variables
	xMutexFlow = xSemaphoreCreateMutex();
	if( xMutexFlow == NULL) {
		printf("ERROR: No Semaphore\n");
	}
	else {
		xSemaphoreGive( xMutexFlow);
	}

	xMutexTrafficLight = xSemaphoreCreateMutex();
	if( xMutexTrafficLight == NULL) {
		printf("ERROR: No Semaphore\n");
	}
	else {
		xSemaphoreGive( xMutexTrafficLight);
	}

	xMutexCarPosition = xSemaphoreCreateMutex();
	if( xMutexCarPosition == NULL) {
		printf("ERROR: No Semaphore\n");
	}
	else {
		xSemaphoreGive( xMutexCarPosition);
	}


//	xTaskCreate( Traffic_Flow_Task, "Flow", configMINIMAL_STACK_SIZE, NULL, Prio_Task_Traffic_Flow, NULL);
//	xTaskCreate( Traffic_Generator_Task, "Generator", configMINIMAL_STACK_SIZE, NULL, Prio_Task_Traffic_Create, NULL);
	xTaskCreate( Traffic_Light_State_Task, "Light-State", configMINIMAL_STACK_SIZE, NULL, Prio_Task_Traffic_Light, NULL);
	xTaskCreate( System_Display_Task, "Display", configMINIMAL_STACK_SIZE, NULL, Prio_Task_Traffic_Display, NULL);
	
	// Create timers for the traffic light
	Red_Light_TIMER = xTimerCreate( "Red_Light_Timer", 5000 / portTICK_PERIOD_MS, pdFALSE, ( void * ) 0, Red_Timer_Callback);
	Yellow_Light_TIMER = xTimerCreate( "Yellow_Light_Timer", 2000 / portTICK_PERIOD_MS, pdFALSE, ( void * ) 0, Yellow_Timer_Callback);
	Green_Light_TIMER = xTimerCreate( "Green_Light_Timer", 10000 / portTICK_PERIOD_MS, pdFALSE, ( void * ) 0, Green_Timer_Callback);
	

	//uint16_t newactiveprelighttraffic[8] = {0};
	while (1)
	{

	//reset(Clear)
	GPIO_ResetBits(GPIOC, GPIO_Pin_8);
	for (int i=0; i<10; i++){

	}
	GPIO_SetBits(GPIOC, GPIO_Pin_8);

	uint32_t newactiveprelighttrafficoff[24] = {0};
	uint16_t car_value = 0;

	for (uint16_t i=0; i<24; i++){
		newactiveprelighttrafficoff[i] = car_value;
	}

	for (uint16_t i=23; i>0; i--){
		ShiftRegisterValuePreLight(newactiveprelighttrafficoff[i]);

		for(int i = 0; i>10; i++)
		{}
	}

	uint32_t newactiveprelighttraffic[24] = {0};
	car_value = 1;

	for (uint16_t i=0; i<24; i++){
		newactiveprelighttraffic[i] = car_value;
	}

	for (uint16_t i=23; i>0; i--){
		ShiftRegisterValuePreLight(newactiveprelighttraffic[i]);

		for(int i = 0; i>10; i++)
		{}
	}

	// Start with green traffic light
	GPIO_SetBits(GPIOC, Traffic_Green_Pin);
	xTimerStart( Green_Light_TIMER, 0);
	TRAFFIC_Init->light_state = 1;


	}
	return 0;
}

// Traffic Light Task
/*-----------------------------------------------------------*/

void Red_Timer_Callback( xTimerHandle xTimer) {
	// Red light off, green light on
	GPIO_ResetBits( GPIOC, Traffic_Red_Pin);
	GPIO_SetBits( GPIOC, Traffic_Green_Pin);

	// Use semaphore to update global light color to green
	if( xSemaphoreTake( xMutexTrafficLight, (TickType_t) 0) == pdTrue ) {
		
		global_light_color = 1;
		xSemaphoreGive( xMutexTrafficLight);
	}
	else {
		printf("Semaphore not available \n");
	}

	xTimerStart( Green_Light_TIMER, 0);


}

void Green_Timer_Callback( xTimerHandle xTimer) {
	// Green light off, yellow light on
	GPIO_ResetBits( GPIOC, Traffic_Green_Pin);
	GPIO_SetBits( GPIOC, Traffic_Yellow_Pin);

	// Use semaphore to update global light color to red through yellow
	if( xSemaphoreTake( xMutexTrafficLight, (TickType_t) 0) == pdTrue ) {
		global_light_color = 0;
		xSemaphoreGive( xMutexTrafficLight);
	}
	else {
		printf("Semaphore not available \n");
	}

	xTimerStart( Yellow_Light_TIMER, 0);
}

void Yellow_Timer_Callback( xTimerHandle xTimer) {
	// Red light off, green light on
	GPIO_ResetBits( GPIOC, Traffic_Yellow_Pin);
	GPIO_SetBits( GPIOC, Traffic_Red_Pin);

	xTimerStart( Red_Light_TIMER, 0);
}

void Traffic_Light_State_Task( void *pvParameters) {

	uint16_t current_speed = 4;
	uint16_t new_speed = 0;

	while(1) {

		vTaskDelay(500);

		if( xSemaphoreTake( xMutexFlow, (TickType_t) 10) == pdTrue) {
			new_speed = global_flowrate;
		}

		else {
			printf("Semaphore not available \n");
		}

		if(current_speed != new_speed) {
			if(xTimerIsTimerActive( Green_Light_TIMER)) {
				xTimerStop(Green_Light_TIMER, 0);
				xTimerChangePeriod(Green_Light_TIMER, (5000 + 2000 * (8 - new_speed)) / portTICK_PERIOD_MS, 0);
				xTimerChangePeriod(Red_Light_TIMER, (2000 + 1500 * (8 - new_speed)) / portTICK_PERIOD_MS, 0);
				xTimerStop(Red_Light_TIMER, 0);
			}

			else if(xTimerIsTimerActive( Green_Light_TIMER)) {
				xTimerChangePeriod(Green_Light_TIMER, (5000 + 2000 * (8 - new_speed)) / portTICK_PERIOD_MS, 0);
				xTimerStop(Green_Light_TIMER, 0);
				xTimerChangePeriod(Red_Light_TIMER, (2000 + 1500 * (8 - new_speed)) / portTICK_PERIOD_MS, 0);
				xTimerStop(Red_Light_TIMER, 0);
			}

			else if(xTimerIsTimerActive( Green_Light_TIMER)) {
				xTimerStop(Red_Light_TIMER, 0);
				xTimerChangePeriod(Green_Light_TIMER, (5000 + 2000 * (8 - new_speed)) / portTICK_PERIOD_MS, 0);
				xTimerStop(Green_Light_TIMER, 0);
				xTimerChangePeriod(Red_Light_TIMER, (2000 + 1500 * (8 - new_speed)) / portTICK_PERIOD_MS, 0);
			} 
		}

		current_speed = new_speed;

	}
}

/*-----------------------------------------------------------*/

// System Display Task
/*-----------------------------------------------------------*/

void System_Display_Task( void *pvParameters) {
	uint16_t car_value;
	struct TRAFFIC_Struct *TRAFFIC_Received;
	uint16_t light_color = 1;
	uint16_t currentactiveprelighttraffic[8] = {0};
	uint16_t newactiveprelighttraffic[8] = {0};

	while(1){

		vTaskDelay(500);

		if(xSemaphoreTake( xMutexCarPosition, ( TickType_t) 10) == pdTRUE) {
			
			if( xQueueReceive(xQueue_handle, &(TRAFFIC_Received), 500)) {
				car_value = TRAFFIC_Received->car;
				xSemaphoreGive( xMutexCarPosition);
				printf("DisplayTask: Accessed xMutexCars, updated local car_value:  %u. \n", car_value );
			}
			

		}
		else {
			printf("Mutex Car Unavailable \n");
		}

		if(xSemaphoreTake( xMutexTrafficLight, ( TickType_t) 0) == pdTRUE) {

			if( xQueueReceive(xQueue_handle, &(TRAFFIC_Received), 500)) {
				light_color = TRAFFIC_Received->light_state;
				xSemaphoreGive( xMutexTrafficLight);
				printf("DisplayTask: Accessed:  %u. \n", light_color );
			}

		}
		else {
			printf("Mutex Light Unavailable \n");
		}

		if( light_color == 1) {
//			Green light
			ShiftRegisterValuePreLight(car_value);

			newactiveprelighttraffic[0] = car_value;

			for (uint16_t i = 1; i < 8; i++) {
				newactiveprelighttraffic[i] = currentactiveprelighttraffic[i - 1];
			}
		}
		else if (light_color == 0) {
//			Red light

			uint16_t emptyQueue = 0;

			for (uint16_t i = 7; i > 0; i--) {

				if(currentactiveprelighttraffic[i] == 0){
					emptyQueue = 1;
					newactiveprelighttraffic[0] = car_value;
				}

				if(emptyQueue == 1) {
					newactiveprelighttraffic[i] = currentactiveprelighttraffic[i -1];
				}
				else {
					newactiveprelighttraffic[i] = currentactiveprelighttraffic[i];
				}
			}

			for (uint16_t i = 7; i >= 0; i--) {
				ShiftRegisterValuePreLight(newactiveprelighttraffic[i]);
			}
		}

		for (uint16_t i = 0; i < 8; i++) {
			currentactiveprelighttraffic[i] = newactiveprelighttraffic[i];
		}
	}
}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

void prvSetupHardware(void)
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	GPIO_InitTypeDef  GPIO_Init_Shift_1;
	//GPIO_InitTypeDef  GPIO_Init_Shift_2;
	//GPIO_InitTypeDef  GPIO_Init_Shift_3;
	//GPIO_InitTypeDef  GPIO_Init_Traffic;

	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(LED_CLK_C, ENABLE);
	//RCC_AHB1PeriphClockCmd(LED_CLK_A, ENABLE);

	GPIO_Init_Shift_1.GPIO_Pin = SHIFT1_REG_PIN | SHIFT1_CLK_PIN | GPIO_Pin_8;
	GPIO_Init_Shift_1.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init_Shift_1.GPIO_OType = GPIO_OType_PP;
	GPIO_Init_Shift_1.GPIO_PuPd = GPIO_PuPd_NOPULL;
	//GPIO_Init_Shift_1.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_Init(SHIFT1_REG_Port, &GPIO_Init_Shift_1);

//	GPIO_Init_Shift_2.GPIO_Pin = SHIFT2_REG_PIN | SHIFT2_CLK_PIN;
//	GPIO_Init_Shift_2.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_Init_Shift_2.GPIO_OType = GPIO_OType_PP;
//	GPIO_Init_Shift_2.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	//GPIO_Init_Shift_2.GPIO_Speed = GPIO_Speed_25MHz;
//	GPIO_Init(SHIFT2_REG_Port, &GPIO_Init_Shift_2);
//
//	GPIO_Init_Shift_3.GPIO_Pin = SHIFT3_REG_PIN | SHIFT3_CLK_PIN;
//	GPIO_Init_Shift_3.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_Init_Shift_3.GPIO_OType = GPIO_OType_PP;
//	GPIO_Init_Shift_3.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	//GPIO_Init_Shift_3.GPIO_Speed = GPIO_Speed_25MHz;
//	GPIO_Init(SHIFT3_REG_Port, &GPIO_Init_Shift_3);


	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

void ShiftRegisterValuePreLight(uint16_t value)
{
	if (value == 0){
		GPIO_ResetBits(GPIOC, SHIFT1_REG_PIN);

	}else{
		GPIO_SetBits(GPIOC, SHIFT1_REG_PIN);
	}
	GPIO_SetBits(GPIOC, SHIFT1_CLK_PIN);
	GPIO_ResetBits(GPIOC, SHIFT1_CLK_PIN);
}
