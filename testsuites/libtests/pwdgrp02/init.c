/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>

#include "tmacros.h"

const char rtems_test_name[] = "PWDGRP 2";

static void assert_pwd(struct passwd *pwd)
{
  rtems_test_assert(strcmp(pwd->pw_name, "root") == 0);
  rtems_test_assert(strcmp(pwd->pw_passwd, "") == 0);
  rtems_test_assert(pwd->pw_uid == 0);
  rtems_test_assert(pwd->pw_gid == 0);
  rtems_test_assert(strcmp(pwd->pw_comment, "") == 0);
  rtems_test_assert(strcmp(pwd->pw_gecos, "") == 0);
  rtems_test_assert(strcmp(pwd->pw_dir, "") == 0);
  rtems_test_assert(strcmp(pwd->pw_shell, "") == 0);
}

static void assert_grp(struct group *grp)
{
  rtems_test_assert(strcmp(grp->gr_name, "root") == 0);
  rtems_test_assert(strcmp(grp->gr_passwd, "") == 0);
  rtems_test_assert(grp->gr_gid == 0);
  rtems_test_assert(grp->gr_mem[0] == NULL);
}

static void assert_dir(const char *name)
{
  int rv;
  struct stat st;

  rv = lstat(name, &st);
  rtems_test_assert(rv == 0);
  rtems_test_assert(st.st_uid == 0);
  rtems_test_assert(st.st_gid == 0);
  rtems_test_assert(
    st.st_mode == (S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
  );
}

static void test(void)
{
  int rv;
  struct passwd pwd;
  struct group grp;
  struct passwd *pwd_res;
  struct group *grp_res;
  char buf[256];

  rtems_test_assert(getuid() == 0);
  rtems_test_assert(geteuid() == 0);

  rtems_test_assert(getgid() == 0);
  rtems_test_assert(getegid() == 0);

  memset(&pwd, 0xff, sizeof(pwd));
  rv = getpwnam_r("root", &pwd, &buf[0], sizeof(buf), &pwd_res);
  rtems_test_assert(rv == 0);
  rtems_test_assert(&pwd == pwd_res);
  assert_pwd(pwd_res);

  memset(&pwd, 0xff, sizeof(pwd));
  rv = getpwuid_r(0, &pwd, &buf[0], sizeof(buf), &pwd_res);
  rtems_test_assert(rv == 0);
  rtems_test_assert(&pwd == pwd_res);
  assert_pwd(pwd_res);

  memset(&grp, 0xff, sizeof(grp));
  rv = getgrnam_r("root", &grp, &buf[0], sizeof(buf), &grp_res);
  rtems_test_assert(rv == 0);
  rtems_test_assert(&grp == grp_res);
  assert_grp(grp_res);

  memset(&grp, 0xff, sizeof(grp));
  rv = getgrgid_r(0, &grp, &buf[0], sizeof(buf), &grp_res);
  rtems_test_assert(rv == 0);
  rtems_test_assert(&grp == grp_res);
  assert_grp(grp_res);

  assert_dir("/dev");
  assert_dir("/etc");

  rv = setuid(1);
  rtems_test_assert(rv == 0);

  rv = seteuid(1);
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = unlink("/etc/passwd");
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EACCES);

  errno = 0;
  rv = unlink("/etc/group");
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EACCES);

  errno = 0;
  rv = open("/etc/passwd", O_RDONLY);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EACCES);

  errno = 0;
  rv = open("/etc/group", O_RDONLY);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EACCES);

  errno = 0;
  rv = open("/etc/passwd", O_WRONLY);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EACCES);

  errno = 0;
  rv = open("/etc/group", O_WRONLY);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EACCES);

  errno = 0;
  rv = open("/etc/passwd", 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EACCES);

  errno = 0;
  rv = open("/etc/group", 0);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EACCES);
}

static void Init(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
