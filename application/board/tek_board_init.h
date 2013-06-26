/*
 * NOTE: jjones
 *       copied from board_ngx_xplorer_18304330.[ch]
 *       and renamed             tek_board_init.[ch]
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __TEK_BOARD_INIT_H
#define __TEK_BOARD_INIT_H

#include "chip.h"
#include "board_api.h"
// #include "lpc_phy.h"
// #include "uda1380.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * tek_board_init board turn-on and basic support functions
 * 
 */


/*---------------------------------------------------------------------------------------------------------------------+
| GPIO pin used for LEDs 1 and 2
+---------------------------------------------------------------------------------------------------------------------*/

/// GPIO ports to which blink-LED's are connected
/// Tek gen-1 controller: orange LED1 is on P6_6, pin 83 -> GPIO-0, pin 5
/// Tek gen-1 controller: red    LED2 is on P6_7, pin 85 -> GPIO-5, pin 15
enum { BLINK_LED1_GPIO = 0 };
enum { BLINK_LED2_GPIO = 5 };

/// pin numbers of LED1, LED2
enum { BLINK_LED1_pin = 5 };
enum { BLINK_LED2_pin = 15 };

/// LED's pin masks
enum { BLINK_LED1 = 1 << BLINK_LED1_pin };
enum { BLINK_LED2 = 1 << BLINK_LED2_pin };

/// bit-band "variable" to directly handle the pins
#define BLINK_LED1_bb BITBAND(&LPC_GPIO_PORT->PIN[BLINK_LED1_GPIO], BLINK_LED1_pin)
#define BLINK_LED2_bb BITBAND(&LPC_GPIO_PORT->PIN[BLINK_LED2_GPIO], BLINK_LED2_pin)


// Keyboard/front-panel GPIO usage/pin maps, taken from schematic:
//    Initial version:  GPIO pins used to read/set/toggle 
//    Later version:    SCT  pins used, PWM-style, to manage color 
//                      by setting the tri-color LED duty cycle.
//                      This will require the SCU-function to be 
//                      changed to 1, to select the state-control clock (SCT)
//
// I am putting the information for both use-cases here as
// it is  esoteric and  buried in the user-manual, table 128
// ---------------------------------------------------------
// Tek     LWFP144 GPIO GPIO   SCFP  SCU-FCN  SCU-FCN CTOUT
// Symbol   pin    Num  Out    Port  for GPIO for SCT Num
// ---------------------------------------------------------
// POWSW     75    	5    0     P2_0     4      n/a    n/a
// ENABLESW  81    	5    1     P2_1     4      n/a    n/a
//                 	                                     
// PLEDR     98    	5    7     P2_8     4       1      0
// PLEDG     96    	0    7     P2_7     0       1      1
// PLEDB    104    	0    14    P2_10    0       1      2
//                 	                                     
// ALEDR    102    	1    10    P2_9     0       1      3
// ALEDG    106    	1    12    P2_12    0       1      4
// ALEDB    105    	1    11    P2_11    0       1      5
//                 	                                     
// ELEDR     82    	3     4    P6_5     0       1      6
// ELEDG    103    	2     8    P6_12    0       1      7
// ELEDB    140    	3    15    P7_7     0       1      8

// --------- KPS => Keyboard Power Switch (input)
#define KPS_GPIO           5
#define KPS_PIN            0
#define KPS_SCU_GPIOFUNC   4

// --------- KES => Keyboard Enable Switch (input)
#define KES_GPIO           5
#define KES_PIN            1
#define KES_SCU_GPIOFUNC   4

// --------- KPL => Keyboard Power LEDs (output)
#define KPL_R_GPIO         5
#define KPL_G_GPIO         0
#define KPL_B_GPIO         0
#define KPL_B_SCU_GPIOFUNC 4
#define KPL_R_PIN          7
#define KPL_G_PIN          7
#define KPL_B_PIN         14

// --------- KAL => Keyboard Attention LEDs
#define KAL_R_GPIO         1
#define KAL_G_GPIO         1
#define KAL_B_GPIO         1
#define KAL_R_PIN         10
#define KAL_G_PIN         12
#define KAL_B_PIN         11

// --------- KEL => Keyboard Enable LEDs
#define KEL_R_GPIO         3
#define KEL_G_GPIO         2
#define KEL_B_GPIO         3
#define KEL_R_PIN          4
#define KEL_G_PIN          8
#define KEL_B_PIN         15


