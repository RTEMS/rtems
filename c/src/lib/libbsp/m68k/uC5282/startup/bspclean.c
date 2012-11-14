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

void bsp_cleanup(
  uint32_t status
)
{
  bsp_reset();
}
