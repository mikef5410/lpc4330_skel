/*
 * Creates all the demo application tasks, then starts the scheduler.
 *
 * "OLED" task - the OLED task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the OLED send the message on a queue to the OLED task instead of
 * accessing the OLED themselves.  The OLED task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" hook -  This only executes every five seconds from the tick hook.
 * Its main function is to check that all the standard demo tasks are still
 * operational.  Should any unexpected behaviour within a demo task be discovered
 * the tick hook will write an error to the OLED (via the OLED task).  If all thqe
 * demo tasks are executing with their expected behaviour then the check task
 * writes PASS to the OLED (again via the OLED task), as described above.
 *
 */

/* Standard includes. */
//#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


void vLEDTask(void *pvParameters);
/*-- ---------------------------------------------------------*/
/* The time between cycles of the 'check' functionality (defined within the
tick hook. */
#define mainCHECK_DELAY             ( ( portTickType ) 500 / portTICK_RATE_MS )


/* The OLED task uses the sprintf function so requires a little more stack too. */
#define mainLED_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE + 50 )
#define mainCMD_TASK_STACK_SIZE	        ( configMINIMAL_STACK_SIZE + 50 )
/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY			( tskIDLE_PRIORITY )
/* The maximum number of message that can be waiting for display at any one
   time. */
#define mainLED_QUEUE_SIZE					( 3 )
/* Dimensions the buffer into which the jitter time is written. */
#define mainMAX_MSG_LEN						25
/* The period of the system clock in nano seconds.  This is used to calculate
   the jitter time in nano seconds. */
#define mainNS_PER_CLOCK ( ( unsigned portLONG ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )

/*-----------------------------------------------------------*/

/*
 * Configure the hardware for the demo.
 */
void SysInit(void);
void Board_Init(void);
/*
 * Configures the high frequency timers - those used to measure the timing
 * jitter while the real time kernel is executing.
 */
extern void vSetupHighFrequencyTimer(void);

/*
 * The idle hook is used to run a test of the scheduler context switch
 * mechanism.
 */
void vApplicationIdleHook(void) __attribute__ ((naked));
/*-----------------------------------------------------------*/

/* The queue used to send messages to the OLED task. */
xQueueHandle xOLEDQueue;

/* The welcome text. */
const portCHAR *const pcWelcomeMessage = "   www.FreeRTOS.org";

/* Variables used to detect the test in the idle hook failing. */
unsigned portLONG ulIdleError = pdFALSE;

/*-----------------------------------------------------------*/

#define CMD_BUF_SIZE            128
static char cmdBuf[CMD_BUF_SIZE];
int status = 0;
/*************************************************************************
 * Please ensure to read http://www.freertos.org/portLM3Sxxxx_Eclipse.html
 * which provides information on configuring and running this demo for the
 * various Luminary Micro EKs.
 *************************************************************************/
int main(void)
{
  SystemInit();
  Board_Init();

    UARTprintf("I'm here\n");

    /* Start the tasks defined within this file/specific to this demo. */
    xTaskCreate(vLEDTask, (signed portCHAR *) "LED",
		mainLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

    /* Configure the high frequency interrupt used to measure the interrupt
       jitter time. */
    vSetupHighFrequencyTimer();

    /* Start the scheduler. */
    vTaskStartScheduler();

    
    /* Will only get here if there was insufficient memory to create the idle
       task. */
    for (;;);

    return 0;
}

/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
    static unsigned portLONG ulTicksSinceLastDisplay = 0;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    static unsigned long on;

    /* Called from every tick interrupt.  Have enough ticks passed to make it
       time to perform our health status check again? */
    ulTicksSinceLastDisplay++;
    if (ulTicksSinceLastDisplay >= mainCHECK_DELAY) {
	ulTicksSinceLastDisplay = 0;

	/* Send the message to the OLED gatekeeper for display. */
	xHigherPriorityTaskWoken = pdFALSE;
        //printf("From vApplicationTickHook\n");
	//xQueueSendFromISR(xOLEDQueue, &xMessage,
	//		  &xHigherPriorityTaskWoken);

        //Toggle the LED
        if (on) {
            //GPIO_PORTF_DATA_R |= 0x1;
            on=0;
        } else {
            //GPIO_PORTF_DATA_R &= ~(0x1);
            on=1;
        }
    }
}

//Called when idle
void vApplicationIdleHook( void ) 
{
  return;
}


void vLEDTask(void *pvParameters)
{
  int count=0;
  while (1) {
    count++;
  }
  return;
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(xTaskHandle * pxTask,
				   signed portCHAR * pcTaskName)
{
    for (;;);
}

