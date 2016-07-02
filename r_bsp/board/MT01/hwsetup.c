/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : hwsetup.c
* Device(s)    : RX
* H/W Platform : YRDKRX63N
* Description  : Defines the initialization routines used each time the MCU is restarted.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.10.2011 1.00     First Release
*         : 12.02.2013 1.01     Bug fix: Wifi-CS is PJ_3 not PJ_2
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
/* I/O Register and board definitions */


#include "../../platform.h"

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* MCU I/O port configuration function delcaration */
static void output_ports_configure(void);

/* Interrupt configuration function delcaration */
static void interrupts_configure(void);

/* MCU peripheral module configuration function declaration */
static void peripheral_modules_enable(void);


/***********************************************************************************************************************
* Function name: hardware_setup
* Description  : Contains setup functions called at device restart
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void hardware_setup(void)
{
    output_ports_configure();
    interrupts_configure();
    peripheral_modules_enable();
}

/***********************************************************************************************************************
* Function name: output_ports_configure
* Description  : Configures the port and pin direction settings, and sets the pin outputs to a safe level.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void output_ports_configure(void)
{
    SYSTEM.PRCR.WORD = 0xA50B;          /* Protect off */
    MPC.PWPR.BIT.B0WI = 0 ;             /* Unlock protection register */
    MPC.PWPR.BIT.PFSWE = 1 ;            /* Unlock MPC registers */
    
	/* Port 0 - not used*/
	PORT0.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORT0.PDR.BYTE  = 0xFF ;    /* all  are outputs */

	/* Port 1 -USB VBUS, USB Overcurrent Plasma - Limites (IRQ2) */
	PORT1.PMR.BYTE = 0x54;
	MPC.P12PFS.BYTE = 0x40; /* IRQ2 -  Limites*/
	MPC.P14PFS.BYTE = 0x12; /* USB0_OVRCURA */
	MPC.P16PFS.BYTE = 0x12; /* USB0_VBUS */
	PORT1.PODR.BYTE = 0x00 ; /* All unused pins  outputs low to start */
	PORT1.PDR.BYTE  = 0xFF ;  /* All  unused pins are outputs */

	/* Port 2 - Plasma - Emergencia (IRQ8), Arco OK (IRQ9), PWM Chp (P22), Tocha (P23)  */
	PORT2.PMR.BYTE = 0x03;
	MPC.P20PFS.BYTE = 0x40; /* IRQ8 - Emergencia */
	MPC.P21PFS.BYTE = 0x40; /* IRQ9 - Arco Ok */
	PORT2.PODR.BYTE = 0x08 ;    /* All outputs low to start except TORCH*/
	PORT2.PDR.BYTE  = 0xFE ;    /* All outputs - Emergencia (Input)  */

	/* Port 3 - JTAG, DA0(MTIOC0C), DA1(MTIOC0D) */
	PORT3.PMR.BYTE = 0x0C;
	MPC.P32PFS.BYTE = 0x01; /* MTIOC0C - DA0 */
	MPC.P33PFS.BYTE = 0x01; /* MTIOC0D - DA1 */
	PORT3.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORT3.PDR.BYTE = 0x0C;

	/* Port 4 - TLINHA0-2 (P44 - 46),AN0 (AN001), AN1(AN002), THC Voltage (AN003) */
	PORT4.PMR.BYTE = 0x0E;
	MPC.P41PFS.BYTE = 0x80;  /* AN001 - AN0 */
	MPC.P42PFS.BYTE = 0x80;  /* AN002 - AN1 */
	MPC.P43PFS.BYTE = 0x80;  /* AN003 - THC Voltage */
	PORT4.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORT4.PDR.BYTE  = 0x8F ;    /* TLINHA0-2 (P44 - 46) - inputs, All outputs  */
	PORT4.PCR.BYTE  = 0x70 ;	 /* Pull up */

	/* Port 5 - OUT0-3 (P50 - 53) */
	PORT5.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORT5.PDR.BYTE  = 0xFF ;    /* All outputs */

	/* Port A -   */
	PORTA.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORTA.PDR.BYTE  = 0xFF ;    /* All outputs */

	//    /* Port B - IN0-4 (PB1 - B5) */
	PORTB.PODR.BYTE = 0x00 ;
	PORTB.PDR.BYTE = 0x01; /* IN0-4 - inputs, All outputs  */

	/* Port C -  LCD SPI signals, LCD CS (PC2)*/
	PORTC.PMR.BYTE  = 0x60 ;    /*  */
	MPC.PC5PFS.BYTE = 0x0D ;    /* PC5 is RSPCKA */
	MPC.PC6PFS.BYTE = 0x0D ;    /* PC6 is MOSIA */
	PORTC.PODR.BYTE = 0x00 ;    /* All outputs low to start */
	PORTC.PDR.BYTE  = 0xFF ;    /* All outputs*/

	/* Port D -  TCOL0-2 (P44 - 46) */
	PORTD.PODR.BYTE = 0x00 ;    /* All outputs low to start  	*/
	PORTD.PDR.BYTE  = 0xFF ;    /* All outputs*/

	/* Port E -  CNC signals*/
    PORTE.PMR.BYTE  = 0x0E ;    /* All GPIO for now */
    MPC.PE1PFS.BYTE = 0x0E ;    /* PE1 is RSPCKB */
    MPC.PE2PFS.BYTE = 0x0E ;    /* PE2 is MOSIB */
    MPC.PE3PFS.BYTE = 0x0D ;    /* PE3 is MISOB */
	PORTE.PODR.BYTE = 0x00 ;    /* All outputs low to start  */
	PORTE.PDR.BYTE  = 0xFF ;    /* All outputs*/

	/* Port J -  No used	*/
	PORTJ.PODR.BYTE = 0x00 ;    /* All outputs low to start  */
	PORTJ.PDR.BYTE  = 0xFF ;    /* All output */
}

/***********************************************************************************************************************
* Function name: interrupts_configure
* Description  : Configures interrupts used
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void interrupts_configure(void)
{
    /* Add code here to setup additional interrupts */
}

/***********************************************************************************************************************
* Function name: peripheral_modules_enable
* Description  : Enables and configures peripheral devices on the MCU
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void peripheral_modules_enable(void)
{
    /* Add code here to enable peripherals used by the application */
}
