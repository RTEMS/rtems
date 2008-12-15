/**
 * @file rtems/itron/itronapi.h
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

/**
 *  @brief Initialize ITRON API
 *
 *  This method is used to initialize the ITRON API.
 */
void _ITRON_API_Initialize(void);

/**
 *  @brief ITRON API Cofniguration Table
 *
 *  This is the ITRON API Configuration Table expected to be generated
 *  by confdefs.h.
 */
extern itron_api_configuration_table Configuration_ITRON_API;

#endif
/* end of include file */
