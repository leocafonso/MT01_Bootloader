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
* File Name    : r_rspi_rx.c
* Device(s)    : RX62x, RX63x, RX210
* Tool-Chain   : Renesas RX Standard Toolchain 1.02
* OS           : None
* H/W Platform : YRDKRX62N, YRDKRX63N, RSK+RX63N, RSK+RX62N, RSKRX210, RSKRX111
* Description  : Functions for using RSPI on RX devices. Note that the pin setups done in R_RSPI_Init() are done for 
*                specific boards. Make sure that the initialization code is using the same pins as your own board, or 
*                change it to meet your needs.
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
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integers */
#include <stdint.h>
/* Boolean defines */
#include <stdbool.h>
/* Used for xchg() intrinsic */
#include <machine.h>
#include <platform.h>
/* Access to peripherals and board defines. */
#include "r_rspi_rx_if.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Number of channels of RSPI this MCU has. */
#if defined(BSP_MCU_RX62N) || defined(BSP_MCU_RX621)
#define RSPI_NUM_CHANNELS   (2)
#elif defined(BSP_MCU_RX63N) || defined(BSP_MCU_RX630) || defined(BSP_MCU_RX631)
#define RSPI_NUM_CHANNELS   (3)
#elif defined(BSP_MCU_RX63T)
    #if (BSP_CFG_MCU_PART_MEMORY_SIZE <= 0x6)
    #define RSPI_NUM_CHANNELS   (1)
    #else
    #define RSPI_NUM_CHANNELS   (2)
    #endif
#define RSPI_NUM_CHANNELS   (3)
#elif defined(BSP_MCU_RX210) || defined(BSP_MCU_RX111)
#define RSPI_NUM_CHANNELS   (1)
#else
#error  "ERROR in r_rspi_rx package! Either no MCU chosen or MCU is not supported!"
#endif

#ifndef NULL
#define NULL	0
#endif

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* These variables are only needed if locking is enabled in r_rspi_rx_config.h */
#if defined(RSPI_RX_CFG_REQUIRE_LOCK)
/* Holds the process currently using the RSPI peripheral */
static uint32_t g_rspi_locked_id[RSPI_NUM_CHANNELS] = 
{
    /* Initialize the array for up to 2 channels. Add more as needed. */
#if   RSPI_NUM_CHANNELS == 1
    NO_DEVICE_SELECTED
#elif RSPI_NUM_CHANNELS == 2
    NO_DEVICE_SELECTED, NO_DEVICE_SELECTED
#elif RSPI_NUM_CHANNELS == 3
    NO_DEVICE_SELECTED, NO_DEVICE_SELECTED, NO_DEVICE_SELECTED
#endif
};
#endif

/* This variable determines whether the peripheral has already been initialized. */
static bool rspi_initialized[RSPI_NUM_CHANNELS] = 
{
    /* Initialize the array for up to 2 channels. Add more as needed. */
#if   RSPI_NUM_CHANNELS == 1
    false
#elif RSPI_NUM_CHANNELS == 2
    false, false
#elif RSPI_NUM_CHANNELS == 3
    false, false, false
#endif
};

/* Used to prevent having duplicate code for each channel. This only works if the channels are identical (just at 
   different locations in memory). This is easy to tell by looking in iodefine.h and seeing if the same structure
   was used for all channels. */
volatile struct st_rspi __evenaccess * g_rspi_channels[RSPI_NUM_CHANNELS] =
{
/* Initialize the array for up to 2 channels. Add more as needed. */
#if   RSPI_NUM_CHANNELS == 1
    &RSPI0,
#elif RSPI_NUM_CHANNELS == 2
    &RSPI0, &RSPI1
#elif RSPI_NUM_CHANNELS == 3
    &RSPI0, &RSPI1, &RSPI2
#endif
};

static void power_on(uint32_t channel);
static void power_off(uint32_t channel);
static void rspi_set_power(uint32_t channel, bool setting);
static bool rspi_rx_buffer_full(uint32_t channel);

