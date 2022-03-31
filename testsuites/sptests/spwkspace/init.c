/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Exercise Classic API Workspace Wrappers
 *
 *  COPYRIGHT (c) 1989-2009.
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

#include <tmacros.h>

#include <string.h>

#include <rtems/score/wkspace.h>

const char rtems_test_name[] = "SPWKSPACE";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

static void test_workspace_string_duplicate(void)
{
  char a [] = "abcd";
  char b [] = "abc";
  char c [] = "ab";
  char d [] = "a";
  char e [] = "";
  size_t maxlen = 3;
  char *dup_a = _Workspace_String_duplicate( a, maxlen );
  char *dup_b = _Workspace_String_duplicate( b, maxlen );
  char *dup_c = _Workspace_String_duplicate( c, maxlen );
  char *dup_d = _Workspace_String_duplicate( d, maxlen );
  char *dup_e = _Workspace_String_duplicate( e, maxlen );

  rtems_test_assert( dup_a != NULL );
  rtems_test_assert( dup_b != NULL );
  rtems_test_assert( dup_c != NULL );
  rtems_test_assert( dup_d != NULL );
  rtems_test_assert( dup_e != NULL );
  rtems_test_assert( strcmp( dup_a, b ) == 0 );
  rtems_test_assert( strcmp( dup_b, b ) == 0 );
  rtems_test_assert( strcmp( dup_c, c ) == 0 );
  rtems_test_assert( strcmp( dup_d, d ) == 0 );
  rtems_test_assert( strcmp( dup_e, e ) == 0 );

  _Workspace_Free( dup_a );
  _Workspace_Free( dup_b );
  _Workspace_Free( dup_c );
  _Workspace_Free( dup_d );
  _Workspace_Free( dup_e );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  void                   *p1;
  bool                    retbool;
  Heap_Information_block  info;

  TEST_BEGIN();

  puts( "rtems_workspace_get_information - null pointer" );
  retbool = rtems_workspace_get_information( NULL );
  rtems_test_assert( retbool == false );

  puts( "rtems_workspace_get_information - OK" );
  retbool = rtems_workspace_get_information( &info );
  rtems_test_assert( retbool == true );

  puts( "rtems_workspace_allocate - null pointer" );
  retbool = rtems_workspace_allocate( 42, NULL );
  rtems_test_assert( retbool == false );

  puts( "rtems_workspace_allocate - 0 bytes" );
  retbool = rtems_workspace_allocate( 0, &p1 );
  rtems_test_assert( retbool == false );

  puts( "rtems_workspace_allocate - too many bytes" );
  retbool = rtems_workspace_allocate( info.Free.largest * 2, &p1 );
  rtems_test_assert( retbool == false );

  puts( "rtems_workspace_allocate - 42 bytes" );
  retbool = rtems_workspace_allocate( 42, &p1 );
  rtems_test_assert( retbool == true );
  rtems_test_assert( p1 != NULL );

  puts( "rtems_workspace_free - NULL" );
  retbool = rtems_workspace_free( NULL );
  rtems_test_assert( retbool == true );

  puts( "rtems_workspace_free - previous pointer to 42 bytes" );
  retbool = rtems_workspace_free( p1 );
  rtems_test_assert( retbool == true );

  puts( "_Workspace_String_duplicate - samples" );
  test_workspace_string_duplicate();

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_TASKS             1

#define CONFIGURE_MEMORY_OVERHEAD 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
