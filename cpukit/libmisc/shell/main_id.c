/*
 *  ID Command Implementation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_id(
  int   argc __attribute__((unused)),
  char *argv[] __attribute__((unused))
)
{
  struct passwd *pwd;
  struct group  *grp;

  pwd = getpwuid(getuid());
  grp = getgrgid(getgid());
  printf(
    "uid=%d(%s),gid=%d(%s),",
    getuid(),
    (pwd) ? pwd->pw_name : "",
    getgid(),
    (grp) ? grp->gr_name : ""
  );
  pwd = getpwuid(geteuid());
  grp = getgrgid(getegid());
  printf(
    "euid=%d(%s),egid=%d(%s)\n",
    geteuid(),
    (pwd) ? pwd->pw_name : "",
    getegid(),
    (grp) ? grp->gr_name : ""
  );
  return 0;
}

rtems_shell_cmd_t rtems_shell_ID_Command = {
  "id",                                      /* name */
  "show uid, gid, euid, and egid",           /* usage */
  "misc",                                    /* topic */
  rtems_shell_main_id,                       /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