/***********************************************************************************************************************
* Function Name: R_RSPI_Init
* Description  : Initializes SPI channel
* Arguments    : channel -
*                    Which channel to initialize
* Return Value : true -
*                    RSPI peripheral initialized correctly
*                false -
*                    Error in initialization
***********************************************************************************************************************/
bool R_RSPI_Init(uint8_t channel)
{
    /* Check channel number. */
    if (channel >= RSPI_NUM_CHANNELS)
    {
        /* Invalid channel. */
        return false;
    }

    /* Initialize SPI channel in SPI master mode, on the 'A' pins, keep all SSLs disabled */       

    /* Check to see if the peripheral has already been initialized. */
    if (true == rspi_initialized[channel])
    {
        /* This channel has already been initialized. */
        return true;
    }

    /* All initialization that is channel specific and cannot be done through the g_rspi_channels[] array should be 
       placed here. Examples include port pins, MSTP, and interrupts. */

    power_on(channel);

    if (0 == channel)
    {
        /* Setup ICU for RSPI */
        /* Set shared IPL for RSPI0 */
        IPR(RSPI0, SPRI0) = 3;
        /* Disable receive buffer full interrupt */
        IEN(RSPI0, SPRI0) = 0;
        /* Disable transmit buffer empty interrupt */
        IEN(RSPI0, SPTI0) = 0;
        /* Clear pending receive buffer full interrupts */
        IR(RSPI0, SPRI0) = 0 ;
    }
#if RSPI_NUM_CHANNELS > 1
    else if (1 == channel)        
    {
        /* Channel 1 */
        /* Setup ICU for RSPI */
        /* Set shared IPL for RSPI1 */
        IPR(RSPI1, SPRI1) = 3;
        /* Disable receive buffer full interrupt */
        IEN(RSPI1, SPRI1) = 0;
        /* Disable transmit buffer empty interrupt */
        IEN(RSPI1, SPTI1) = 0;
        /* Clear pending receive buffer full interrupts */
        IR(RSPI1, SPRI1) = 0 ;
    }
#endif
#if RSPI_NUM_CHANNELS > 2
    else if (2 == channel)
    {
        /* Setup ICU for RSPI */
        /* Set shared IPL for RSPI2 */
        IPR(RSPI2, SPRI2) = 3;
        /* Disable receive buffer full interrupt */
        IEN(RSPI2, SPRI2) = 0;
        /* Disable transmit buffer empty interrupt */
        IEN(RSPI2, SPTI2) = 0;
        /* Clear pending receive buffer full interrupts */
        IR(RSPI2, SPRI2) = 0 ;
    }
#endif
    
    /* Set pin control register (SPPCR) */
    (*g_rspi_channels[channel]).SPPCR.BYTE = 0x00;

    /* Set RSPI bit rate (SPBR) */
    /* -Set baud rate to 8Mbps (48MHz / (2 * (2 + 1) * 2^0) ) = 8Mbps */
    (*g_rspi_channels[channel]).SPBR = 2;

    /* Set RSPI data control register (SPDCR) */
    /* -SPDR is accessed in longwords (32 bits) 
       -Transfer 1 frame at a time */
    (*g_rspi_channels[channel]).SPDCR.BYTE = 0x20;

    /* Can Set if needed */
#if 0
    /* Set RSPI clock delay registers (SPCKD) */
    (*g_rspi_channels[channel]).SPCKD.BYTE = 7;

    /* Set RSPI slave select negation delay register (SSLND) */
    (*g_rspi_channels[channel]).SSLND.BYTE = 7;

    /* Set RSPI next-access dealy register (SPND) */
    (*g_rspi_channels[channel]).SPND.BYTE = 7;
#endif

    /* Set RSPI control register 2 (SPCR2) */
    /* -Disable Idle interrupt */
    (*g_rspi_channels[channel]).SPCR2.BYTE = 0x00;

    /* Set RSPI command register 0 (SPCMD0) */
    /* -MSB first
       -8 bits data length
       -SSL0 (handled manually)
       -Use bit rate % 1       
        */       
    (*g_rspi_channels[channel]).SPCMD0.WORD = 0x0400;    
    
    /* Set RSPI control register (SPCR) */        
    /* -Clock synchronous operation (3-wire)
       -Full duplex operation
       -Master mode
       -SPTI and SPRI enabled in RSPI (have to check ICU also)
       -Enable RSPI function */
    (*g_rspi_channels[channel]).SPCR.BYTE = 0xE9;          

    /* Peripheral Initialized */
    rspi_initialized[channel] = true;

    return true;
}

