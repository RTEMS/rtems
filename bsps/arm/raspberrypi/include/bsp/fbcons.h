/**
 * @file
 *
 * @ingroup raspberrypi_console
 *
 * @brief framebuffer graphic console support.
 */

/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef _FBCONS_H_
#define _FBCONS_H_

#include <libchip/serial.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This is the ASCII for "PI" in the upper word and 2835
 *  in the lower which should be unique enough to
 *  distinguish this type of serial device from others.
 */

#define FB_CONSOLE 0x50492835

bool fbcons_probe( int minor );

/*
 * Driver function table
 */
extern const console_fns fbcons_fns;

#ifdef __cplusplus
}
#endif

#endif /* _FBCONS_H_ */
