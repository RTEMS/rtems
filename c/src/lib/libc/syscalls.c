#if !defined(RTEMS_UNIX)

/*
 *  RTEMS Fake System Calls
 *
 *  This file contains "fake" versions of the system call routines
 *  which are reference by many libc implementations.  Once a routine
 *  has been implemented in terms of RTEMS services, it should be
 *  taken out of this file.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

/*
 *  fstat, stat, and isatty must lie consistently and report that everything
 *  is a tty or stdout will not be line buffered.
 */

int __fstat(int _fd, struct stat* _sbuf)
{
  if ( _fd > 2 ) {
    puts( "__fstat -- only stdio supported" );
    assert( 0 );
  }
  _sbuf->st_mode = S_IFCHR;
#ifdef HAVE_BLKSIZE
  _sbuf->st_blksize = 0;
#endif
  return 0;
}

int __isatty(int _fd)
{
  return 1;
}

int stat( const char *path, struct stat *buf )
{
  if ( strncmp( "/dev/", path, 5 ) ) {
    return -1;
  }
  return __fstat( 0, buf );
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

#endif
