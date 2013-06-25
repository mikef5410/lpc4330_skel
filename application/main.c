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



/*-- ---------------------------------------------------------*/
/* The time between cycles of the 'check' functionality (defined within the
tick hook. */
#define mainCHECK_DELAY                                         ( ( portTickType ) 500 / portTICK_RATE_MS )


/* The OLED task uses the sprintf function so requires a little more stack too. */
#define mainOLED_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )
#define mainCMD_TASK_STACK_SIZE			        ( configMINIMAL_STACK_SIZE + 50 )
/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY			( tskIDLE_PRIORITY )
/* The maximum number of message that can be waiting for display at any one
   time. */
#define mainOLED_QUEUE_SIZE					( 3 )
/* Dimensions the buffer into which the jitter time is written. */
#define mainMAX_MSG_LEN						25
/* The period of the system clock in nano seconds.  This is used to calculate
   the jitter time in nano seconds. */
#define mainNS_PER_CLOCK					( ( unsigned portLONG ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )
/* Constants used when writing strings to the display. */
#define mainCHARACTER_HEIGHT				( 9 )
#define mainMAX_ROWS_128					( mainCHARACTER_HEIGHT * 14 )
#define mainMAX_ROWS_96						( mainCHARACTER_HEIGHT * 10 )
#define mainMAX_ROWS_64						( mainCHARACTER_HEIGHT * 7 )
#define mainFULL_SCALE						( 15 )
#define ulSSI_FREQUENCY						( 3500000UL )
/*-----------------------------------------------------------*/
/*
 * The display is written two by more than one task so is controlled by a
 * 'gatekeeper' task.  This is the only task that is actually permitted to
 * access the display directly.  Other tasks wanting to display a message send
 * the message to the gatekeeper.
 */
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
  SysInit();
  Board_Init();
#if 0
    UARTprintf("I'm here\n");
    /* Create the queue used by the OLED task.  Messages for display on the OLED
       are received via this queue. */
    xOLEDQueue = xQueueCreate(mainOLED_QUEUE_SIZE, sizeof(xOLEDMessage));

    /* Start the tasks defined within this file/specific to this demo. */
    xTaskCreate(vOLEDTask, (signed portCHAR *) "OLED",
		mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);


    xTaskCreate(vCMDTask, (signed portCHAR *) "CMD",
                mainCMD_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

    /* Configure the high frequency interrupt used to measure the interrupt
       jitter time. */
    vSetupHighFrequencyTimer();

    /* Start the scheduler. */
    vTaskStartScheduler();

    
    /* Will only get here if there was insufficient memory to create the idle
       task. */
    for (;;);
#endif        
    return 0;
}

/*-----------------------------------------------------------*/

void prvSetupHardware(void)
{
    unsigned long dummy;
    /* If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
       a workaround to allow the PLL to operate reliably. */
    if (DEVICE_IS_REVA2) {
	SysCtlLDOSet(SYSCTL_LDO_2_75V);
    }

    /* Set the clocking to run from the PLL at 50 MHz */
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
		       SYSCTL_XTAL_8MHZ);
   //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure SysTick for a 100Hz interrupt.  The FatFs driver wants a 10 ms
    // tick.
    //
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / 100);
    ROM_SysTickEnable();
    ROM_SysTickIntEnable();

    //
    // Enable Interrupts
    //
    IntMasterEnable();


    //
    // Set GPIO A0 and A1 as UART.
    //
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART as a console for text I/O.
    //
    UARTStdioInit(0);
  
    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;
    // Do a dummy read to insert a few cycles after enabling the peripheral.
    //
    dummy = SYSCTL_RCGC2_R;

    //
    // Enable the GPIO pin for the LED (PF0).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIO_PORTF_DIR_R = 0x01;
    GPIO_PORTF_DEN_R = 0x01;

}

/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
    static xOLEDMessage xMessage = { "PASS" };
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
            GPIO_PORTF_DATA_R |= 0x1;
            on=0;
        } else {
            GPIO_PORTF_DATA_R &= ~(0x1);
            on=1;
        }
    }
}

/*-----------------------------------------------------------*/
void vCMDTask(void *pvParameters)
{
    while(1) {
        UARTprintf("\n> ");
        UARTgets(cmdBuf, sizeof(cmdBuf));
        status = CmdLineProcess(cmdBuf);
        if (status == CMDLINE_BAD_CMD) {
            UARTprintf("Bad command\n");
        }
    }
    return;
}


