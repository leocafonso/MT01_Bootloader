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
* File Name    : r_bsp.h
* H/W Platform : RDKRX63N 
* Description  : Has the header files that should be included for this platform.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 13.01.2012 1.00     First Release
*         : 12.03.2012 1.10     Added locking.h and r_bsp_config.h #includes. Removed sbrk.h since heap size is not
*                               defined in r_bsp_config.h.
*         : 26.06.2012 1.20     Relocated mcu_info.h to the 'mcu' folder.
*         : 16.07.2012 1.30     Added vecttbl.h include. 
*         : 24.09.2012 1.40     Added test for C99 compliant compiler. Updated includes for v1.0 spec.
*         : 17.01.2013 1.50     Added #include for lowsrc.h.
***********************************************************************************************************************/

#ifndef BSP_BOARD_RDKRX63N
#define BSP_BOARD_RDKRX63N

/* Make sure that no other platforms have already been defined. Do not touch this! */
#ifdef  PLATFORM_DEFINED
#error  "Error - Multiple platforms defined in platform.h!"
#else
#define PLATFORM_DEFINED
#endif

/***********************************************************************************************************************
INCLUDE APPROPRIATE MCU AND BOARD FILES
***********************************************************************************************************************/
#if __STDC_VERSION__ >= 199901L                  
#include    <stdint.h>
#include    <stdbool.h>
#else
#include    "mcu/all/r_typedefs.h"
#endif

#include    "r_bsp_config.h"
#include    "mcu/rx63n/register_access/iodefine.h"
#include    "mcu/rx63n/mcu_info.h"
#include    "mcu/rx63n/mcu_locks.h"
#include    "mcu/rx63n/locking.h"
#include    "board/rdkrx63n/yrdkrx63n.h"
#include    "board/rdkrx63n/hwsetup.h"
#include    "board/rdkrx63n/lowsrc.h"
#include    "board/rdkrx63n/vecttbl.h"

#endif /* BSP_BOARD_RDKRX63N */