/***********************************************************************************************************************
* Function Name: R_RSPI_Close
* Description  : Turns off a RSPI channel. This will put the channel into the module-stop state.
* Arguments    : channel -
*                    Which channel to close
* Return Value : true -
*                    RSPI peripheral turned off
*                false -
*                    This task did lock the RSPI fist, or invalid channel number.
***********************************************************************************************************************/
bool R_RSPI_Close (uint8_t channel, uint32_t pid)
{
    /* Check channel number. */
    if (channel >= RSPI_NUM_CHANNELS)
    {
        /* Invalid channel. */
        return false;
    }

    /* Check to see if the peripheral has already been initialized. */
    if (false == rspi_initialized[channel])
    {
        /* This channel has is already closed. */
        return true;
    }

#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    /* Verify that this task has the lock */
    if (false == R_RSPI_CheckLock(channel, pid)) 
    {
        /* This task does not have the RSPI lock and therefore cannot perform this operation. */
        return false;
    }
#endif    

    /* Power off RSPI channel. */
    power_off(channel);

    /* Peripheral closed */
    rspi_initialized[channel] = false;

    return true;
}

/***********************************************************************************************************************
* Function Name: R_RSPI_Select
* Description  : Enable device chip select on SPI channel
* Arguments    : channel -
*                    Which channel to use
*                chip_select -
*                    Which device to enable (choose from device_selected_t enum)
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
bool R_RSPI_Select(uint8_t channel, device_selected_t chip_select, uint32_t pid)
{
#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    /* Verify that this task has the lock */
    if (false == R_RSPI_CheckLock(channel, pid)) 
    {
        /* This task does not have the RSPI lock and therefore cannot perform this operation. */
        return false;
    }
#endif    

    switch ( chip_select)
    {
#if defined(SDMICRO_CS)
        case SDMICRO_SELECTED:          /* Enable the SD card */
            SDMICRO_CS = 0;
        break;
#endif
#if defined(SPIFLASH_CS)
        case FLASH_SELECTED:            /* Enable Micron flash */
        	SPIFLASH_CS = 0;
        break;
#endif
#if defined(WIFI_CS)
        case WIFI_SELECTED:             /* Enable Redpine WiFi card */
            WIFI_CS = 0;
        break;
#endif
#if defined(LCD_CS)
        case LCD_SELECTED:              /* Enable Okaya display */
            LCD_CS = 0;
        break;
#endif
        default:
        break;           
    }     

    return true;
}

/***********************************************************************************************************************
* Function Name: R_RSPI_Deselect
* Description  : Disables device chip select
* Arguments    : channel -
*                    Which channel to use
*                chip_select -
*                    Which device to disable (choose from device_selected_t enum)
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on
*                    each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
bool R_RSPI_Deselect(uint8_t channel, device_selected_t chip_select, uint32_t pid)
{
#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    /* Verify that this task has the lock */
    if (false == R_RSPI_CheckLock(channel, pid)) 
    {
        /* This task does not have the RSPI lock and therefore cannot perform this operation. */
        return false;
    }
#endif    

    switch ( chip_select)
    {
#if defined(SDMICRO_CS)
        case SDMICRO_SELECTED:          /* Disable the SD card */
            SDMICRO_CS = 1;
        break;
#endif
#if defined(SPIFLASH_CS)
        case FLASH_SELECTED:            /* Disable Micron flash */
        	SPIFLASH_CS = 1;
        break;
#endif
#if defined(WIFI_CS)
        case WIFI_SELECTED:             /* Disable Redpine WiFi card */
            WIFI_CS = 1;
        break;
#endif
#if defined(LCD_CS)
        case LCD_SELECTED:              /* Disable Okaya display */
            LCD_CS = 1;
        break;
#endif
        default:
        break;
    }     

    return true;
}

