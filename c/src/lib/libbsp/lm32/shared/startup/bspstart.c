/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#include <string.h>

#include <bsp.h>
#include "../include/system_conf.h"
#include "../console/uart.h"
#include <rtems/score/heap.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start(void)
{
  /* Setup console baud rate */
  BSP_uart_init(UART_BAUD_RATE);
}

void bsp_predriver_hook(void)
{
}


