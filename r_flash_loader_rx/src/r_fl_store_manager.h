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
* File Name    : r_fl_store_manager.h
* Version      : 3.00
* Description  : High level memory management is done here.  This file will
*                not change unless you want to change file operations.
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 23.02.2012 3.00    Removed 'FL_SM_ROM_PROGRAM_SIZE' macro because
*                              this is defined in Flash API. Removed function
*                              headers from prototypes because it was only
*                              a duplication.
******************************************************************************/

#ifndef FL_STORE_H
#define FL_STORE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Fixed width types support. */
#include <stdint.h>
/* Used for bool. */
#include <stdbool.h>
/* Flash Loader types. */
#include "r_fl_types.h"

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
bool    fl_store_init(void);
void    fl_store_image_header(fl_image_header_t *fl_image_pointer);
bool    fl_start_erase_load_block(uint8_t block);
bool    fl_continue_erase_load_block(void);
bool    fl_is_store_busy(void);
void    fl_store_retry_init(uint32_t image_index);
bool    fl_store_retry_continue(uint16_t * seq_id);
bool    fl_store_retry_get_block(uint16_t * seq_id);
void    fl_store_block_init(uint8_t * data_addr, uint32_t data_size);
bool    fl_store_block_continue(void);
void    fl_store_finish(void);
void    fl_get_load_image_headers(void);
int32_t fl_find_matching_image(fl_image_header_t * ptr);
uint16_t fl_verify_load_image(uint32_t image_index);
int32_t fl_get_latest_image(void);

#endif /* FL_STORE_H */
