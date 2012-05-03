#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <errno.h>
#include <rtems/seterr.h>

/*
 *  4.3.3 Set Process Group ID for Job Control, P1003.1b-1993, p. 89
 */

int setpgid(
  pid_t  pid __attribute__((unused)),
  pid_t  pgid __attribute__((unused))
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
