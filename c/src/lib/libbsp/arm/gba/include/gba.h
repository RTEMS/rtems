/**
 *  @file gba.h
 *
 *  This include file contains definitions related to the ARM BSP.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _GBA_H
#define _GBA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gba_registers.h>

/*---------------------------------------------------------------------------*
 * Clock                                                                     *
 *---------------------------------------------------------------------------*/
#define  __ClockFrequency   16780000L    /**< ClockFreguency in Hz           */
#define  __TimPreScaler     1024L        /**< Prescaler value 1,64,256,1024  */

/*---------------------------------------------------------------------------*
 * Activation defines                                                        *
 *---------------------------------------------------------------------------*/
/**
 * Multiboot/Cart boot Selection.
 *
 * If the variable __gba_multiboot is defined
 * (probably should be in your main project file) then code
 * is generated which will run as a multiboot image (code starts
 * at 0x02000000) or as a normal flash cart / emulator image.
 * (i.e. start.S copies code from ROM to EWRAM if started in cart
 * or emulator.) If this variable is not defined then code is
 * generated for flash cart / emulator only operation (code starts
 * at 0x08000000).
 */
#define MULTIBOOT volatile const unsigned short __gba_multiboot;

/**
 * IWRAM/EWRAM data Selection.
 *
 * If the variable __gba_iwram_data is defined
 * (probably should be in your main project file) then code
 * is generated which will allocate data section in IWRAM
 * (data starts at 0x03000080).
 * If this variable is not defined then data sections is
 * allocated in EWRAM (data starts at 0x02000000).
 */
#define IWRAMDATA volatile const unsigned short __gba_iwram_data;

/**
 * IWRAM/EWRAM bss Selection.
 *
 * If the variable __gba_iwram_bss is defined
 * (probably should be in your main project file) then code
 * is generated which will allocate bss section in IWRAM
 * (data starts at 0x03000080).
 * If this variable is not defined then bss sections is
 * allocated in EWRAM (data starts at 0x02000000).
 */
#define IWRAMBSS volatile const unsigned short __gba_iwram_bss;


/*---------------------------------------------------------------------------*
 *  Attributes                                                               *
 *---------------------------------------------------------------------------*/
#define CODE_IN_ROM   __attribute__ ((section (".text"), long_call))
#define CODE_IN_IWRAM __attribute__ ((section (".iwram"), long_call))
#define IN_IWRAM      __attribute__ ((section (".iwram")))
#define IN_EWRAM      __attribute__ ((section (".ewram")))

/*---------------------------------------------------------------------------*
 * Console Keypad                                                            *
 *---------------------------------------------------------------------------*/
#define OK_KEY (0==0)  /**< TRUE  */
#define NO_KEY (0==1)  /**< FALSE */
#define GBA_KEYS_PRESSED(keys) (( ((~GBA_REG_P1) & (keys)) == (keys)) ? (OK_KEY) : (NO_KEY))
#define GBA_ANY_KEY(keys)      (( ((~GBA_REG_P1) & (keys)) != 0) ? (OK_KEY) : (NO_KEY))
#define GBA_KEY()              ((~GBA_REG_P1)&GBA_KEY_ALL)

/*---------------------------------------------------------------------------*
 * Console Screen                                                            *
 *---------------------------------------------------------------------------*/
#define GBA_LCD_WIDTH           240
#define GBA_LCD_HEIGHT          160
#define GBA_MODE5_WIDTH         160
#define GBA_MODE5_HEIGHT        128
#define GBA_LCD_ASPECT          ((float)(GBA_LCD_WIDTH / GBA_LCD_HEIGHT))


#ifdef __cplusplus
}
#endif

#endif
