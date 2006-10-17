/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains definitions for the M93Cxx EEPROM devices    |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       m93cxx.h                                            */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS M93C64-based header file                      */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  M93C46 is a serial microwire EEPROM which contains  */
/*                 1Kbit (128 bytes/64 words) of non-volatile memory.  */
/*                 The device can be configured for byte- or word-     */
/*                 access. The driver provides a file-like interface   */
/*                 to this memory.                                     */
/*                                                                     */
/*                 MPC5x00 PIN settings:                               */
/*                                                                     */
/*                 PSC3_6 (output) -> MC93C46 serial data in    (D)    */
/*                 PSC3_7 (input)  -> MC93C46 serial data out   (Q)    */
/*                 PSC3_8 (output) -> MC93C46 chip select input (S)    */
/*                 PSC3_9 (output) -> MC93C46 serial clock      (C)    */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   none                                                */
/*   Module:                                                           */
/*   Project:                                                          */
/*   Version                                                           */
/*   Date:                                                             */
/*   Author:                                                           */
/*   Copyright:                                                        */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#ifndef __M93CXX_H__
#define __M93CXX_H__

#ifdef __cplusplus
extern "C" {
#endif

static void m93cxx_enable_write(void);
static void m93cxx_disable_write(void);
static void m93cxx_write_byte(uint32_t, uint8_t);
static uint8_t m93cxx_read_byte(uint32_t);
void wait_usec(unsigned long);

#define M93CXX_MODE_WORD
/*#define M93C46_MODE_BYTE*/
#define M93C46
#define M93C46_NVRAM_SIZE 128

#define GPIO_PSC3_6 (1 << 12)
#define GPIO_PSC3_7 (1 << 13)
#define GPIO_PSC3_8 (1 << 26)
#define GPIO_PSC3_9 (1 << 26)

#define START_BIT    0x1
#define EWDS_OPCODE  0x0
#define WRAL_OPCODE  0x1
#define ERAL_OPCODE  0x2
#define EWEN_OPCODE  0x3
#define WRITE_OPCODE 0x4
#define READ_OPCODE  0x8
#define ERASE_OPCODE 0xC

#define WAIT(i) wait_usec(i)

#define ENABLE_CHIP_SELECT        mpc5200.gpiosido |= GPIO_PSC3_8
#define DISABLE_CHIP_SELECT       mpc5200.gpiosido &= ~GPIO_PSC3_8
#define SET_DATA_BIT_HIGH         mpc5200.gpiosdo  |= GPIO_PSC3_6
#define SET_DATA_BIT_LOW          mpc5200.gpiosdo  &= ~GPIO_PSC3_6

#ifdef M93CXX_MODE_BYTE
#define GET_DATA_BYTE_SHIFT(val)  ((val) |= ((mpc5200.gpiosdi & GPIO_PSC3_7) >> 13)); \
                                  ((val) <<= 1)
#define SET_DATA_BYTE_SHIFT(val)  (((val) & 0x80) ? (mpc5200.gpiosdo |= GPIO_PSC3_6) : (mpc5200.gpiosdo &= ~GPIO_PSC3_6)); \
                                  ((val) <<= 1)
#else
#define GET_DATA_WORD_SHIFT(val)  ((val) |= ((mpc5200.gpiosdi & GPIO_PSC3_7) >> 13)); \
                                  ((val) <<= 1)
#define SET_DATA_WORD_SHIFT(val)  (((val) & 0x8000) ? (mpc5200.gpiosdo |= GPIO_PSC3_6) : (mpc5200.gpiosdo &= ~GPIO_PSC3_6)); \
                                  ((val) <<= 1)
#endif

#define MASK_HEAD_SHIFT(head)     ((((head) & 0x80000000) >> 31) ? (mpc5200.gpiosdo |= GPIO_PSC3_6) : (mpc5200.gpiosdo &= ~GPIO_PSC3_6)); \
                                  ((head) <<= 1)
#define DO_CLOCK_CYCLE            mpc5200.gpiowdo  |= GPIO_PSC3_9; \
                                  WAIT(1000); \
                                  mpc5200.gpiowdo  &= ~GPIO_PSC3_9
#define CHECK_WRITE_BUSY          while(!(mpc5200.gpiosdi & GPIO_PSC3_7))


#ifdef M93CXX_MODE_BYTE
#ifdef M93C46
#define M93C46_EWDS         ((START_BIT << 31) | (EWDS_OPCODE << 27))
#define M93C46_WRAL         ((START_BIT << 31) | (WRAL_OPCODE << 27))
#define M93C46_ERAL         ((START_BIT << 31) | (ERAL_OPCODE << 27))
#define M93C46_EWEN         ((START_BIT << 31) | (EWEN_OPCODE << 27))
#define M93C46_READ(addr)   ((START_BIT << 31) | (READ_OPCODE << 27)  | ((addr) << 22))
#define M93C46_WRITE(addr)  ((START_BIT << 31) | (WRITE_OPCODE << 27) | ((addr) << 22))
#define M93C46_ERASE(addr)  ((START_BIT << 31) | (ERASE_OPCODE << 27) | ((addr) << 22))
#define M93C46_CLOCK_CYCLES 10
#endif
#else
#ifdef M93C46
#define M93C46_EWDS         ((START_BIT << 31) | (EWDS_OPCODE << 27))
#define M93C46_WRAL         ((START_BIT << 31) | (WRAL_OPCODE << 27))
#define M93C46_ERAL         ((START_BIT << 31) | (ERAL_OPCODE << 27))
#define M93C46_EWEN         ((START_BIT << 31) | (EWEN_OPCODE << 27))
#define M93C46_READ(addr)   ((START_BIT << 31) | (READ_OPCODE << 27)  | ((addr) << 23))
#define M93C46_WRITE(addr)  ((START_BIT << 31) | (WRITE_OPCODE << 27) | ((addr) << 23))
#define M93C46_ERASE(addr)  ((START_BIT << 31) | (ERASE_OPCODE << 27) | ((addr) << 23))
#define M93C46_CLOCK_CYCLES 9
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __M93CXX_H__ */
