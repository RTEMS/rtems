/**
 * @file
 *
 * @brief Get Real User, Effective User, Ral Group, and Effective Group IDs
 * @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/userenv.h>

/**
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs,
 *        P1003.1b-1993, p. 84
 */
int setegid(
  gid_t  gid
)
{
  _POSIX_types_Egid = gid;
  return 0;
}
