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
* File Name    : r_delay_config.h
* Description  : Configures this code
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 07.11.2012 1.00    First Release     
***********************************************************************************************************************/
#ifndef DELAY_CONFIG_HEADER_FILE
#define DELAY_CONFIG_HEADER_FILE

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* This macro determines whether this module can use hardware resources (e.g. CMT timer) to implement delays. Using 
   hardware timers is more accurate, but in some cases all timers may be needed by the user. 
   If this definition is set to 0 (default) then hardware resources will be used if available.
   If this definition is set to 1 then no hardware resources will be used and only software delays will be implemented.   
*/
#define DELAY_CFG_SOFTWARE_ONLY_ENABLE      (0)

/* This macro can be used in the event that the user wishes to implement their own delay routines. To do this, the user 
   needs to set the definition for this macro to the name of the function they wish to be called. For example, if the 
   user wants the function my_delay() to be called, then they would define this macro as:
   #define DELAY_CFG_USER_DELAY_FUNCTION my_delay
   NOTE: The same parameters that are sent to the R_DELAY_Wait() function will also be sent to this user defined 
         function.
   If this definition is defined, then the function that is defined shall be called when the user calls the function 
   R_DELAY_Wait(). 
   If this definition is not defined (default), then no user-defined function shall be called and delays will be 
   implemented inside of this module.
*/
//#define DELAY_CFG_USER_DELAY_FUNCTION       (my_delay)

#endif /* DELAY_CONFIG_HEADER_FILE */


