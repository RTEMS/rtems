/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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

#include <bsp.h>
#include <pmacros.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

const char rtems_test_name[] = "PSXPASSWD 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);
void print_passwd(struct passwd *pw);
void print_group(struct group *gr);

void print_passwd(
  struct passwd *pw
)
{
  printf( 
    "  username: %s\n"
    "  user password: %s\n"
    "  user ID: %d\n"
    "  group ID: %d\n"
    "  real name: %s\n"
    "  home directory: %s\n"
    "  shell program: %s\n",
    pw->pw_name,
    pw->pw_passwd,
    pw->pw_uid,
    pw->pw_gid,
    pw->pw_gecos,
    pw->pw_dir,
    pw->pw_shell
  );
}
  
void print_group(
  struct group *gr
)
{
  printf( 
    "  group name: %s\n"
    "  group  password: %s\n"
    "  group  ID: %d\n",
    gr->gr_name,
    gr->gr_passwd,
    gr->gr_gid
  );

  /* TBD print users in group */
}
  
rtems_task Init(
  rtems_task_argument ignored
)
{
  struct passwd *pw;
  struct group  *gr;

  TEST_BEGIN();

  /* getpwent */
  puts( "Init - getpwent() -- OK, result should be NULL" );
  pw = getpwent();
  rtems_test_assert( !pw );

  /* getgrent */
  puts( "Init - getgrent() -- OK, result should be NULL" );
  gr = getgrent();
  rtems_test_assert( !gr );

  /* setpwent */

  puts( "Init - setpwent() -- OK" );
  setpwent();

  /* setgrent */

  puts( "Init - setgrent() -- OK" );
  setgrent();

  /* getpwent */

  puts( "Init - getpwent() (1) -- OK" );
  pw = getpwent();
  rtems_test_assert( pw );
  print_passwd( pw );

  puts( "Init - getpwent() (2) -- result should be NULL" );
  pw = getpwent();
  rtems_test_assert( !pw );

  /* getgrent */

  puts( "Init - getgrent() (1) -- OK" );
  gr = getgrent();
  rtems_test_assert( gr );
  print_group( gr );

  puts( "Init - getgrent() (2) -- result should be NULL" );
  gr = getgrent();
  rtems_test_assert( !gr );

  /* getpwnam */
  puts( "Init - getpwnam(\"root\") -- OK" );
  pw = getpwnam( "root" );
  rtems_test_assert( pw );
  print_passwd( pw );

  puts( "Init - getpwnam(\"suser\") -- result should be NULL" );
  pw = getpwnam( "suser" );
  rtems_test_assert( !pw );

  /* getpwuid */
  puts( "Init - getpwuid(0) -- OK" );
  pw = getpwuid( 0 );
  rtems_test_assert( pw );
  print_passwd( pw );

  rtems_test_assert( strcmp(pw->pw_name, "root") == 0 );
  rtems_test_assert( strcmp(pw->pw_passwd, "") == 0 );
  rtems_test_assert( pw->pw_uid == 0 );
  rtems_test_assert( pw->pw_gid == 0 );
  rtems_test_assert( strcmp(pw->pw_comment, "") == 0 );
  rtems_test_assert( strcmp(pw->pw_gecos, "") == 0 );
  rtems_test_assert( strcmp(pw->pw_dir, "") == 0 );
  rtems_test_assert( strcmp(pw->pw_shell, "") == 0 );

  puts( "Init - getpwuid(4) -- result should be NULL" );
  pw = getpwuid( 4 );
  rtems_test_assert( !pw );

  /* getgrnam */
  puts( "Init - getgrnam(\"root\") -- OK" );
  gr = getgrnam("root");
  rtems_test_assert( gr );
  print_group( gr );

  /* getgrgid */
  puts( "Init - getgrgid(0) -- OK" );
  gr = getgrgid(0);
  rtems_test_assert( gr );
  print_group( gr );

  rtems_test_assert( strcmp(gr->gr_name, "root") == 0 );
  rtems_test_assert( strcmp(gr->gr_passwd, "") == 0 );
  rtems_test_assert( gr->gr_gid == 0 );
  rtems_test_assert( gr->gr_mem[0] == NULL );

  puts( "Init - getgrgid(4) -- result should be NULL");
  gr = getgrgid( 4 );
  rtems_test_assert( !gr );

  /* endpwent */
  puts( "Init - endpwent() -- OK" );
  endpwent();

  /* endgrent */
  puts( "Init - endgrent() -- OK" );
  endgrent();

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_POSIX_KEYS 1
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS 1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
