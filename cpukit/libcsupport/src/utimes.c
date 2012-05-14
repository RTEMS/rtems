/*
 *  Written by: Vinu Rajashekhar <vinutheraj@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <utime.h>
#include <sys/time.h>

int utimes(
  const char           *path,
  const struct timeval  times[2]
) 
{
  struct utimbuf timeinsecs;

  if ( times == NULL )
    return utime( path, NULL );

  timeinsecs.actime  = times[0].tv_sec;
  timeinsecs.modtime = times[1].tv_sec;

  return utime( path, &timeinsecs );
}
