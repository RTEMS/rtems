/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <pmacros.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

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

  puts( "*** PASSWORD/GROUP TEST ***" );

  /* getpwnam */
  puts( "Init - getpwnam(\"root\") -- OK" );
  pw = getpwnam("root");
  rtems_test_assert( pw );
  print_passwd( pw );

  puts( "Init - getpwnam(\"rtems\") -- OK" );
  pw = getpwnam("rtems");
  rtems_test_assert( pw );
  print_passwd( pw );

  /* getgrnam */
  puts( "Init - getgrnam(\"root\") -- OK" );
  gr = getgrnam("root");
  rtems_test_assert( gr );
  print_group( gr );

  puts( "Init - getgrnam(\"rtems\") -- OK" );
  gr = getgrnam("rtems");
  rtems_test_assert( gr );
  print_group( gr );

  puts( "*** END OF PASSWORD/GROUP TEST ***" );
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
