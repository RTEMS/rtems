/*
 * Copyright (c) 2014, 2019 embedded brains GmbH.  All rights reserved.
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

#include <rtems/imfs.h>
#include <rtems/shell.h>
#include <rtems/userenv.h>

#include "tmacros.h"

const char rtems_test_name[] = "SHELL 1";

static const char etc_passwd[] =
  "moop:foo:1:3:blob:a::c\n"
  "no:*:2:4::::\n"
  "zero::3:5::::\n"
  "shadow:x:4:6::::\n"
  "invchroot::5:7:::/inv:\n"
  "chroot::6:8:::/chroot:\n";

static const char etc_group[] =
  "A::1:moop,u,v,w,zero\n"
  "B::2:moop\n"
  "blub:bar:3:moop\n"
  "C::4:l,m,n,moop\n"
  "D::5:moop,moop\n"
  "E::6:x\n"
  "E::7:y,z\n"
  "F::8:s,moop,t\n";

static const char joel_in[] =
  "#! joel\n"
  "jtst hello world\n"
  "jtst 1 2 3 4 5\n";

static const char joel_out_1[] =
  " 3 'jtst hello world'\n"
  " 6 'jtst 1 2 3 4 5'\n";

static const char joel_out_2[] =
  "\n"
  "RTEMS Shell on (null). Use 'help' to list commands.\n"
  " 3 'jtst hello world'\n"
  " 6 'jtst 1 2 3 4 5'\n";

static int joel_test_command(int argc, char** argv)
{
  int i;
  fprintf(stdout, "%2d '", argc);
  for (i = 0; i < argc; ++i) {
    fprintf(stdout, argv[i]);
    if (i < (argc - 1))
      fprintf(stdout, " ");
  }
  fprintf(stdout, "'\n");
  return 0;
}

static void test_joel(void)
{
  /*
   * Running a joel script tests the shell main loop.
   */
  char buf[sizeof(joel_out_2) + 1];
  rtems_shell_cmd_t* cmd;
  FILE *in;
  FILE *out;
  FILE *current_stdout = stdout;
  FILE *current_stdin = stdin;
  size_t len;
  rtems_status_code sc;

  /*
   * Use a private command due to the way the testsuite maps printk onto printf.
   */
  cmd = rtems_shell_add_cmd("jtst", "misc", "joel test", joel_test_command);
  rtems_test_assert(cmd != NULL);

  len = strlen(joel_in);

  in = fopen("/jin", "w");
  rtems_test_assert(in != NULL);
  rtems_test_assert(fwrite(joel_in, sizeof(char), len, in) == len);
  rtems_test_assert(fclose(in) == 0);

  /*
   * The shell opens the input and output files.
   */
  sc = rtems_shell_script(
    "JTST",
    8 * 1024,
    1,
    "/jin",
    "/jout",
    false,
    true,
    false);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  out = fopen("/jout", "r");
  rtems_test_assert(out != NULL);
  rtems_test_assert(!feof(out));
  memset(buf, 0, sizeof(buf));
  len = fread(buf, sizeof(char), sizeof(buf), out);
  rtems_test_assert(len > 0);
  rtems_test_assert(strcmp(joel_out_1, buf) == 0);
  rtems_test_assert(fclose(out) == 0);

  /*
   * The shell task inherits the parent stdin and stdout
   */
  in = fopen("/jin", "r");
  rtems_test_assert(in != NULL);
  out = fopen("/jout", "w");
  rtems_test_assert(out != NULL);

  stdin = in;
  stdout = out;

  sc = rtems_shell_script(
    "JTST",
    8 * 1024,
    1,
    "stdin",
    "stdout",
    false,
    true,
    false);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  stdout = current_stdout;
  stdin = current_stdin;

  rtems_test_assert(fclose(in) == 0);
  rtems_test_assert(fclose(out) == 0);

  out = fopen("/jout", "r");
  rtems_test_assert(out != NULL);
  rtems_test_assert(!feof(out));
  memset(buf, 0, sizeof(buf));
  len = fread(buf, sizeof(char), sizeof(buf), out);
  rtems_test_assert(len > 0);
  rtems_test_assert(strcmp(joel_out_2, buf) == 0);
  rtems_test_assert(fclose(out) == 0);
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

  rv = IMFS_make_linearfile(
    "/etc/passwd",
    S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH,
    etc_passwd,
    sizeof(etc_passwd)
  );
  rtems_test_assert(rv == 0);

  rv = IMFS_make_linearfile(
    "/etc/group",
    S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH,
    etc_group,
    sizeof(etc_group)
  );
  rtems_test_assert(rv == 0);

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
  test_joel();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_POSIX_KEYS 2
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

#define CONFIGURE_SHELL_COMMANDS_INIT

#include <rtems/shellconfig.h>
