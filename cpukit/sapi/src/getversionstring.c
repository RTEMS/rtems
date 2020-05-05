/**
 * @file
 *
 * @brief Get the RTEMS Version as a String
 *
 * @ingroup ClassicRTEMS
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <rtems.h>

const char *rtems_get_version_string(void)
{
  return _RTEMS_version;
}
