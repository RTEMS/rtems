/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <rtems/userenv.h>

/*
 *
 *  4.2.2 Set User and Group IDs, P1003.1b-1993, p. 84
 */
int setuid(
  uid_t  uid
)
{
  _POSIX_types_Uid = uid;
  return 0;
}
