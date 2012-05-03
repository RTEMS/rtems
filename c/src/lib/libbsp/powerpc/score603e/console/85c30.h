/*  85c30.h
 *
 *  This include file contains z85c30 chip information.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __85c30_H
#define __85c30_H

/*
 * Clock Speed Definations
 */

#define Z8530_x1_CLOCK                        0x00
#define Z8530_x16_CLOCK                       0x40
#define Z8530_x32_CLOCK                       0x80
#define Z8530_x64_CLOCK                       0xC0

/*
 * Number of Stop Bits.
 */
#define Z8530_STOP_BITS_1                     0x04
#define Z8530_STOP_BITS_1_AND_A_HALF          0x08
#define Z8530_STOP_BITS_2                     0x0C

/*
 * PARITY
 */
#define Z8530_PARITY_NONE                     0x00
#define Z8530_PARITY_ODD                      0x01
#define Z8530_PARITY_EVEN                     0x03

/*
 * Character Bits
 */
#define Z8530_READ_CHARACTER_BITS_8           0xC0
#define Z8530_READ_CHARACTER_BITS_7           0x40
#define Z8530_READ_CHARACTER_BITS_6           0x80
#define Z8530_READ_CHARACTER_BITS_5           0x00

#define Z8530_WRITE_CHARACTER_BITS_8          0x60
#define Z8530_WRITE_CHARACTER_BITS_7          0x20
#define Z8530_WRITE_CHARACTER_BITS_6          0x40
#define Z8530_WRITE_CHARACTER_BITS_5          0x00

#endif
