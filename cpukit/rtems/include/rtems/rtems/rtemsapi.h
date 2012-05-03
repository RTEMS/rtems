/**
 * @file rtems/rtems/rtemsapi.h
 *
 *  RTEMS API Support
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_RTEMSAPI_H
#define _RTEMS_RTEMS_RTEMSAPI_H

#include <rtems/config.h>

/**
 *  _RTEMS_API_Initialize
 *
 *  This routine initializes the RTEMS API by invoking the initialization
 *  routine for each RTEMS manager with the appropriate parameters
 *  from the configuration_table.
 */
void _RTEMS_API_Initialize(void);

#endif
/* end of include file */
