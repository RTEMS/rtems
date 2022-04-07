/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2007.
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

#define CONFIGURE_INIT
#include "system.h"
#include "tmacros.h"

#include <unistd.h>
#include <stdint.h>
#include <errno.h>

const char rtems_test_name[] = "PSXSYSCONF";

void *POSIX_Init(
  void *argument
)
{
  long  sc;

  TEST_BEGIN();

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

  rtems_test_assert(
    sysconf( _SC_NPROCESSORS_CONF )
      == (long) rtems_configuration_get_maximum_processors()
  );

  rtems_test_assert(
    sysconf( _SC_NPROCESSORS_ONLN )
      == (long) rtems_scheduler_get_processor_maximum()
  );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
