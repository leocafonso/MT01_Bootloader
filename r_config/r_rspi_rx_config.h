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
* File Name     : r_rspi_rx_config.c
* Description   : Configures the RSPI drivers
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
***********************************************************************************************************************/
#ifndef RSPI_CONFIG_HEADER_FILE
#define RSPI_CONFIG_HEADER_FILE

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* If this definition is uncommented then obtaining a lock on the RSPI peripheral using the R_RSPI_Lock() function will 
   be required. After obtaining the lock the user will have to pass the tasks PID to all other RSPI functions. If this 
   is commented out then locking will not be enabled and the user can pass any value they want for 'pid' because the 
   functions will ignore the lock. */
//#define RSPI_RX_CFG_REQUIRE_LOCK

#endif /* RSPI_CONFIG_HEADER_FILE */
