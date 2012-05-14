/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

#if UNUSED
/* FIXME: This test doesn't make sense.
 * On targets with sizeof(int) < sizeof(long), compilation will fail,
 * On targets with sizeof(int) == sizeof(long) the call is valid.
 */
  puts( "sysconf -- bad configuration parameter - too large" );
  sc = sysconf( LONG_MAX );
  fatal_posix_service_status_errno( sc, EINVAL, "bad conf name" );
#endif

  sc = sysconf( _SC_CLK_TCK );
  printf( "sysconf - _SC_CLK_TCK=%ld\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);

  sc = sysconf( _SC_OPEN_MAX );
  printf( "sysconf - _SC_OPEN_MAX=%ld\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);

  sc = sysconf( _SC_GETPW_R_SIZE_MAX );
  printf( "sysconf - _SC_GETPW_R_SIZE_MAX=%ld\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);

  sc = sysconf( _SC_PAGESIZE );
  printf( "sysconf - _SC_PAGESIZE=%ld\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);

  sc = getpagesize();
  printf( "getpagesize = %ld\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);

  sc = sysconf( INT_MAX );
  printf( "sysconf - bad parameter = %ld errno=%s\n", sc, strerror(errno) );
  if ( (sc != -1) || (errno != EINVAL) )
   rtems_test_exit(0);

#if defined(__sparc__)
  /* Solaris _SC_STACK_PROT - 515 */
  sc = sysconf( _SC_PAGESIZE );
  printf( "sysconf - (SPARC only) _SC_STACK_PROT=%ld\n", sc );
  if ( sc == -1 )
   rtems_test_exit(0);
#endif

  puts( "*** END OF POSIX TEST SYSCONF ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
