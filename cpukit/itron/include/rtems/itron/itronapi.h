/**
 * @file rtems/itron/itronapi.h
 */

/*
 *  ITRON API Support
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_ITRON_ITRONAPI_H
#define _RTEMS_ITRON_ITRONAPI_H

#include <rtems/config.h>

/*
 *  _ITRON_API_Initialize
 *
 *  Initialize the ITRON API.
 */

void _ITRON_API_Initialize(
  rtems_configuration_table *configuration_table
);

#endif
/* end of include file */