/** @defgroup BOARD_NGX_XPLORER_1830_OPTIONS BOARD: LPC4330 NGX Xplorer board options
 * This board has options that configure its operation at build-time.
 *
 *
 */

#define BOARD_NGX_XPLORER_18304330

#define I2CDEV_UDA1380_ADDR     (0x34 >> 1)
#define UDA1380_I2C_BUS          I2C0

/* For USBLIB examples */
#define LEDS_LED1           0x01
#define LEDS_LED2           0x02
#define LEDS_LED3           0x04
#define LEDS_LED4           0x08
#define LEDS_NO_LEDS        0x00
#define BUTTONS_BUTTON1     0x01
#define JOY_UP              0x01
#define JOY_DOWN            0x02
#define JOY_LEFT            0x04
#define JOY_RIGHT           0x08
#define JOY_PRESS           0x10
#define NO_BUTTON_PRESSED   0x00

/*Define if use SDCARD for Mass Storage Example*/
//#define CFG_SDCARD

#define BUTTONS_BUTTON1_GPIO_PORT_NUM   0
#define BUTTONS_BUTTON1_GPIO_BIT_NUM    7
#define LED1_GPIO_PORT_NUM              1
#define LED1_GPIO_BIT_NUM               11
#define LED2_GPIO_PORT_NUM              1
#define LED2_GPIO_BIT_NUM               12

/**
 * @brief	Sets up board specific I2C interface
 * @param	id	: I2C Peripheral ID (I2C0, I2C1)
 * @return	Nothing
 */
void Board_I2C_Init(I2C_ID_T id);

/**
 * @brief	Sets up I2C Fast Plus mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @note	This function must be called before calling
 *      	Chip_I2C_SetClockRate() to set clock rates above
 *      	normal range 100KHz to 400KHz. Only I2C0 supports
 *       	this mode.
 */
STATIC INLINE void Board_I2C_EnableFastPlus(I2C_ID_T id)
{
	Chip_SCU_I2C0PinConfig(I2C0_FAST_MODE_PLUS);
}

/**
 * @brief	Disable I2C Fast Plus mode and enables default mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @sa		Board_I2C_EnableFastPlus()
 */
STATIC INLINE void Board_I2C_DisableFastPlus(I2C_ID_T id)
{
	Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
}

/**
 * @brief	Initializes board specific GPIO Interrupt
 * @return	Nothing
 */
void Board_GPIO_Int_Init(void);

/**
 * @brief	Initialize pin muxing for SSP interface
 * @param	pSSP	: Pointer to SSP interface to initialize
 * @return	Nothing
 */
void Board_SSP_Init(LPC_SSP_T *pSSP);

/**
 * @brief	Returns the MAC address assigned to this board
 * @param	mcaddr : Pointer to 6-byte character array to populate with MAC address
 * @return	Nothing
 */
void Board_ENET_GetMacADDR(uint8_t *mcaddr);

/**
 * @brief	Initialize pin muxing for a UART
 * @param	pUART	: Pointer to UART register block for UART pins to init
 * @return	Nothing
 */
void Board_UART_Init(LPC_USART_T *pUART);

/**
 * @brief	Initialize pin muxing for SDMMC interface
 * @return	Nothing
 */
void Board_SDMMC_Init(void);

/**
 * @brief	Initialize button(s) interface on board
 * @return	Nothing
 */
void Board_Buttons_Init(void);

/**
 * @brief	Initialize joystick interface on board
 * @return	Nothing
 */
void Board_Joystick_Init(void);

/**
 * @brief	Returns joystick states on board
 * @return	Returns a JOY_* value, ir JOY_PRESS or JOY_UP
 */
uint8_t Joystick_GetStatus(void);

/**
 * @brief	Returns button(s) state on board
 * @return	Returns BUTTONS_BUTTON1 if button1 is pressed
 */
uint32_t Buttons_GetStatus (void);

/**
 * @brief	Initialize I2S interface for the board and UDA1380
 * @param	pI2S	: Pointer to I2S register interface used on this board
 * @param	micIn	: If 1 MIC will be used as input, if 0 LINE_IN will be used
 * @return	Nothing
 */
void Board_Audio_Init(LPC_I2S_T *pI2S, int micIn);

/**
 * @brief	FIXME
 * @param	Stream	: FIXME
 * @return	Nothing
 */
void Serial_CreateStream(void *Stream);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TEK_BOARD_INIT_H */