/***********************************************************************************************************************
* Function Name: R_RSPI_BaudRateSet
* Description  : Set the baud rate of the RSPI peripheral
* Arguments    : channel -
*                    Which channel to initialize
*                divisor -
*                    What divisor to use for the baud rate. The formula is:
*                    baud rate divisor = (divisor+1) * 2
*                    e.g. 0 = /2, 1 = /4, 2 = /6, 3 = /8, etc.                        
*                NOTE: Be careful with low divisors. You can set the divisor low but you still have to hit the 
*                      timing requirements.
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
bool R_RSPI_BaudRateSet(uint8_t channel, uint8_t divisor, uint32_t pid)
{
#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    /* Verify that this task has the lock */
    if (false == R_RSPI_CheckLock(channel, pid)) 
    {
        /* This task does not have the RSPI lock and therefore cannot perform this operation. */
        return false;
    }
#endif    

    /* Set RSPI bit rate (SPBR) */
    /* -Set baud rate to (48MHz / (2 * (divisor + 1) * 2^0) ) */
    (*g_rspi_channels[channel]).SPBR = divisor;

    return true;
}

/***********************************************************************************************************************
* Function Name: R_RSPI_SendReceive
* Description  : Performs SPI transfers. Can read and write at the same time.
* Arguments    : channel -
*                    Which channel to use
*                pSrc -  
*                    pointer to data buffer with data to be transmitted.
*                    If NULL, const 0xFF as source.
*                pDest - 
*                    pointer to location to put the received data (can be same as pSrc if desired). 
*                    If NULL, receive data discarded.
*                usBytes - 
*                    number of bytes to be sent/received
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
bool R_RSPI_SendReceive(uint8_t channel, 
                        uint8_t const *pSrc, 
                        uint8_t *pDest, 
                        uint16_t usBytes, 
                        uint32_t pid)
{    	
    uint16_t byte_count;  
    volatile uint32_t temp;

#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    /* Verify that this task has the lock */
    if (false == R_RSPI_CheckLock(channel, pid)) 
    {
        /* This task does not have the RSPI lock and therefore cannot perform this operation. */
        return false;
    }
#endif    

    for (byte_count = 0; byte_count < usBytes; byte_count++)
    {
        /* Ensure transmit register is empty */
        while ((*g_rspi_channels[channel]).SPSR.BIT.IDLNF) ;
        
        /* If just reading then transmit 0xFF */
        (*g_rspi_channels[channel]).SPDR.LONG = (pSrc == NULL) ? 0xFF : pSrc[byte_count];
                
        while (false == rspi_rx_buffer_full(channel))
        {
            /* Transfer is complete when a byte has been shifted in (full duplex) */
        }
        
        /* Read received data.  If transmit only, then ignore it */
        if (pDest == NULL)
        {
            temp = (*g_rspi_channels[channel]).SPDR.LONG;
        }
        else
        {
            pDest[byte_count] = (uint8_t) ((*g_rspi_channels[channel]).SPDR.LONG & 0xFF);
        }
        
    }

    return true;
}

/***********************************************************************************************************************
* Function Name: R_RSPI_Read
* Description  : Reads data using RSPI
* Arguments    : channel -
*                    Which channel to use
*                pDest - 
*                    Pointer to location to put the received data. 
*                    Returned value will be incremented by number of bytes received.
*                usBytes - 
*                    number of bytes to be received
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
bool R_RSPI_Read(uint8_t channel, 
                 uint8_t *pDest, 
                 uint16_t usBytes, 
                 uint32_t pid)
{
    uint16_t byte_count;  
    volatile uint32_t temp;

#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    /* Verify that this task has the lock */
    if (false == R_RSPI_CheckLock(channel, pid)) 
    {
        /* This task does not have the RSPI lock and therefore cannot perform this operation. */
        return false;
    }
#endif    

    for (byte_count = 0; byte_count < usBytes; byte_count++)
    {
        /* Ensure transmit register is empty */
        while ((*g_rspi_channels[channel]).SPSR.BIT.IDLNF) ;
        
        /* If just reading then transmit 0xFF */
        (*g_rspi_channels[channel]).SPDR.LONG = 0xFFFFFFFF ;
        
        while (false == rspi_rx_buffer_full(channel))
        {
            /* Transfer is complete when a byte has been shifted in (full duplex) */
        }
        
        /* Read received data.  If transmit only, then ignore it */
        pDest[byte_count] = (uint8_t) ((*g_rspi_channels[channel]).SPDR.LONG & 0xFF);        
    }  

    return true;
}

