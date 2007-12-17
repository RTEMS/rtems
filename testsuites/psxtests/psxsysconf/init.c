/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include "tmacros.h"

#include <unistd.h>
#include <stdint.h>
#include <errno.h>

void *POSIX_Init(
  void *argument
)
{
  long  sc;

  puts( "\n\n*** POSIX TEST -- SYSCONF ***" );

  puts( "sysconf -- bad configuration parameter - negative" );
  sc = sysconf( -1 );
  fatal_posix_service_status_errno( sc, EINVAL, "bad conf name" );
  
  puts( "sysconf -- bad configuration parameter - too large" );
  sc = sysconf( LONG_MAX );
  fatal_posix_service_status_errno( sc, EINVAL, "bad conf name" );
  
  sc = sysconf( _SC_CLK_TCK );
  printf( "sysconf - _SC_CLK_TCK=%d\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);

  sc = sysconf( _SC_OPEN_MAX );
  printf( "sysconf - _SC_OPEN_MAX=%d\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);

  sc = sysconf( _SC_GETPW_R_SIZE_MAX );
  printf( "sysconf - _SC_GETPW_R_SIZE_MAX=%d\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);

  sc = sysconf( _SC_PAGESIZE );
  printf( "sysconf - _SC_PAGESIZE=%d\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);

#if defined(__sparc__)
  /* Solaris _SC_STACK_PROT - 515 */
  sc = sysconf( _SC_PAGESIZE );
  printf( "sysconf - (SPARC only) _SC_STACK_PROT=%d\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);
#endif

  puts( "*** END OF POSIX TEST SYSCONF ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
