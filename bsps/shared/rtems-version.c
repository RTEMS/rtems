/*
 *  COPYRIGHT (c) 2003, Ralf Corsepius, Ulm, Germany.
 *  COPYRIGHT (c) 2003, On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/version.h>
#include <rtems/system.h>

#include <bspopts.h>

#ifndef RTEMS_VERSION
#error "Missing RTEMS_VERSION"
#endif
#ifndef CPU_NAME
#error "Missing CPU_NAME"
#endif
#ifndef CPU_MODEL_NAME
#error "Missing CPU_MODEL_NAME"
#endif
#ifndef RTEMS_BSP
#error "Missing RTEMS_BSP"
#endif

const char _RTEMS_version[] =
  "rtems-" RTEMS_VERSION " (" CPU_NAME "/" CPU_MODEL_NAME "/"
  RTEMS_XSTRING( RTEMS_BSP ) ")";

const char *rtems_board_support_package( void )
{
  return RTEMS_XSTRING( RTEMS_BSP );
}
