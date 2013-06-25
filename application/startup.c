//*****************************************************************************
//
// startup.c - Boot code for Tek 4330 controller board.
//
//*****************************************************************************

#if defined (__cplusplus)
// The entry point for the C++ library startup
extern "C" {
	extern void __libc_init_array(void);
}
#endif

#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

// foo
// if CMSIS is being used, then SystemInit() routine
// will be called by startup code rather than in application's main()
//*****************************************************************************
// Forward declaration of the default fault handlers.
//*****************************************************************************

extern void SystemInit(void);

//*****************************************************************************
#if defined (__cplusplus)
extern "C" {
#endif

//*****************************************************************************
// main() is the entry point for Newlib based applications
//*****************************************************************************
extern int main(void);

//*****************************************************************************
// Forward declaration of the default handlers. These are aliased using the
// linker "weak" attribute:  if the application defines a handler with the 
// same name, it will take precedence over the weak definition
//*****************************************************************************
     void 	ResetISR(void);
WEAK void 	NMI_Handler(void);
WEAK void 	HardFault_Handler(void);
WEAK void 	MemManage_Handler(void);
WEAK void 	BusFault_Handler(void);
WEAK void 	UsageFault_Handler(void);
WEAK void   DebugMon_Handler(void);
WEAK void   IntDefaultHandler(void);

// FreeRTOS vs CMSIS interrupt vector definitions
extern void xPortPendSVHandler( void );
extern void PendSV_Handler(void);

extern void xPortSysTickHandler( void );
extern void SysTick_Handler(void);

extern void vPortSVCHandler( void );
extern void SVC_Handler(void);

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. Most are aliased
// to the IntDefaultHandler, which is a 'forever' loop.
//
//*****************************************************************************
void DAC_IRQHandler(void)     	  ALIAS(IntDefaultHandler);
void MX_CORE_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void DMA_IRQHandler(void)     	  ALIAS(IntDefaultHandler);
void FLASHEEPROM_IRQHandler(void) ALIAS(IntDefaultHandler);
void ETH_IRQHandler(void)  		  ALIAS(IntDefaultHandler);
void SDIO_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void LCD_IRQHandler(void)  		  ALIAS(IntDefaultHandler);
void USB0_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void USB1_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void SCT_IRQHandler(void)  		  ALIAS(IntDefaultHandler);
void RIT_IRQHandler(void)  		  ALIAS(IntDefaultHandler);
void TIMER0_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void TIMER1_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void TIMER2_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void TIMER3_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void MCPWM_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void ADC0_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void I2C0_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void I2C1_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void SPI_IRQHandler (void) 		  ALIAS(IntDefaultHandler);
void ADC1_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void SSP0_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void SSP1_IRQHandler(void) 		  ALIAS(IntDefaultHandler);
void UART0_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void UART1_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void UART2_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void UART3_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void I2S0_IRQHandler(void)  	  ALIAS(IntDefaultHandler);
void I2S1_IRQHandler(void)  	  ALIAS(IntDefaultHandler);
void SPIFI_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void SGPIO_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO0_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO1_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO2_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO3_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO4_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO5_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO6_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GPIO7_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GINT0_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void GINT1_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void EVRT_IRQHandler(void) 	 	  ALIAS(IntDefaultHandler);
void CAN1_IRQHandler(void) 	 	  ALIAS(IntDefaultHandler);
void ATIMER_IRQHandler(void) 	  ALIAS(IntDefaultHandler);
void RTC_IRQHandler(void) 	 	  ALIAS(IntDefaultHandler);
void WDT_IRQHandler(void) 	 	  ALIAS(IntDefaultHandler);
void CAN0_IRQHandler(void)   	  ALIAS(IntDefaultHandler);
void QEI_IRQHandler(void)    	  ALIAS(IntDefaultHandler);

//*****************************************************************************
// External declaration for the pointer to the stack top from the Linker Script
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
#if defined (__cplusplus)
} // extern "C"
#endif

//*****************************************************************************
// The vector table.
//*****************************************************************************

extern void (* const g_pfnVectors[])(void);

__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
	// Core Level - CM4/CM3
    // The initial stack pointer
#ifdef NOT_STELLARIS
	&_vStackTop,	                                                 // LPCXpresso usage, stack-top
#else // Stellaris
    (void (*)(void)) ((unsigned long) pulStack + sizeof(pulStack)),  // Stellaris usage,  stack-top
