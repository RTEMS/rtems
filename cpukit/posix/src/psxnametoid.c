/**
 * @file
 *
 * @brief POSIX Name to ID
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/posix/posixapi.h>

#include <errno.h>

const int _POSIX_Get_by_name_error_table[ 3 ] = {
  EINVAL,
  ENAMETOOLONG,
  ENOENT
};
