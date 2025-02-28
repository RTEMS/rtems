/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Shell login check function.
 */

/*
 * Copyright (C) 2001 Fernando Ruiz Casas <fruizcasas@gmail.com>
 * Copyright (C) 2009, 2014 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Based on work from Chris Johns and Fernando Ruiz.
 * Derived from file "cpukit/libmisc/shell/shell.c".
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
