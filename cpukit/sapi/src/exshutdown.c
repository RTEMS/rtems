/*
 *  Initialization Manager
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/init.h>
#include <rtems/score/interr.h>

void rtems_shutdown_executive( uint32_t result )
{
  _Terminate( RTEMS_FATAL_SOURCE_EXIT, result );

  /***************************************************************
   ***************************************************************
   *   SYSTEM SHUTS DOWN!!!  WE DO NOT RETURN TO THIS POINT!!!   *
   ***************************************************************
   ***************************************************************/
}
