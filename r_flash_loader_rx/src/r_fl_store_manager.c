/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* File Name    : r_fl_store_manager.c
* Version      : 3.00
* Description  : High level memory management is done here.  This file will
*                not change unless you want to change file operations.
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 23.02.2012 3.00    Made compliant with CS v4.0. Fixed bug in 
*                              fl_store_block_init() function when data member
*                              spanned programming pages.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines standard macros used in this file */
#include <stddef.h>
/* Defines intrinsic functions of the MCU */
#include <machine.h>
/* Fixed width types support. */
#include <stdint.h>
/* Used for bool. */
#include <stdbool.h>

/* Flash Loader project includes. */
#include "r_fl_includes.h"
/* Uses r_crc_rx package for CRC calculations. */
#include "r_crc_rx_if.h"

#define CRC_ADDRESS (((uint32_t)__sectop("APPHEADER_1"))-0xFFF00000)

/******************************************************************************
Private global variables and functions
******************************************************************************/


extern volatile    uint16_t calc_crc;


/******************************************************************************
* Function Name: fl_get_latest_image
* Description  : Returns index for the last downloaded load image
* Arguments    : none
* Return value : -1 -
*                    No load image found
*                >0 -
*                    The index of the latest load image
******************************************************************************/
int32_t fl_get_latest_image(void)
{
    uint32_t i;
    int32_t  image_to_load;

    /* Initialize values. */
    image_to_load = -1;

    for(i = 0; i < FL_CFG_MEM_NUM_LOAD_IMAGES; i++)
    {
        if(g_fl_load_image_headers[i].valid_mask == FL_LI_VALID_MASK)
        {
                image_to_load = i;
        }
    }

    return image_to_load;
}
/******************************************************************************
End of function fl_get_latest_image
******************************************************************************/

/******************************************************************************
* Function Name: fl_get_load_image_headers
* Description  : Obtains Load Image Header if available from external memory
* Arguments    : none
* Return value : none
******************************************************************************/
void fl_get_load_image_headers(void)
{
    uint32_t i;

    /* Get load image headers from external memory */
    for(i = 0; i < FL_CFG_MEM_NUM_LOAD_IMAGES; i++)
    {
        fl_mem_read(g_fl_li_mem_info.addresses[i]+CRC_ADDRESS, (uint8_t *)&g_fl_load_image_headers[i], sizeof(fl_image_header_t));
    }
}
/******************************************************************************
End of function fl_get_load_image_headers
******************************************************************************/


/******************************************************************************
* Function Name: fl_verify_load_image
* Description  : Verifies that a load image is complete and error free
* Arguments    : image_index - 
*                    Which load image block to check
* Return value : true - 
*                    Image is complete and correct
*                false - 
*                    Image has errors
******************************************************************************/
uint8_t fl_app_buffer[4096];

uint16_t fl_verify_load_image(uint32_t image_index)
{
	 uint16_t calc_crc;
	 uint32_t sizeapp;
	 uint32_t start_address;

    /* Get lowest flash address. ROM_PE_ADDR is the lowest address with the
       MSB set to 0x00. To get the read address just make the MSB 0xFF. */
    start_address = 0;
    sizeapp = sizeof(fl_app_buffer);
	fl_mem_read(start_address,fl_app_buffer,sizeapp);
	R_CRC_Compute( RX_LINKER_SEED,
				   (uint8_t *) fl_app_buffer,
				   sizeapp,
				   &calc_crc);
	start_address += sizeof(fl_app_buffer);
    while(start_address < 0xFF000)
    {
		fl_mem_read(start_address,fl_app_buffer,sizeof(fl_app_buffer));
		/* Calculate CRC up to the location where the linker put the CRC value */
		R_CRC_Compute( calc_crc,
					   (uint8_t *) fl_app_buffer,
					   sizeof(fl_app_buffer),
					   &calc_crc);
		start_address += sizeof(fl_app_buffer);
    }
    sizeapp = CRC_ADDRESS-start_address + \
            offsetof(fl_image_header_t, raw_crc);
	fl_mem_read(start_address,fl_app_buffer,sizeapp);

	R_CRC_Compute( calc_crc,
				   (uint8_t *) fl_app_buffer,
				   sizeapp,
				   &calc_crc);
    /* Move start_address to right after 'raw_crc' */
    start_address = CRC_ADDRESS + \
                    offsetof(fl_image_header_t, raw_crc) + \
                    sizeof(((fl_image_header_t *) 0)->raw_crc);
    sizeapp = (0x000FFFFF - start_address) + 1;
	fl_mem_read(start_address,fl_app_buffer,sizeapp);
    /* Calculate the rest of flash after the CRC in memory */
    R_CRC_Compute( calc_crc,
                   (uint8_t *) fl_app_buffer,
				   sizeapp,
                   &calc_crc);

    /* The RX linker does a bitwise NOT on the data after the
       CRC has finished */
    calc_crc = (uint16_t)(~calc_crc);

    return calc_crc;
}
/******************************************************************************
End of function fl_verify_load_image
******************************************************************************/



