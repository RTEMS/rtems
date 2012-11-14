/*
 *  SBC5206 bsp_cleanup
 *
 *  This routine returns control from RTEMS to the monitor.
 *
 *  Author:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code error
)
{
  printk("\nRTEMS exited!\n");
}

void  __attribute__((weak)) bsp_cleanup(
  uint32_t status
)
{
  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, status );
}
