/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/seterr.h>

/*PAGE
 *
 *  4.2.3 Get Supplementary IDs, P1003.1b-1993, p. 86
 */

int getgroups(
  int    gidsetsize,
  gid_t  grouplist[]
)
{
  return 0;  /* no supplemental group ids */
}

