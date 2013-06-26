/*
 * Creates all the demo application tasks, then starts the scheduler.
 *    Based on the  OPENLpc Xpresso project FreeRTOS blinky
 *    in which two LED's blink, each in its own task, with different
 *    blocking intervals (blink rates), and a debug USART output
 *    that outputs once-per-second
 */

//#include <stdio.h>

// scheduler
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "tek_board_init.h"  // Board_LED_Set

// void SysInit(void);
extern void Board_Init(void);

/* welcome text */
const portCHAR *const pcWelcomeMessage = "   www.FreeRTOS.org";


// #define CMD_BUF_SIZE  128
// static char cmdBuf[CMD_BUF_SIZE];
// int status = 0;


// LED1 toggle thread
static portTASK_FUNCTION(vLEDTask1, pvParameters) {
	bool LedState = false;

	while (1) {
		Board_LED_Set(0, LedState);
		LedState = (bool) !LedState;

		// About a 3Hz toggle rate
		vTaskDelay(configTICK_RATE_HZ / 6);
	}
}

// LED2 toggle thread
static portTASK_FUNCTION(vLEDTask2, pvParameters) {
	bool LedState = false;

	while (1) {
		Board_LED_Set(1, LedState);
		LedState = (bool) !LedState;

		// About a 7Hz toggle rate
		vTaskDelay(configTICK_RATE_HZ / 14);
	}
}

// UART thread
static portTASK_FUNCTION(vUARTTask, pvParameters) {
	int tickCnt = 0;

	while (1) {
		// DEBUGOUT("Tick: %d \r\n", tickCnt);
		tickCnt++;

		// About a 1s delay
		vTaskDelay(configTICK_RATE_HZ);
	}
}

int main(void)
{
	Board_Init();
	// KBD_Init();
	Board_LED_Set(0, false); // Initial LED1 state is off
	Board_LED_Set(1, false); // Initial LED2 state is off

	// LED1 toggle thread
	xTaskCreate(vLEDTask1, (signed char *) "vTaskLed1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	// LED2 toggle thread
	xTaskCreate(vLEDTask2, (signed char *) "vTaskLed2",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	// USART output thread, simply counts seconds
	xTaskCreate(vUARTTask, (signed char *) "vTaskUart",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	// start the scheduler
	vTaskStartScheduler();

	// should never happen
	return 1;
}
