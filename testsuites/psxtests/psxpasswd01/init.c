/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <pmacros.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

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

  puts( "*** PASSWORD/GROUP TEST - 01 ***" );

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

  puts( "Init - getpwent() (2) -- OK" );
  pw = getpwent();
  rtems_test_assert( pw );
  print_passwd( pw );

  puts( "Init - getpwent() (3) -- OK" );
  pw = getpwent();
  rtems_test_assert( pw );
  print_passwd( pw );

  puts( "Init - getpwent() (4) -- result should be NULL" );
  pw = getpwent();
  rtems_test_assert( !pw );

  /* getgrent */

  puts( "Init - getgrent() (1) -- OK" );
  gr = getgrent();
  rtems_test_assert( gr );
  print_group( gr );

  puts( "Init - getgrent() (2) -- OK" );
  gr = getgrent();
  rtems_test_assert( gr );
  print_group( gr );

  puts( "Init - getgrent() (3) -- OK" );
  gr = getgrent();
  rtems_test_assert( gr );
  print_group( gr );

  puts( "Init - getgrent() (4) -- result should be NULL" );
  gr = getgrent();
  rtems_test_assert( !gr );

  /* getpwnam */
  puts( "Init - getpwnam(\"root\") -- OK" );
  pw = getpwnam( "root" );
  rtems_test_assert( pw );
  print_passwd( pw );

  puts( "Init - getpwnam(\"rtems\") -- OK" );
  pw = getpwnam( "rtems" );
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

  puts( "Init - getpwuid(1) -- OK" );
  pw = getpwuid( 1 );
  rtems_test_assert( pw );
  print_passwd( pw );

  puts( "Init - getpwuid(4) -- result should be NULL" );
  pw = getpwuid( 4 );
  rtems_test_assert( !pw );

  /* getgrnam */
  puts( "Init - getgrnam(\"root\") -- OK" );
  gr = getgrnam("root");
  rtems_test_assert( gr );
  print_group( gr );

  puts( "Init - getgrnam(\"rtems\") -- OK" );
  gr = getgrnam("rtems");
  rtems_test_assert( gr );
  print_group( gr );

  /* getgrgid */
  puts( "Init - getgrgid(0) -- OK" );
  gr = getgrgid(0);
  rtems_test_assert( gr );
  print_group( gr );

  puts( "Init - getgrgid(1) -- OK" );
  gr = getgrgid(1);
  rtems_test_assert( gr );
  print_group( gr );

  puts( "Init - getgrgid(4) -- result should be NULL");
  gr = getgrgid( 4 );
  rtems_test_assert( !gr );

  /* endpwent */
  puts( "Init - endpwent() -- OK" );
  endpwent();

  /* endgrent */
  puts( "Init - endgrent() -- OK" );
  endgrent();

  puts( "*** END OF PASSWORD/GROUP TEST - 01 ***" );
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 6

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
