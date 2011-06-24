/**
 * @file rtems/posix/posixapi.h
 *
 * This include file defines the top level interface to the POSIX API
 * implementation in RTEMS.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_POSIX_POSIXAPI_H
#define _RTEMS_POSIX_POSIXAPI_H

#include <rtems/config.h>

/**
 *  @brief Initialize POSIX API
 *
 *  This method is responsible for initializing each of the POSIX
 *  API managers.
 */
void _POSIX_API_Initialize(void);

#endif
/* end of include file */
