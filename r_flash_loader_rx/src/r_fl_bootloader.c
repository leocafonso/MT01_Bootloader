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
* File Name    : r_fl_bootloader.c
* Version      : 3.00
* Description  : Bootloader to run out of User Boot Area.  Part of the
*                FlashLoader project.
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 08.04.2010 1.01    Added bypass checking and added User
*                              Boot reset vector
*         : 22.03.2011 2.00    First Release for YRDK
*         : 02.03.2012 3.00    Made CS v4.0 compliant. Now uses r_cmt_rx 
*                              package to trigger the FL state machine. Fixed
*                              bugs found in fl_process_write_buffer() 
*                              function.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines intrinsic functions of the MCU */
#include <machine.h>
#include <platform.h>

/* Prototype for memcpy() */
#include <string.h>
/* Used for fixed-width types. */
#include <stdint.h>
/* Used for booleans. */
#include <stdbool.h>
/* r_bsp Support. */
#include "r_fl_includes.h"
/* Function prototypes for erasing/programming MCU flash */
#include "r_flash_api_rx_if.h"
/* Create timer to call Flash Loader state machine. */
#include "r_cmt_rx_if.h"
/* Used for CRC calculations. */
#include "r_crc_rx_if.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* Reset vector */
#define MCU_RESET_VECTOR        (0xFFFFFFFC)
#define JUMP_TO_APPLICATION     ((void (*)(void))*((uint32_t *)MCU_RESET_VECTOR))


extern uint8_t fl_app_buffer[4096];
fl_image_header_t   g_fl_load_image_headers[FL_CFG_MEM_NUM_LOAD_IMAGES];

/******************************************************************************
Private global variables and functions
******************************************************************************/
static bool fl_write_new_image(uint8_t image_index);
static bool fl_process_write_buffer(uint32_t address, uint8_t * data, uint32_t bytes);
static bool fl_flush_write_buffer(void);
static void fl_trigger_sm(void * pdata);

/* Points to info on current application */
volatile fl_image_header_t * g_pfl_cur_app_header;


/******************************************************************************
* Function Name: main
* Description  : The main loop for the bootloader. This function will check to
*                see if any new images are available. If a valid image is found
*                then it will be programmed in to the User Application space.
*                It will then jump to the newly programmed application. If a 
*                image is not found then the function will start the 
*                Downloader state machine and wait for a new image.
* Arguments    : none
* Return Value : none
******************************************************************************/
void main(void)
{    
    /* Index of load image to use */
    int32_t  image_to_load;

	/* Initialize to -1, no valid image */
	image_to_load = -1;

	/* Initialize resources needed for using external memory */
	fl_mem_init();

	/* Initialize CRC code. */
	R_CRC_Init();

	/* Initialize pointer to current app's load image header */
	g_pfl_cur_app_header = (fl_image_header_t *)__sectop("APPHEADER_1");

	/* Get info on any load images that are stored.  Put this data in
	   g_fl_load_image_headers[] */
	fl_get_load_image_headers();

	/* Get last downloaded image to use */
	image_to_load = fl_get_latest_image();

	/* Was a valid load image header found? */
	if(image_to_load == -1)
	{
		/* No valid image found in external memory */
		/* Check to see if valid image header is already in MCU flash */
		if(g_pfl_cur_app_header->valid_mask == FL_LI_VALID_MASK)
		{
			/* Valid image header in MCU flash, validate the whole image */
			if( fl_check_application() == g_pfl_cur_app_header->raw_crc )
			{
				/* Valid image in MCU flash, jump to it */
				JUMP_TO_APPLICATION();
			}
			/* Else, the image was not successfully validated, wait for new
			   load image */
		}
	}
	else
	{
		/* Check to see if there is a current image in MCU flash */
		if( (g_pfl_cur_app_header->valid_mask == FL_LI_VALID_MASK) &&
			(g_pfl_cur_app_header->version_major == g_fl_load_image_headers[image_to_load].version_major) &&
			(g_pfl_cur_app_header->version_middle == g_fl_load_image_headers[image_to_load].version_middle) &&
			(g_pfl_cur_app_header->version_minor == g_fl_load_image_headers[image_to_load].version_minor) &&
			(g_pfl_cur_app_header->version_comp == g_fl_load_image_headers[image_to_load].version_comp) )
		{
			/* Valid image was found but it is same as the one already in flash.
			   Check to make sure image in MCU flash is valid, if so then jump
			   to it.  If not, then program in the load image. */
			if( fl_check_application() == g_pfl_cur_app_header->raw_crc )
			{
				/* Valid image in MCU flash, jump to it */
				JUMP_TO_APPLICATION();
			}
		}

		/* Verify load image is complete and error free */
		if( fl_verify_load_image((uint32_t)image_to_load) == g_fl_load_image_headers[image_to_load].raw_crc )
		{
			/* Load image is valid, program in new image */
			fl_write_new_image((uint8_t)image_to_load);
		}

		/* Verify image in MCU flash and jump to it */
		if( fl_check_application() == g_pfl_cur_app_header->raw_crc )
		{
			/* Valid image in MCU flash, jump to it */
			JUMP_TO_APPLICATION();
		}
	}
    
    while(1) 
    {

    }       
}

/******************************************************************************
* Function Name: fl_write_new_image
* Description  : Write load image into MCU flash.
* Arguments    : image_index - 
*                    Which load image to use
* Return value : true - 
*                    Image programmed successfully
*                false - 
*                    Error occurred
******************************************************************************/
static bool fl_write_new_image(uint8_t image_index)
{
    uint32_t i;
    bool check;
    uint8_t ret;
    uint32_t address = 0x00F00000;
    uint32_t spiaddress = 0;
    
#ifdef FLASH_API_RX_CFG_COPY_CODE_BY_API
    /* Before calling any other Flash API functions the API code needs to be 
       copied to RAM. This can still be done the 'old way' by editting dbsct.c
       if desired. */
    R_FlashCodeCopy();
#endif
    
    /* Start off by erasing flash */
    for(i = 0; i < ROM_NUM_BLOCKS; i++)
    {
        /* Erase Block 'i' */
        ret = R_FlashErase((uint8_t)i);
        
        /* Check for errors */
        if( ret != FLASH_SUCCESS )
        {
            return false;
        }
    }
    
    /* Now we can program flash */
    while( address < 0x01000000)
    {
    	fl_mem_read(spiaddress,fl_app_buffer,sizeof(fl_app_buffer));

        /* Write buffer */
    	ret = R_FlashWrite( address,
                            (uint32_t)&fl_app_buffer[0],
							sizeof(fl_app_buffer));

        /* Check for errors */
        if( ret != FLASH_SUCCESS )
        {
            return false;
        } ;
        address += sizeof(fl_app_buffer);
        spiaddress += sizeof(fl_app_buffer);
    }
    
    return true;
}
/******************************************************************************
End of function fl_write_new_image
******************************************************************************/
