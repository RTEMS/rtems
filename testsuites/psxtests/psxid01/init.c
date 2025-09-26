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

#include <tmacros.h>
#include "test_support.h"

#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>

const char rtems_test_name[] = "PSXID 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void test_gid(void);
void test_uid(void);
void test_pid(void);
void test_getlogin(void);

void test_gid(void)
{
  gid_t gid;
  int sc;

  gid = getegid();
  rtems_test_assert( gid == 0 );
  printf( "getegid = %d\n", gid );

  gid = getgid();
  rtems_test_assert( gid == 0 );
  printf( "getgid = %d\n", gid );

  puts( "setgid(5)" );
  sc = setgid(5);
  rtems_test_assert( sc == 0 );

  gid = getegid();
  rtems_test_assert( gid == 0 );
  printf( "getegid = %d\n", gid );

  gid = getgid();
  rtems_test_assert( gid == 5 );
  printf( "getgid = %d\n", gid );

  puts( "setegid(5)" );
  sc = setegid(5);
  rtems_test_assert( sc == 0 );

  gid = getegid();
  rtems_test_assert( gid == 5 );
  printf( "getegid = %d\n", gid );

  gid = getgid();
  rtems_test_assert( gid == 5 );
  printf( "getgid = %d\n", gid );

  puts( "setgid(0)" );
  sc = setgid(0);
  rtems_test_assert( sc == 0 );

  puts( "setegid(0)" );
  sc = setegid(0);
  rtems_test_assert( sc == 0 );

  errno = 0;
  puts( "setpgid(getpid(), 10) - ENOSYS" );
  sc = setpgid( getpid(), 10 );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == ENOSYS );
}

void test_uid(void)
{
  uid_t uid;
  int sc;

  uid = geteuid();
  rtems_test_assert( uid == 0 );
  printf( "geteuid = %d\n", uid );

  uid = getuid();
  rtems_test_assert( uid == 0 );
  printf( "getuid = %d\n", uid );

  puts( "setuid(5)" );
  sc = setuid(5);
  rtems_test_assert( sc == 0 );

  uid = geteuid();
  rtems_test_assert( uid == 0 );
  printf( "geteuid = %d\n", uid );

  uid = getuid();
  rtems_test_assert( uid == 5 );
  printf( "getuid = %d\n", uid );

  puts( "seteuid(5)" );
  sc = seteuid(5);
  rtems_test_assert( sc == 0 );

  uid = geteuid();
  rtems_test_assert( uid == 5 );
  printf( "geteuid = %d\n", uid );

  uid = getuid();
  rtems_test_assert( uid == 5 );
  printf( "getuid = %d\n", uid );

  puts( "seteuid(0)" );
  sc = seteuid(0);
  rtems_test_assert( sc == 0 );

  puts( "setuid(0)" );
  sc = setuid(0);
  rtems_test_assert( sc == 0 );
}

pid_t __getpid(void);
int issetugid(void);

void test_pid(void)
{
  pid_t pid;
  int   sc;

  pid = getpid();
  printf( "getpid = %d\n", pid );

  pid = __getpid();
  printf( "__getpid = %d\n", pid );

  pid = getppid();
  printf( "getppid = %d\n", pid );

  puts( "setsid - EPERM" );
  pid = setsid();
  rtems_test_assert( pid == -1 );
  rtems_test_assert( errno == EPERM );

  sc = issetugid();
  rtems_test_assert( sc == 0 );

  puts( "getpgrp - return local node - OK" );
  pid = getpgrp();
  printf( "getpgrp returned %d\n", pid ); 
}

void test_getlogin(void)
{
  int sc;
  char ch;

  puts( "setuid(5)" );
  sc = setuid(5);
  rtems_test_assert( sc == 0 );
  printf( "getlogin() -- (%s)\n", getlogin() );

  puts( "setuid(0)" );
  sc = setuid(0);
  rtems_test_assert( sc == 0 );
  printf( "getlogin() -- (%s)\n", getlogin() );

  puts( "getlogin_r(NULL, LOGIN_NAME_MAX) -- EFAULT" );
  sc = getlogin_r( NULL, LOGIN_NAME_MAX );
  rtems_test_assert( sc == EFAULT );

  puts( "getlogin_r(buffer, 0) -- ERANGE" );
  sc = getlogin_r( &ch, 0 );
  rtems_test_assert( sc == ERANGE );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  TEST_BEGIN();

  test_gid();
  puts( "" );

  test_uid();
  puts( "" );

  test_pid();
  puts( "" );

  test_getlogin();

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS                  1
/* so we can write /etc/passwd and /etc/group */
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
