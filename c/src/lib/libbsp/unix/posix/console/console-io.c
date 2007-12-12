/*
 *  UNIX BSP Debug IO
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

/*
 *  To support printk
 */

#include <rtems/bspIo.h>
#include <stdio.h>

void POSIX_BSP_output_char(char c) { fputc( c, stderr ); }

BSP_output_char_function_type           BSP_output_char = POSIX_BSP_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
