/*
 *  RTEMS Fake System Calls
 *
 *  This file contains "fake" versions of the system call routines
 *  which are reference by many libc implementations.  Once a routine
 *  has been implemented in terms of RTEMS services, it should be
 *  taken out of this file.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>  /* only for puts */

#include <rtems.h>

#ifdef RTEMS_NEWLIB
/*
 *  fstat, stat, and isatty must lie consistently and report that everything
 *  is a tty or stdout will not be line buffered.
 */

int __rtems_fstat(int _fd, struct stat* _sbuf)
{
  if ( _fd > 2 ) {
    puts( "__rtems_fstat -- only stdio supported" );
    assert( 0 );
  }
  _sbuf->st_mode = S_IFCHR;
#ifdef HAVE_BLKSIZE
  _sbuf->st_blksize = 0;
#endif
  return 0;
}

int __rtems_isatty(int _fd)
{
  return 1;
}

#if !defined(RTEMS_UNIX)
int stat( const char *path, struct stat *buf )
{
  if ( strncmp( "/dev/", path, 5 ) ) {
    return -1;
  }
  return __rtems_fstat( 0, buf );
}

int link( const char *existing, const char *new )
{
  /* always fail */
  return -1;
}

int unlink( const char *path )
{
  /* always fail */
  return -1;
}

char *getcwd( char *_buf, size_t _size)
{

/*  assert( FALSE ); */
  errno = ENOSYS;
  return 0;
}
int fork() {
  puts( "fork -- not supported!!!" );
  assert( 0 );
  errno = ENOSYS;
  return -1;
}
int execv(const char *_path, char * const _argv[] ) {
  puts( "execv -- not supported!!!" ); 
  assert( 0 );
  errno = ENOSYS;
  return -1;
}
int wait() {
  puts( "wait -- not supported!!!" );
  assert( 0 );
  errno = ENOSYS;
  return -1;
}
#endif

#endif
