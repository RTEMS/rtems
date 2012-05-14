/*
 *  ERC32 Idle Thread with power-down function
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#include <bsp.h>

void *bsp_idle_thread( uintptr_t ignored )
{
  while (1) {
    ERC32_MEC.Power_Down = 0;   /* value is irrelevant */
  }
  return NULL;
}
