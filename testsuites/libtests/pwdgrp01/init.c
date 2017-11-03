/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>

#include "tmacros.h"

const char rtems_test_name[] = "PWDGRP 1";

static void create_file(const char *name, const char *content)
{
  FILE *fp;
  int rv;

  fp = fopen(name, "wx");
  rtems_test_assert(fp != NULL);

  rv = fputs(content, fp);
  rtems_test_assert(rv == 0);

  rv = fclose(fp);
  rtems_test_assert(rv == 0);
}

static void assert_pwd(struct passwd *pwd)
{
  rtems_test_assert(strcmp(pwd->pw_name, "moop") == 0);
  rtems_test_assert(strcmp(pwd->pw_passwd, "foo") == 0);
  rtems_test_assert(pwd->pw_uid == 1);
  rtems_test_assert(pwd->pw_gid == 3);
  rtems_test_assert(strcmp(pwd->pw_comment, "blob") == 0);
  rtems_test_assert(strcmp(pwd->pw_gecos, "a") == 0);
  rtems_test_assert(strcmp(pwd->pw_dir, "b") == 0);
  rtems_test_assert(strcmp(pwd->pw_shell, "c") == 0);
}

static void assert_grp(struct group *grp)
{
  rtems_test_assert(strcmp(grp->gr_name, "blub") == 0);
  rtems_test_assert(strcmp(grp->gr_passwd, "bar") == 0);
  rtems_test_assert(grp->gr_gid == 3);
  rtems_test_assert(strcmp(grp->gr_mem[0], "moop") == 0);
  rtems_test_assert(grp->gr_mem[1] == NULL);
}

static void test(void)
{
  int rv;
  struct passwd pwd;
  struct group grp;
  struct passwd *pwd_res;
  struct group *grp_res;
  char buf[256];
  gid_t grps[5];

  rv = mkdir("/etc", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  rtems_test_assert(rv == 0);

  create_file(
    "/etc/passwd",
    "moop:foo:1:3:blob:a:b:c\n"
  );

  create_file(
    "/etc/group",
    "A::1:moop,u,v,w\n"
    "B::2:moop\n"
    "blub:bar:3:moop\n"
    "C::4:l,m,n,moop\n"
    "D::5:moop,moop\n"
    "E::6:x\n"
    "E::7:y,z\n"
    "F::8:s,moop,t\n"
  );

  memset(&pwd, 0xff, sizeof(pwd));
  rv = getpwnam_r("moop", &pwd, &buf[0], sizeof(buf), &pwd_res);
  rtems_test_assert(rv == 0);
  rtems_test_assert(&pwd == pwd_res);
  assert_pwd(pwd_res);

  memset(&pwd, 0xff, sizeof(pwd));
  rv = getpwuid_r(1, &pwd, &buf[0], sizeof(buf), &pwd_res);
  rtems_test_assert(rv == 0);
  rtems_test_assert(&pwd == pwd_res);
  assert_pwd(pwd_res);

  memset(&grp, 0xff, sizeof(grp));
  rv = getgrnam_r("blub", &grp, &buf[0], sizeof(buf), &grp_res);
  rtems_test_assert(rv == 0);
  rtems_test_assert(&grp == grp_res);
  assert_grp(grp_res);

  memset(&grp, 0xff, sizeof(grp));
  rv = getgrgid_r(3, &grp, &buf[0], sizeof(buf), &grp_res);
  rtems_test_assert(rv == 0);
  rtems_test_assert(&grp == grp_res);
  assert_grp(grp_res);

  rv = setuid(0);
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = getgroups(0, NULL);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EINVAL);

  rv = setuid(1);
  rtems_test_assert(rv == 0);

  rv = getgroups(0, NULL);
  rtems_test_assert(rv == 5);

  errno = 0;
  rv = getgroups(1, &grps[0]);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EINVAL);

  memset(&grps[0], 0xff, sizeof(grps));
  rv = getgroups(5, &grps[0]);
  rtems_test_assert(rv == 5);
  rtems_test_assert(grps[0] == 1);
  rtems_test_assert(grps[1] == 2);
  rtems_test_assert(grps[2] == 4);
  rtems_test_assert(grps[3] == 5);
  rtems_test_assert(grps[4] == 8);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_POSIX_KEYS 1
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
