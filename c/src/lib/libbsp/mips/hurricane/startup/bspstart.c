/**
 *  @file
 *  
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <string.h>

#include <bsp.h>
#include <libcpu/isr_entries.h>
#include <bsp/irq-generic.h>

void bsp_start( void );
uint32_t bsp_clicks_per_microsecond;

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{

  bsp_clicks_per_microsecond = CPU_CLOCK_RATE_MHZ;

  bsp_interrupt_initialize();
}
