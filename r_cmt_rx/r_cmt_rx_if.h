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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name	   : r_cmt_rx_if.h
* Description  : This module creates a timer tick using a CMT channel.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description               
*         : 29.02.2012 1.00    First Release.      
*         : 10.05.2012 1.10    Updated to be compliant with FIT Module Spec v0.7
*         : 01.06.2012 1.20    Updated so that channel allocation is done by the module and is dynamic. To do this the
*                              API did have to change.
*         : 07.11.2012 1.30    Updated to be compliant with FIT Module Spec v1.00. Added R_CMT_CreateOneShot() function.                               
*         : 14.12.2012 1.40    Added support for RX111. Completed updates to FIT 1.0 spec to support hardware locking.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed-size integer typedefs. */
#include <stdint.h>
/* bool support. */
#include <stdbool.h>

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Version Number of API. */
#define CMT_RX_VERSION_MAJOR            (1)
#define CMT_RX_VERSION_MINOR            (30)

/* This define is used with the R_CMT_Control() function if not channel needs to input. */
#define CMT_RX_NO_CHANNEL               (0xFFFFFFFF)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Commands that can be used with R_CMT_Control() function. */
typedef enum
{
    CMT_RX_CMD_IS_CHANNEL_COUNTING = 0,    //Used for determining if a particular CMT channel is currently being used
    CMT_RX_CMD_GET_NUM_CHANNELS            //Used for getting number of CMT channels on this MCU
} cmt_commands_t;

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
bool R_CMT_CreatePeriodic(uint32_t frequency_hz, void (* callback)(void * pdata), uint32_t * channel);
bool R_CMT_CreateOneShot(uint32_t period_us, void (* callback)(void * pdata), uint32_t * channel);
bool R_CMT_Control(uint32_t channel, cmt_commands_t command, void * pdata);
bool R_CMT_Stop(uint32_t channel);


