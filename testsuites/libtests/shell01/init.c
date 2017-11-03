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

#include <rtems/shell.h>
#include <rtems/userenv.h>

#include "tmacros.h"

const char rtems_test_name[] = "SHELL 1";

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

static void test(void)
{
  rtems_user_env_t *uenv;
  rtems_status_code sc;
  struct stat st_chroot;
  struct stat st_workdir;
  bool ok;
  int rv;

  rv = mkdir("/etc", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  rtems_test_assert(rv == 0);

  rv = mkdir("/chroot", S_IRWXU | S_IRWXG | S_IRWXO);
  rtems_test_assert(rv == 0);

  rv = lstat("/chroot", &st_chroot);
  rtems_test_assert(rv == 0);

  create_file(
    "/etc/passwd",
    "moop:foo:1:3:blob:a::c\n"
    "no:*:2:4::::\n"
    "zero::3:5::::\n"
    "shadow:x:4:6::::\n"
    "invchroot::5:7:::/inv:\n"
    "chroot::6:8:::/chroot:\n"
  );

  create_file(
    "/etc/group",
    "A::1:moop,u,v,w,zero\n"
    "B::2:moop\n"
    "blub:bar:3:moop\n"
    "C::4:l,m,n,moop\n"
    "D::5:moop,moop\n"
    "E::6:x\n"
    "E::7:y,z\n"
    "F::8:s,moop,t\n"
  );

  sc = rtems_libio_set_private_env();
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  uenv = rtems_current_user_env_get();

  rv = setuid(0);
  rtems_test_assert(rv == 0);

  rv = seteuid(0);
  rtems_test_assert(rv == 0);

  ok = rtems_shell_login_check("inv", NULL);
  rtems_test_assert(!ok);

  ok = rtems_shell_login_check("no", NULL);
  rtems_test_assert(!ok);

  ok = rtems_shell_login_check("shadow", NULL);
  rtems_test_assert(!ok);

  ok = rtems_shell_login_check("moop", "false");
  rtems_test_assert(!ok);

  ok = rtems_shell_login_check("invchroot", NULL);
  rtems_test_assert(!ok);

  rtems_test_assert(getuid() == 0);
  rtems_test_assert(geteuid() == 0);
  rtems_test_assert(getgid() == 0);
  rtems_test_assert(getegid() == 0);
  rtems_test_assert(uenv->ngroups == 0);

  ok = rtems_shell_login_check("zero", NULL);
  rtems_test_assert(ok);
  rtems_test_assert(getuid() == 3);
  rtems_test_assert(geteuid() == 3);
  rtems_test_assert(getgid() == 5);
  rtems_test_assert(getegid() == 5);
  rtems_test_assert(uenv->ngroups == 1);
  rtems_test_assert(uenv->groups[0] == 1);

  ok = rtems_shell_login_check("moop", "foo");
  rtems_test_assert(ok);
  rtems_test_assert(getuid() == 1);
  rtems_test_assert(geteuid() == 1);
  rtems_test_assert(getgid() == 3);
  rtems_test_assert(getegid() == 3);
  rtems_test_assert(uenv->ngroups == 5);
  rtems_test_assert(uenv->groups[0] == 1);
  rtems_test_assert(uenv->groups[1] == 2);
  rtems_test_assert(uenv->groups[2] == 4);
  rtems_test_assert(uenv->groups[3] == 5);
  rtems_test_assert(uenv->groups[4] == 8);

  rv = setuid(0);
  rtems_test_assert(rv == 0);

  rv = seteuid(0);
  rtems_test_assert(rv == 0);

  ok = rtems_shell_login_check("chroot", NULL);
  rtems_test_assert(ok);
  rtems_test_assert(getuid() == 6);
  rtems_test_assert(geteuid() == 6);
  rtems_test_assert(getgid() == 8);
  rtems_test_assert(getegid() == 8);

  rv = lstat(".", &st_workdir);
  rtems_test_assert(rv == 0);
  rtems_test_assert(memcmp(&st_chroot, &st_workdir, sizeof(st_chroot)) == 0);

  rtems_libio_use_global_env();
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
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#define CONFIGURE_SHELL_COMMANDS_INIT

#include <rtems/shellconfig.h>