void vOLEDTask(void *pvParameters)
{
    xOLEDMessage xMessage;
    unsigned portLONG ulY, ulMaxY;
    static portCHAR cMessage[mainMAX_MSG_LEN];
    extern volatile unsigned portLONG ulMaxJitter;
    unsigned portBASE_TYPE uxUnusedStackOnEntry, uxUnusedStackNow;
    const unsigned portCHAR *pucImage;

/* Functions to access the OLED.  The one used depends on the dev kit
   being used. */
    void (*vOLEDInit) (unsigned portLONG) = NULL;
    void (*vOLEDStringDraw) (const portCHAR *, unsigned portLONG,
			     unsigned portLONG, unsigned portCHAR) = NULL;
    void (*vOLEDImageDraw) (const unsigned portCHAR *, unsigned portLONG,
			    unsigned portLONG, unsigned portLONG,
			    unsigned portLONG) = NULL;
    void (*vOLEDClear) (void) = NULL;

    /* Just for demo purposes. */
    uxUnusedStackOnEntry = uxTaskGetStackHighWaterMark(NULL);

    /* Map the OLED access functions to the driver functions that are appropriate
       for the evaluation kit being used. */
    switch (HWREG(SYSCTL_DID1) & SYSCTL_DID1_PRTNO_MASK) {
#if 0
    case SYSCTL_DID1_PRTNO_6965:
    case SYSCTL_DID1_PRTNO_2965:
	vOLEDInit = OSRAM128x64x4Init;
	vOLEDStringDraw = OSRAM128x64x4StringDraw;
	vOLEDImageDraw = OSRAM128x64x4ImageDraw;
	vOLEDClear = OSRAM128x64x4Clear;
	ulMaxY = mainMAX_ROWS_64;
	pucImage = pucBasicBitmap;
	break;
#endif
    default:
	vOLEDInit = vFormike128x128x16Init;
	vOLEDStringDraw = vFormike128x128x16StringDraw;
	vOLEDImageDraw = vFormike128x128x16ImageDraw;
	vOLEDClear = vFormike128x128x16Clear;
	ulMaxY = mainMAX_ROWS_128;
	pucImage = pucGrLibBitmap;
	break;
    }

    ulY = ulMaxY;

    /* Initialise the OLED and display a startup message. */
    vOLEDInit(ulSSI_FREQUENCY);
    vOLEDStringDraw("POWERED BY FreeRTOS", 0, 0, mainFULL_SCALE);
    vOLEDImageDraw(pucImage, 0, mainCHARACTER_HEIGHT + 1, bmpBITMAP_WIDTH,
		   bmpBITMAP_HEIGHT);

    for (;;) {
	/* Wait for a message to arrive that requires displaying. */
	xQueueReceive(xOLEDQueue, &xMessage, portMAX_DELAY);

	/* Write the message on the next available row. */
	ulY += mainCHARACTER_HEIGHT;
	if (ulY >= ulMaxY) {
	    ulY = mainCHARACTER_HEIGHT;
	    vOLEDClear();
	    vOLEDStringDraw(pcWelcomeMessage, 0, 0, mainFULL_SCALE);
	}

	/* Display the message along with the maximum jitter time from the
	   high priority time test. */
	sprintf(cMessage, "%s ", xMessage.pcMessage);
	vOLEDStringDraw(cMessage, 0, ulY, mainFULL_SCALE);
    }
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(xTaskHandle * pxTask,
				   signed portCHAR * pcTaskName)
{
    for (;;);
}




//*****************************************************************************
//
// This function implements the "help" command.  It prints a simple list of the
// available commands with a brief description.
//
//*****************************************************************************
int Cmd_help(int argc, char *argv[])
{
    tCmdLineEntry *pEntry;

    //
    // Print some header text.
    //
    UARTprintf("\nAvailable commands\n");
    UARTprintf("------------------\n");

    //
    // Point at the beginning of the command table.
    //
    pEntry = &g_sCmdTable[0];

    //
    // Enter a loop to read each entry from the command table.  The end of the
    // table has been reached when the command name is NULL.
    //
    while (pEntry->pcCmd) {
	//
	// Print the command name and the brief description.
	//
	UARTprintf("%s%s\n", pEntry->pcCmd, pEntry->pcHelp);

	//
	// Advance to the next entry in the table.
	//
	pEntry++;
    }
    //
    // Return success.
    //
    return (0);
}


//*****************************************************************************
//
// This is the table that holds the command names, implementing functions, and
// brief description.
//
//*****************************************************************************

tCmdLineEntry g_sCmdTable[] = {
    {"help", Cmd_help, " : Display list of commands"}
    ,
    {"h", Cmd_help, "    : alias for help"}
    ,
    {"?", Cmd_help, "    : alias for help"}
    ,
    {0, 0, 0}
};
