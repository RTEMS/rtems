/**
 * @file rtems/mouse_parser.h
 *
 * @brief Initialize Mouse Parser Engine
 *
 * This file is the header file for the Mouse Parser Engine.
 */

/*
 * This file is derived from a UNIX Serial Port Mouse Driver with the
 * following notice:
 *
 * ==================================================================
 * Copyright (c) 1999 Greg Haerr <greg@censoft.com>
 * Portions Copyright (c) 1991 David I. Bell
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * UNIX Serial Port Mouse Driver
 *
 * This driver opens a serial port directly, and interprets serial data.
 * Microsoft, PC, Logitech and PS/2 mice are supported.  The PS/2 mouse
 * is only supported if the OS runs the mouse byte codes through the
 * serial port.
 *
 * Mouse Types Supported: pc  ms, logi, ps2
 * ==================================================================
 *
 * It has been modified to support the concept of being just a parser
 * fed data from an arbitrary source.  It is independent of either
 * a PS/2 driver or a serial port.
 *
 * It was moved to cpukit/libmisc/mouse by Joel Sherrill.
 */

#ifndef __MOUSE_PARSER_h__
#define __MOUSE_PARSER_h__

#include <rtems/mw_uid.h>

/**
 *  @defgroup libmisc_mouseparser Mouse Parser Engine
 *  @ingroup libmisc_mouse
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is the mask for the right button.
 *
 * @note Use the same definitions as the user interface.
 */
#define RBUTTON      MV_BUTTON_RIGHT

/**
 * This is the mask for the center button.
 *
 * @note Use the same definitions as the user interface.
 */
#define MBUTTON      MV_BUTTON_CENTER

/**
 * This is the mask for the left button.
 *
 * @note Use the same definitions as the user interface.
 */
#define LBUTTON      MV_BUTTON_LEFT

/**
 *  This type is the device coordinates.
 */
typedef int           COORD;

/**
 *  This type is the mouse button mask.
 */
typedef unsigned int  BUTTON;

/**
 *  This type defines a pointer to the enqueue method.  It is
 *  available since some device drivers keep pointers to the method
 *  to know when to enqueue or not.
 */
typedef void (*mouse_parser_enqueue_handler)(unsigned char *, size_t);

/**
 * @brief Initialize the mouse parser engine.
 *
 * This method initializes the Mouse Parser Engine for the mouse
 * of @a type. The @a type should be one of the following strings:
 * pc  ms, logi, ps2.
 *
 * @a param[in] type indicates the type of mouse.
 *
 * @retval This method returns 0 on success and -1 on error.
 */
int mouse_parser_initialize(const char *type);

/**
 * @brief Enqueue input to the mouse parser engine.
 *
 * This method is used to pass mouse input to the Mouse Parser Engine.
 *
 * @a param[in] buffer is the data to enqueue
 * @a param[in] size is the amount of data to enqueue
 */
void mouse_parser_enqueue(
  unsigned char *buffer,
  size_t         size
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
