/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief ls Shell Command Implementation
 */

/*
 * Copyright (C) 2001 Fernando Ruiz Casas <fruizcasas@gmail.com>
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
  int   argc RTEMS_UNUSED,
  char *argv[] RTEMS_UNUSED
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
  .name = "id",
  .usage = "show uid",
  .topic = "misc",
  .command = rtems_shell_main_id,
  .alias = NULL,
  .next = NULL
};
