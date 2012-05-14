/*
 *  Dynamically build the shell prompt
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <time.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/system.h>
#include <rtems/shell.h>
#include "internal.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>

void rtems_shell_get_prompt(
  rtems_shell_env_t *shell_env,
  char              *prompt,
  size_t             size
)
{
  char buf[256];
  char *cwd;

  /* XXX: show_prompt user adjustable */
  cwd = getcwd(buf,sizeof(buf));
  cwd = cwd != NULL ? cwd : "?";
  snprintf(prompt, size - 1, "%s%s[%s] %c ",
          ((shell_env->taskname) ? shell_env->taskname : ""),
          ((shell_env->taskname) ? " " : ""),
          cwd,
          geteuid()?'$':'#');
}
