/*
 *  Instantiate the clock driver shell.
 *
 *  Since there is no clock source on the simulator, all we do is
 *  make sure it will build.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CLOCK_VECTOR 0

#define Clock_driver_support_at_tick()

#define Clock_driver_support_install_isr( _new, _old ) \
  do { _old = 0; } while(0)

#define Clock_driver_support_initialize_hardware()

#define Clock_driver_support_shutdown_hardware()

#include "clockdrv_shell.c"
