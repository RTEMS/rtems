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
#include <rtems/fatal.h>
#include <stdio.h>

void _OR1K_Exception_default(uint32_t vector, CPU_Exception_frame *frame);

void _OR1K_Exception_default(uint32_t vector, CPU_Exception_frame *frame)
{
  rtems_fatal( RTEMS_FATAL_SOURCE_EXCEPTION, (rtems_fatal_code) frame );
}
