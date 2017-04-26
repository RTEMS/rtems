/*
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/cpu.h>
#include <rtems/bspIo.h>
#include <inttypes.h>

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame )
{
  int i;

  for ( i = 0; i < 32; ++i ) {
      printk( "r%02i = 0x%016" PRIx32 "\n",i, frame->r[i]);
  }
}
