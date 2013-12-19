/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>

const char *Mouse_Type_Long = "Microsoft Mouse";
const char *Mouse_Type_Short = "ms";

#define PRESSED     1 
#define NOT_PRESSED 0 

#define MS_MOUSE_BYTES( _lb, _rb, _x, _y ) \
  (0x40 | ((_lb) ? 0x20 : 0x00) | ((_rb) ? 0x10 : 0x00) | \
     ((_y & 0xC) >> 4) | (_x >> 6)), \
  (_x & 0x3F), \
  (_y & 0x3F)

const unsigned char Mouse_Actions[] = {
  MS_MOUSE_BYTES(     PRESSED, NOT_PRESSED,    1,    2),
  MS_MOUSE_BYTES( NOT_PRESSED, PRESSED,        1,    2),
  MS_MOUSE_BYTES( NOT_PRESSED, NOT_PRESSED, 0xff,    2),
  MS_MOUSE_BYTES( NOT_PRESSED, NOT_PRESSED,    1, 0xff)
};

const size_t Mouse_Actions_Size = sizeof(Mouse_Actions);
const size_t Mouse_Actions_Per_Iteration = 3;

