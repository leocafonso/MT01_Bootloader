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
* File Name    : r_rspi_rx_if.h
* Description  : Functions for using RSPI on RX devices. 
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 03.08.2011 1.00    First Release            
*         : 30.11.2011 1.10    Added RX63x support
*         : 27.02.2012 1.20    Moved pin setup out of here and into the r_bsp package. Added preprocessor guards for 
*                              boards that do not use all slave selects.
*         : 08.03.2012 1.30    Renamed r_rspi_rx.h to r_rspi_rx_if.h to be compliant with updated FIT spec. Also
*                              added GetVersion() function (though it's really a macro). Updated Select() and Deselect()
*                              functions to take 'device_selected_t' as type for chip select since these are the 
*                              devices supported. Updated Lock() function so that it uses the locking mechanism in 
*                              the r_bsp package.
*          : 20.04.2012 1.40   Added RX210 support. Changed name of module from r_rspi_rx to r_rspi_rx since RX2xx is
*                              now supported.
*          : 14.12.2012 1.50   Added RX111 support. Changed locking scheme to FIT 1.0 specification. Requires FIT 1.0
*                              or later BSP. Refactored code for ease of future upgrades.
*          : 01.03.2013 1.60   Added R_RSPI_Close() function.
***********************************************************************************************************************/

#ifndef RSPI_API_HEADER_FILE
#define RSPI_API_HEADER_FILE

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integer support. */
#include <stdint.h>
/* bool support */
#include <stdbool.h>
/* Used for configuring the RSPI code */
#include "r_rspi_rx_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Version Number of API. */
#define RSPI_RX_VERSION_MAJOR           (1)
#define RSPI_RX_VERSION_MINOR           (60)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Enumeration for common chip selects. */
typedef enum 
{
    NO_DEVICE_SELECTED=0,
    SDMICRO_SELECTED,
    FLASH_SELECTED,
    WIFI_SELECTED,
    LCD_SELECTED
} device_selected_t;

/***********************************************************************************************************************
Public Functions
***********************************************************************************************************************/
bool R_RSPI_Init(uint8_t channel);
bool R_RSPI_Select(uint8_t channel, device_selected_t chip_select, uint32_t pid);
bool R_RSPI_Deselect(uint8_t channel, device_selected_t chip_select, uint32_t pid);
bool R_RSPI_BaudRateSet(uint8_t channel, uint8_t divisor, uint32_t pid);
bool R_RSPI_SendReceive(uint8_t channel, uint8_t const *pSrc, uint8_t *pDest, uint16_t usBytes, uint32_t pid);
bool R_RSPI_Read(uint8_t channel, uint8_t *pDest, uint16_t usBytes, uint32_t pid);
bool R_RSPI_Write(uint8_t channel, const uint8_t *pSrc, uint16_t usBytes, uint32_t pid);
bool R_RSPI_Close(uint8_t channel, uint32_t pid);

/* If RSPI_REQUIRE_LOCK is not defined then these always return true. */
bool R_RSPI_Lock(uint8_t channel, uint32_t pid);
bool R_RSPI_Unlock(uint8_t channel, uint32_t pid);
bool R_RSPI_CheckLock(uint8_t channel, uint32_t pid);

#endif /* RSPI_API_HEADER_FILE */