/***********************************************************************************************************************
* Function Name: R_RSPI_Write
* Description  : Write to a SPI device
* Arguments    : channel -
*                    Which channel to use
*                pSrc -  
*                    Pointer to data buffer with data to be transmitted.
*                    Returned value will be incremented by number of attempted writes.
*                usBytes - 
*                    Number of bytes to be sent
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
bool R_RSPI_Write(uint8_t channel, 
                  const uint8_t *pSrc, 
                  uint16_t usBytes, 
                  uint32_t pid)
{
    uint16_t byte_count;  
    volatile uint32_t temp;

#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    /* Verify that this task has the lock */
    if (false == R_RSPI_CheckLock(channel, pid)) 
    {
        /* This task does not have the RSPI lock and therefore cannot perform this operation. */
        return false;
    }
#endif    

    for (byte_count = 0; byte_count < usBytes; byte_count++)
    {
        /* Ensure transmit register is empty */
        while ((*g_rspi_channels[channel]).SPSR.BIT.IDLNF) ;
        
        /* If just reading then transmit 0xFF */
        (*g_rspi_channels[channel]).SPDR.LONG = pSrc[byte_count];
        
        while (false == rspi_rx_buffer_full(channel))
        {
            /* Transfer is complete when a byte has been shifted in (full duplex) */
        }
        
        /* Read received data.  If transmit only, then ignore it */
        temp = (*g_rspi_channels[channel]).SPDR.LONG;
    }

    return true;
}

/***********************************************************************************************************************
* Function Name: rspi_rx_buffer_full
* Description  : Returns whether the receive buffer full flag is set for a RSPI channel. Clear flag after read.
* Arguments    : channel -
*                    Which channel to use.
* Return Value : true -
*                    Flag is set.
*                false -
*                    Flag is not set.
***********************************************************************************************************************/
static bool rspi_rx_buffer_full (uint32_t channel)
{
    bool flag_set = false;

    /* Clear pending interrupts */
    if (0 == channel)
    {
        if (1 == IR(RSPI0, SPRI0))
        {
            /* Clear bit. */
            IR(RSPI0, SPRI0) = 0;

            flag_set = true;
        }
    }
#if RSPI_NUM_CHANNELS > 1
    else if (1 == channel)
    {
        if (1 == IR(RSPI1, SPRI1))
        {
            /* Clear bit. */
            IR(RSPI1, SPRI1) = 0;

            flag_set = true;
        }
    }
#endif
#if RSPI_NUM_CHANNELS > 2
    else 
    {
        if (1 == IR(RSPI2, SPRI2))
        {
            /* Clear bit. */
            IR(RSPI2, SPRI2) = 0;

            flag_set = true;
        }
    }
#endif

    return flag_set;
}

/***********************************************************************************************************************
* Function Name: rspi_set_power
* Description  : Configures power for RSPI channel.
* Arguments    : channel -
*                    Which channel to configure.
*                setting -
*                    Whether to power on or off. True = turn on. False = turn off.
* Return Value : none
***********************************************************************************************************************/
static void rspi_set_power (uint32_t channel, bool setting)
{
    /* Value to set for MSTP bit. */
    uint8_t value;

    if (true == setting)
    {
        value = 0;
    }
    else
    {
        value = 1;
    }

#if defined(BSP_MCU_RX63_ALL) || defined(BSP_MCU_RX21_ALL) || defined(BSP_MCU_RX11_ALL) 
    /* Enable writing to MSTP registers. */
    SYSTEM.PRCR.WORD = 0xA502;
#endif

    switch (channel)
    {
        case 0:
            MSTP(RSPI0) = value;
        break;
#if RSPI_NUM_CHANNELS > 1
        case 1:
            MSTP(RSPI1) = value;
        break;
#endif
#if RSPI_NUM_CHANNELS > 2
        case 2:
            MSTP(RSPI2) = value;
        break;
#endif
        default:
        break;
    }

#if defined(BSP_MCU_RX63_ALL) || defined(BSP_MCU_RX21_ALL) || defined(BSP_MCU_RX11_ALL) 
    /* Disable writing to MSTP registers. */
    SYSTEM.PRCR.WORD = 0xA500;
#endif
}

