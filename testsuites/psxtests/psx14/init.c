/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems.h>
#include <rtems/score/objectimpl.h>
#include "tmacros.h"

const char rtems_test_name[] = "PSX 14";

void *POSIX_Init(
  void *argument
)
{
  char               name[128];
  char              *ptr;
  rtems_status_code  status;

  TEST_BEGIN();

  ptr = rtems_object_get_name( pthread_self(), 128, name );
  printf( "rtems_object_get_name returned (%s) for init thread\n", ptr );

  /* Set my name to Justin */
  puts( "Setting current thread name to Justin" );
  status = rtems_object_set_name( pthread_self(), "Justin" );
  directive_failed( status, "rtems_object_set_name" );

  ptr = rtems_object_get_name( pthread_self(), 128, name );
  printf( "rtems_object_get_name returned (%s) for init thread\n", ptr );

  /* Set my name to Jordan */
  puts( "Setting current thread name to Jordan" );
  status = rtems_object_set_name( pthread_self(), "Jordan" );
  directive_failed( status, "rtems_object_set_name" );

  ptr = rtems_object_get_name( pthread_self(), 128, name );
  printf( "rtems_object_get_name returned (%s) for init thread\n", ptr );

  /* exercise the POSIX path through some routines */
  printf( "rtems_object_api_minimum_class(OBJECTS_POSIX_API) returned %d\n",
          rtems_object_api_minimum_class(OBJECTS_POSIX_API) );
  printf( "rtems_object_api_maximum_class(OBJECTS_POSIX_API) returned %d\n",
          rtems_object_api_maximum_class(OBJECTS_POSIX_API) );

  printf( "rtems_object_get_api_name(POSIX_API) = %s\n",
     rtems_object_get_api_name(OBJECTS_POSIX_API) );

  printf("rtems_object_get_api_class_name(POSIX_API, POSIX_KEYS) = %s\n",
    rtems_object_get_api_class_name( OBJECTS_POSIX_API, OBJECTS_POSIX_KEYS)
  );


  TEST_END();
  rtems_test_exit( 0 );

  return NULL;

}
