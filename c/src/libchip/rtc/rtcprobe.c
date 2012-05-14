/*
 *  This file contains the default Real-Time Clock probe routine.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <libchip/rtc.h>


bool rtc_probe(
  int minor
)
{
  return true;
}
