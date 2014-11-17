/**
 * @file
 *
 * @brief Shell login check function.
 */

/*
 * Copyright (c) 2009-2014 embedded brains GmbH and others.
 *
 * embedded brains GmbH
 * Dornierstr. 4
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * Based on work from Chris Johns and Fernando Ruiz.
 *
 * Derived from file "cpukit/libmisc/shell/shell.c".
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <crypt.h>

#include <rtems/shell.h>
#include <rtems/userenv.h>

bool rtems_shell_login_check(
  const char *user,
  const char *passphrase
)
{
  char buf[256];
  struct passwd *pw_res;
  struct passwd pw;
  int eno;
  bool ok;

  eno = getpwnam_r(user, &pw, &buf[0], sizeof(buf), &pw_res);

  /* Valid user? */
  if (eno == 0 && strcmp(pw.pw_passwd, "*") != 0) {
    if (strcmp(pw.pw_passwd, "") == 0) {
      ok = true;
    } else if (strcmp(pw.pw_passwd, "x") == 0) {
      /* TODO: /etc/shadow */
      ok = false;
    } else {
      struct crypt_data data;
      char *s;

      s = crypt_r(passphrase, pw.pw_passwd, &data);
      ok = strcmp(s, pw.pw_passwd) == 0;
    }
  } else {
    ok = false;
  }

  if (ok && strcmp(pw.pw_dir, "") != 0) {
    ok = chroot(pw.pw_dir) == 0;
  }

  if (ok) {
    rtems_shell_env_t *env = rtems_shell_get_current_env();

    if (env != NULL) {
      chown(env->devname, pw.pw_uid, 0);
    }

    setuid(pw.pw_uid);
    setgid(pw.pw_gid);
    seteuid(pw.pw_uid);
    setegid(pw.pw_gid);
    rtems_current_user_env_getgroups();
  }

  return ok;
}
