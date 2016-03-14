/**
 * @file
 *
 * @brief Initialization of Device Drivers
 *
 * @ingroup ClassicIO
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/ioimpl.h>

bool _IO_All_drivers_initialized;

void _IO_Initialize_all_drivers( void )
{
   rtems_device_major_number major;

   _IO_All_drivers_initialized = true;

   for ( major=0 ; major < _IO_Number_of_drivers ; major ++ )
     (void) rtems_io_initialize( major, 0, NULL );
}
