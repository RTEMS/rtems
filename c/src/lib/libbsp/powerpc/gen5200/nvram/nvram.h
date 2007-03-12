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
| this file contains the nvram declarations                       |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       nvram.h                                             */
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
/*   References:   DS1307-based Non-Volatile memory device driver      */
/*   Module:       nvram.h                                             */
/*   Project:      RTEMS 4.6.0pre1 / MCF5206Elite BSP                  */
/*   Version       1.1                                                 */
/*   Date:         10/26/2001                                          */
/*   Author:       Victor V. Vengerov                                  */
/*   Copyright:    Copyright (C) 2000 OKTET Ltd.,St.-Petersburg,Russia */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in the file LICENSE in this distribution or at              */
/*   http://www.rtems.com/license/LICENSE.                        */
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

#ifndef __NVRAM_H__
#define __NVRAM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include <bsp.h> */
#include <bsp.h>

/* Macros for TBL read access */
#define TBL_READ(count)    __asm__ volatile ("mftb %0\n" : "=r" (count) : "0" (count))
#define TMBASE_CLOCK       (G2_CLOCK/4)

/* Simple usec delay function prototype */
void wait_usec(unsigned long);

/* nvram_driver_initialize --
 *     Non-volatile memory device driver initialization.
 */
rtems_device_driver
nvram_driver_initialize(rtems_device_major_number major,
                        rtems_device_minor_number minor,
                        void *arg);

/* nvram_driver_open --
 *     Non-volatile memory device driver open primitive.
 */
rtems_device_driver
nvram_driver_open(rtems_device_major_number major,
                  rtems_device_minor_number minor,
                  void *arg);

/* nvram_driver_close --
 *     Non-volatile memory device driver close primitive.
 */
rtems_device_driver
nvram_driver_close(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void *arg);

/* nvram_driver_read --
 *     Non-volatile memory device driver read primitive.
 */
rtems_device_driver
nvram_driver_read(rtems_device_major_number major,
                  rtems_device_minor_number minor,
                  void *arg);

/* nvram_driver_write --
 *     Non-volatile memory device driver write primitive.
 */
rtems_device_driver
nvram_driver_write(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __NVRAM_H__ */
