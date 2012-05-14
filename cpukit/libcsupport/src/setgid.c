#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <rtems/userenv.h>

/*
 *
 *  4.2.2 Set User and Group IDs, P1003.1b-1993, p. 84
 */
int setgid(
  gid_t  gid
)
{
  _POSIX_types_Gid = gid;
  return 0;
}