/***********************************************************************************************************************
* Function Name: power_on
* Description  : Powers on a RSPI channel
* Arguments    : channel -
*                    Which channel to power on.
* Return Value : none
***********************************************************************************************************************/
static void power_on (uint32_t channel)
{
    rspi_set_power(channel, true);
}

/***********************************************************************************************************************
* Function Name: power_off
* Description  : Powers off a RSPI channel
* Arguments    : channel -
*                    Which channel to power off.
* Return Value : none
***********************************************************************************************************************/
static void power_off (uint32_t channel)
{
    rspi_set_power(channel, false);
}

/***********************************************************************************************************************
* Function Name: R_RSPI_Lock
* Description  : Get lock on RSPI access.
* Arguments    : channel -
*                    Which channel to use
*                pid - 
*                    Unique program ID to attempt to lock RSPI with
* Return Value : true - 
*                    lock acquired
*                false - 
*                    lock not acquired
***********************************************************************************************************************/
bool R_RSPI_Lock(uint8_t channel, uint32_t pid)
{
#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    /* Return variable */
    bool ret;

    /* Attempt to acquire lock for this channel. */
    ret = R_BSP_HardwareLock(BSP_LOCK_RSPI0 + channel);

    /* Check to see if lock was successfully taken */
    if (ret == true)
    {
        /* Only give this pid access if no program has the reservation already */
        if (g_rspi_locked_id[channel] == NO_DEVICE_SELECTED)
        {
            /* This pid now has reserved this channel. */
            g_rspi_locked_id[channel] = pid;
        }
        else
        {
            /* Channel has already been reserved by another process. */
            ret = false;
        }

        /* Release lock. */
        R_BSP_HardwareUnlock(BSP_LOCK_RSPI0 + channel);
    }
    /* Else, lock already taken. Try again later. */

    return ret;
#else
    return true;
#endif
}

/***********************************************************************************************************************
* Function Name: R_RSPI_Unlock
* Description  : Release RSPI lock so another task can use it
* Arguments    : channel -
*                    Which channel to use
*                pid - 
*                    Unique program ID to attempt to unlock RSPI with
* Return Value : true - 
*                    lock was relinquished
*                false - 
*                    lock was not given to this ID previously
***********************************************************************************************************************/
bool R_RSPI_Unlock(uint8_t channel, uint32_t pid)
{
#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    bool ret = true;

    /* Can only release lock if this program previously acquired it */
    if ( g_rspi_locked_id[channel] == pid )
    {
        /* Lock given back successfully */
        g_rspi_locked_id[channel] = NO_DEVICE_SELECTED;
    }
    else
    {
        /* This program did not have this lock */
        ret = false;
    }

    return ret;
#else
    return true;
#endif
}


/***********************************************************************************************************************
* Function Name: R_RSPI_CheckLock
* Description  : Checks to see if PID matches the one that took the lock
* Arguments    : channel -
*                    Which channel to use
*                pid -
*                    Process ID to check against.
* Return Value : true - 
*                    This task has the lock
*                false - 
*                    This task does not have the lock
***********************************************************************************************************************/
bool R_RSPI_CheckLock(uint8_t channel, uint32_t pid)
{    
#if defined(RSPI_RX_CFG_REQUIRE_LOCK)    
    /* Check to see if this task has the lock */
    if (g_rspi_locked_id[channel] != pid) 
    {        
        /* This task does not have the lock */
        return false;
    }
    else
    {
        /* This task has the lock */
        return true;
    }
#else
    return true;
#endif
}

/***********************************************************************************************************************
* Function Name: R_RSPI_GetVersion
* Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
*                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25 
*                would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
***********************************************************************************************************************/
#pragma inline(R_CMT_GetVersion)
uint32_t R_RSPI_GetVersion (void)
{
    /* These version macros are defined in r_cmt_rx_if.h. */
    return ((((uint32_t)RSPI_RX_VERSION_MAJOR) << 16) | (uint32_t)RSPI_RX_VERSION_MINOR);
}



