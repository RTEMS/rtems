/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <unistd.h> /* write */

#include <bsp.h>
#include <rtems/bspIo.h>

static void debug_putc( char c )
{
  write( 2, &c, 1 );
}

BSP_output_char_function_type BSP_output_char =  debug_putc;
BSP_polling_getchar_function_type BSP_poll_char = NULL;
