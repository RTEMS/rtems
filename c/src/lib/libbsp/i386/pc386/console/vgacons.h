/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _VGACONS_H_
#define _VGACONS_H_

#include <libchip/serial.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This is the ASCII for "PC" in the upper word and 0386 
 *  in the lower which should be unique enough to
 *  distinguish this type of serial device from others.
 */

#define VGA_CONSOLE 0x80670386

#define VGACONS_UART0  0
#define VGACONS_UART1  1

bool vgacons_probe( int minor );

/*
 * Driver function table
 */
extern console_fns vgacons_fns;

#ifdef __cplusplus
}
#endif

#endif /* _VGACONS_H_ */
