/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <unistd.h>
#include <limits.h>   /* for LOGIN_NAME_MAX */

int test( void );

int test( void )
{
  char  loginnamebuffer[ LOGIN_NAME_MAX ];
  int   result;

  result = getlogin_r( loginnamebuffer, LOGIN_NAME_MAX );

  return result;
}
