/*
 *  This file is the shell of what it initially was and is now misnamed.
 *
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

/*
 * TEMPORARY
 */

#include <assert.h>

int POSIX_MP_NOT_IMPLEMENTED()
{
  assert( 0 );
  return 0;
}

int POSIX_BOTTOM_REACHED()
{
  assert( 0 );
  return 0;
}

int POSIX_NOT_IMPLEMENTED()
{
  assert( 0 );
  return 0;
}

/*
 * END OF TEMPORARY
 */

