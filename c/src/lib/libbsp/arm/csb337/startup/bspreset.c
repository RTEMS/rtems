/*
 * Cogent CSB337 - AT91RM9200 Startup code
 *
 * Copyright (c) 2004 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
*/
#include <bsp.h>
#include <at91rm9200.h>
#include <at91rm9200_pmc.h>
#include <at91rm9200_emac.h>

void bsp_reset(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  /* Enable the watchdog timer, then wait for the world to end. */
  ST_REG(ST_WDMR) = ST_WDMR_RSTEN | 1;

  while(1);
}