#endif

	ResetISR,						// The reset handler
	NMI_Handler,					// The NMI handler
	HardFault_Handler,				// The hard fault handler
	MemManage_Handler,				// The MPU fault handler
	BusFault_Handler,				// The bus fault handler
	UsageFault_Handler,				// The usage fault handler
	0,								// Reserved
	0,								// Reserved
	0,								// Reserved
	0,								// Reserved
	vPortSVCHandler,				// SVCall handler
	DebugMon_Handler,				// Debug monitor handler
	0,								// Reserved
	xPortPendSVHandler,				// The PendSV handler
	xPortSysTickHandler,			// The SysTick handler

	// Chip Level - LPC18xx/43xx
	DAC_IRQHandler,					// 16 D/A Converter
	MX_CORE_IRQHandler,				// 17 CortexM4/M0 (LPC43XX ONLY)
	DMA_IRQHandler,					// 18 General Purpose DMA
	0,								// 19 Reserved
	FLASHEEPROM_IRQHandler,			// 20 ORed flash Bank A, flash Bank B, EEPROM interrupts
	ETH_IRQHandler,					// 21 Ethernet
	SDIO_IRQHandler,				// 22 SD/MMC
	LCD_IRQHandler,					// 23 LCD
	USB0_IRQHandler,				// 24 USB0
	USB1_IRQHandler,				// 25 USB1
	SCT_IRQHandler,					// 26 State Configurable Timer
	RIT_IRQHandler,					// 27 Repetitive Interrupt Timer
	TIMER0_IRQHandler,				// 28 Timer0
	TIMER1_IRQHandler,				// 29 Timer 1
	TIMER2_IRQHandler,				// 30 Timer 2
	TIMER3_IRQHandler,				// 31 Timer 3
	MCPWM_IRQHandler,				// 32 Motor Control PWM
	ADC0_IRQHandler,				// 33 A/D Converter 0
	I2C0_IRQHandler,				// 34 I2C0
	I2C1_IRQHandler,				// 35 I2C1
	SPI_IRQHandler,					// 36 SPI (LPC43XX ONLY)
	ADC1_IRQHandler,				// 37 A/D Converter 1
	SSP0_IRQHandler,				// 38 SSP0
	SSP1_IRQHandler,				// 39 SSP1
	UART0_IRQHandler,				// 40 UART0
	UART1_IRQHandler,				// 41 UART1
	UART2_IRQHandler,				// 42 UART2
	UART3_IRQHandler,				// 43 USRT3
	I2S0_IRQHandler,				// 44 I2S0
	I2S1_IRQHandler,				// 45 I2S1
	SPIFI_IRQHandler,				// 46 SPI Flash Interface
	SGPIO_IRQHandler,				// 47 SGPIO (LPC43XX ONLY)
	GPIO0_IRQHandler,				// 48 GPIO0
	GPIO1_IRQHandler,				// 49 GPIO1
	GPIO2_IRQHandler,				// 50 GPIO2
	GPIO3_IRQHandler,				// 51 GPIO3
	GPIO4_IRQHandler,				// 52 GPIO4
	GPIO5_IRQHandler,				// 53 GPIO5
	GPIO6_IRQHandler,				// 54 GPIO6
	GPIO7_IRQHandler,				// 55 GPIO7
	GINT0_IRQHandler,				// 56 GINT0
	GINT1_IRQHandler,				// 57 GINT1
	EVRT_IRQHandler,				// 58 Event Router
	CAN1_IRQHandler,				// 59 C_CAN1
	0,								// 60 Reserved
	0,				                // 61 Reserved
	ATIMER_IRQHandler,				// 62 ATIMER
	RTC_IRQHandler,					// 63 RTC
	0,								// 64 Reserved
	WDT_IRQHandler,					// 65 WDT
	0,								// 66 Reserved
	CAN0_IRQHandler,				// 67 C_CAN0
	QEI_IRQHandler,					// 68 QEI
};

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with the lpc4330's banks of
// memory.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void data_init(unsigned int romstart, unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int *pulSrc = (unsigned int*) romstart;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors")))
void bss_init(unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = 0;
}

//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the location of various points in the "Global Section Table". This table is
// created by the linker via the Code Red managed linker script mechanism. It
// contains the load address, execution address and length of each RW data
// section and the execution and length of each BSS (zero initialized) section.
//*****************************************************************************
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

extern void SystemInit(void);

//*****************************************************************************
// Reset entry point: sets up the crt and/or the c/c++ libraries
//*****************************************************************************
void
ResetISR(void) {

    //
    // Copy the data sections from flash to SRAM.
    //
	unsigned int LoadAddr, ExeAddr, SectionLen;
	unsigned int *SectionTableAddr;

    /* Call SystemInit() for clocking/memory setup prior to scatter load */
    SystemInit();
    
	// Load base address of Global Section Table
	SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
	while (SectionTableAddr < &__data_section_table_end) {
		LoadAddr = *SectionTableAddr++;
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
		data_init(LoadAddr, ExeAddr, SectionLen);
	}
	// At this point, SectionTableAddr = &__bss_section_table;
	// Zero fill the bss segment
	while (SectionTableAddr < &__bss_section_table_end) {
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
		bss_init(ExeAddr, SectionLen);
	}

#if defined (__cplusplus)
	//
	// Call C++ library initialisation
	//
	__libc_init_array();
#endif

	// Call main()
	main();

	//
	// main() shouldn't return, but if it does, we'll just enter an infinite loop 
	//
	while (1) {
		;
	}
}

//*****************************************************************************
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void NMI_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void HardFault_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void MemManage_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void BusFault_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void UsageFault_Handler(void)
{
    while(1)
    {
    }
}

__attribute__ ((section(".after_vectors")))
void DebugMon_Handler(void)
{
    while(1)
    {
    }
}


//*****************************************************************************
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void)
{
    while(1)
    {
    }
}

