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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

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
  int status = -1;
  char str[100] = {0};
  int max_int = INT_MAX;

  FILE *fp = NULL;

  puts( "*** PASSWORD/GROUP TEST - 02 ***" );

  puts( "Init - Creating /etc" );
  status = mkdir( "/etc", 0777 );
  rtems_test_assert( status == 0 );
  
  puts( "Init - Creating /etc/passwd" );
  status = mknod( "/etc/passwd", (S_IFREG | S_IRWXU), 0LL );
  rtems_test_assert( status != -1 );

  fp = fopen( "/etc/passwd", "w" );
  rtems_test_assert( fp != NULL );
  fprintf( fp, "bharath:x:-1:-a:Dummy::/:/bin/bash\n" );
  fclose( fp );

  puts( "Init - Creating /etc/group" );
  status = mknod( "/etc/group", (S_IFREG | S_IRWXU), 0LL );
  rtems_test_assert( status != -1);

  fp = fopen( "/etc/group", "w" );
  rtems_test_assert( fp != NULL );
  fprintf( fp, "admin::1:root,su,super-user\n" );
  fclose( fp );

  puts( "Init - setpwent() -- OK" );
  setpwent();

  puts( "Init - setpwent() -- OK" );
  setpwent();

  puts( "Init - setgrent() -- OK" );
  setgrent();

  puts( "Init - setgrent() -- OK" );
  setgrent();  
 
  puts( "Init - getpwnam(\"root\") -- expected EINVAL" );
  pw = getpwnam( "root" );
  rtems_test_assert( !pw );
  rtems_test_assert( errno == EINVAL );

  fp = fopen( "/etc/passwd", "w" );
  rtems_test_assert( fp != NULL );
  fprintf( fp, "rtems:x:1:1:dummy::/:/bin/bash:" );
  fclose( fp );

  puts( "Init - getpwnam(\"root\") -- expected EINVAL" );
  pw = getpwnam( "root" );
  rtems_test_assert( !pw ); 
  rtems_test_assert( errno == EINVAL );

  fp = fopen( "/etc/passwd", "w" );
  rtems_test_assert( fp != NULL );
  fprintf( fp, "user\n:x:2:2:dummy::/:/bin/sh\n" );
  fclose( fp );

  puts( "Init - getpwnam(\"root\") -- expected EINVAL" );
  pw = getpwnam( "root" );
  rtems_test_assert( !pw ); 
  rtems_test_assert( errno == EINVAL );

  fp = fopen( "/etc/passwd", "w" );
  rtems_test_assert( fp != NULL );
  fprintf( fp, "user:x:999999999999:1:dummy::/:/bin/sh\n" );
  fclose( fp );

  puts( "Init - getpwnam(\"root\") -- expected EINVAL" );
  pw = getpwnam( "root" );
  rtems_test_assert( !pw ); 
  rtems_test_assert( errno == EINVAL );

  sprintf( str, "user:x:%d%d:1:dummy::/:/bin/sh\n", max_int/10, max_int%10+1 );

  fp = fopen( "/etc/passwd", "w" );
  rtems_test_assert( fp != NULL );
  fprintf( fp, str );
  fclose( fp );

  puts( "Init - getpwnam(\"root\") -- expected EINVAL" );
  pw = getpwnam( "root" );
  rtems_test_assert( !pw ); 
  rtems_test_assert( errno == EINVAL );

  fp = fopen( "/etc/passwd", "w" );
  rtems_test_assert( fp != NULL );
  fprintf( fp, "\
    ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ\
    ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ\
    ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ\
    ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ\
    ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ\
    ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ\
    :x:999999999999:1:dummy::/:/bin/sh\n" );
  fclose( fp );

  puts( "Init - getpwnam(\"root\") -- expected EINVAL" );
  pw = getpwnam( "root" );
  rtems_test_assert( !pw ); 
  rtems_test_assert( errno == EINVAL );

  puts( "Init - getgrent() -- OK" );
  gr = getgrent();
  rtems_test_assert( gr != NULL );
  print_group( gr );

  puts( "Init - endpwent() -- OK" );
  endpwent();

  puts( "Init - endpwent() -- OK" );
  endpwent();

  puts( "Init - endgrent() -- OK" );
  endgrent();

  puts( "Init - endgrent() -- OK" );
  endgrent();

  puts( "Removing /etc/passwd" );
  status = unlink( "/etc/passwd" );
  rtems_test_assert( status == 0 );

  puts( "Removing /etc/group" );
  status = unlink( "/etc/group" );
  rtems_test_assert( status == 0 );

  puts( "Init - getpwnam(\"root\") -- expected EINVAL" );
  pw = getpwnam( "root" );
  rtems_test_assert( !pw );
  rtems_test_assert( errno == EINVAL );

  puts( "Init - getgrnam(\"root\") -- expected EINVAL" );
  gr = getgrnam( "root" );
  rtems_test_assert( !gr );
  rtems_test_assert( errno == EINVAL );
  
  puts( "*** END OF PASSWORD/GROUP TEST - 02 ***" );
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
