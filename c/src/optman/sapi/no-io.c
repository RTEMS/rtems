/*
 *  Input/Output Manager
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/io.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>

void _IO_Manager_initialization(
  rtems_driver_address_table *driver_table,
  uint32_t                    drivers_in_table,
  uint32_t                    number_of_drivers )
{
}

void _IO_Initialize_all_drivers( void )
{
}
