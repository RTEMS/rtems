/**
 *  @file
 *
 *  @brief Get Supplementary IDs
 *  @ingroup libcsupport
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

#include <rtems/seterr.h>

/**
 *  4.2.3 Get Supplementary IDs, P1003.1b-1993, p. 86
 */
int getgroups(
  int    gidsetsize,
  gid_t  grouplist[]
)
{
  int rv;
  struct passwd pwd;
  struct passwd *pwd_res;
  char buf[256];
  gid_t gid;
  const char *user;
  struct group *grp;

  rv = getpwuid_r(getuid(), &pwd, &buf[0], sizeof(buf), &pwd_res);
  if (rv != 0) {
    return rv;
  }

  gid = pwd.pw_gid;
  user = pwd.pw_name;

  setgrent();

  while ((grp = getgrent()) != NULL) {
    char **mem = &grp->gr_mem[0];

    if (grp->gr_gid == gid) {
      continue;
    }

    while (*mem != NULL) {
      if (strcmp(*mem, user) == 0) {
        if (rv < gidsetsize) {
          grouplist[rv] = grp->gr_gid;
        }

        ++rv;

        break;
      }

      ++mem;
    }
  }

  endgrent();

  if (gidsetsize == 0 || rv <= gidsetsize) {
    return rv;
  } else {
    rtems_set_errno_and_return_minus_one(EINVAL);
  }
}
