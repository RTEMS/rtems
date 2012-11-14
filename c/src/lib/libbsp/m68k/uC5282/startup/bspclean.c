/*
 *  SBC5206 bsp_cleanup
 *
 *  This routine returns control from RTEMS to the monitor.
 *
 *  Author: W. Eric Norum <norume@aps.anl.gov>
 *
 *  COPYRIGHT (c) 2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code error
)
{
  bsp_reset();
}

void bsp_cleanup(
  uint32_t status
)
{
  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, status );
}
