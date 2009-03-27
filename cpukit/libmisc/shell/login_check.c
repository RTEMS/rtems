/**
 * @file
 *
 * @author Sebastian Huber <sebastian.huber@embedded-brains.de>
 *
 * @brief Shell login check function.
 */

/*
 * Copyright (c) 2009
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * Based on work from Chris Johns, Fernando Ruiz and Till Straumann.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#include <rtems/login.h>
#include <rtems/shell.h>
#include <rtems/userenv.h>

bool rtems_shell_login_check(
  const char *user,
  const char *passphrase
)
{
  struct passwd *pw = getpwnam( user);

  /* Valid user? */
  if (pw != NULL && strcmp( pw->pw_passwd, "!") != 0) {
    setuid( pw->pw_uid);
    setgid( pw->pw_gid);
    rtems_current_user_env->euid = 0;
    rtems_current_user_env->egid = 0;
    chown( rtems_current_shell_env->devname, pw->pw_uid, 0);
    rtems_current_user_env->euid = pw->pw_uid;
    rtems_current_user_env->egid = pw->pw_gid;
    if (strcmp( pw->pw_passwd, "*") == 0) {
      /* TODO: /etc/shadow */
      return true;
    } else {
      /* TODO: crypt() */
      return true;
    }
  }

  return false;
}
