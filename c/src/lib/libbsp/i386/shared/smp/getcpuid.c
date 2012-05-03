/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp/apic.h>
#include <bsp/smp-imps.h>

static int lapic_dummy = 0;
unsigned imps_lapic_addr = ((unsigned)(&lapic_dummy)) - LAPIC_ID;

int  bsp_smp_processor_id(void)
{
  return APIC_ID(IMPS_LAPIC_READ(LAPIC_ID));
}

