#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <unistd.h>

/*
 *  4.2.3 Get Supplementary IDs, P1003.1b-1993, p. 86
 */

int getgroups(
  int    gidsetsize __attribute__((unused)),
  gid_t  grouplist[] __attribute__((unused))
)
{
  return 0;  /* no supplemental group ids */
}
