/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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
#include "test_support.h"
#include <rtems/libcsupport.h>
#include <rtems/malloc.h>
#include <errno.h>

const char rtems_test_name[] = "MALLOC 4";

/* configuration information */
/* At top of file to have access to configuration variables */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of configuration */

#ifndef CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
void *rtems_heap_null_extend(
  Heap_Control *heap,
  size_t alloc_size
)
{
  return rtems_heap_extend_via_sbrk( heap, alloc_size );
}
#endif

char Malloc_Heap[ 1024 ] CPU_STRUCTURE_ALIGNMENT;

/*
 * Use volatile to prevent compiler optimizations due to the malloc() builtin.
 */
volatile int sbrk_count;

Heap_Control TempHeap;

size_t offset;

void * sbrk(ptrdiff_t incr)
{
  void *p = (void *) -1;

  printf( "sbrk(%td)\n", incr );
  if ( sbrk_count == -1 ) {
    p = (void *) -2;
    sbrk_count = 0;
  } else if ( offset + incr <= sizeof(Malloc_Heap) ) {
    p = &Malloc_Heap[ offset ];
    offset += incr;
  }

  ++sbrk_count;

  return p;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  Heap_Control *real_heap;
  const Memory_Information *mem;
  Memory_Area *area;
  size_t i;

  void *p;

  TEST_BEGIN();

  mem = _Memory_Get();

  for ( i = 0; i < _Memory_Get_count( mem ); ++i ) {
    area = _Memory_Get_area( mem, i );
    _Memory_Initialize( area, NULL, NULL );
  }

  area = _Memory_Get_area( mem, 0 );

  /* Safe information on real heap */
  real_heap = malloc_get_heap_pointer();
  malloc_set_heap_pointer( &TempHeap );

  rtems_heap_set_sbrk_amount( 0 );

  puts( "No sbrk() amount" );

  sbrk_count = 0;
  offset     = 256;
  _Memory_Initialize_by_size( area, &Malloc_Heap[ 0 ], offset );
  _Malloc_Initialize();

  errno = 0;
  p = malloc( 256 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == ENOMEM );
  rtems_test_assert( sbrk_count == 0 );

  rtems_heap_set_sbrk_amount( 256 );

  puts( "Misaligned extend" );

  sbrk_count = 0;
  offset     = 256;
  _Memory_Initialize_by_size( area, &Malloc_Heap[ 0 ], offset );
  _Malloc_Initialize();

  p = malloc(1);
  rtems_test_assert( p != NULL );
  rtems_test_assert( sbrk_count == 0 );

  p = malloc(257);
  rtems_test_assert( p != NULL );
  rtems_test_assert( sbrk_count == 1 );

  puts( "Not enough sbrk() space" );

  sbrk_count = 0;
  offset     = 256;
  _Memory_Initialize_by_size( area, &Malloc_Heap[ 0 ], offset );
  _Malloc_Initialize();

  errno = 0;
  p = malloc( sizeof( Malloc_Heap ) );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == ENOMEM );
  rtems_test_assert( sbrk_count == 1 );

  puts( "Valid heap extend" );

  sbrk_count = 0;
  offset     = 256;
  _Memory_Initialize_by_size( area, &Malloc_Heap[ 0 ], offset );
  _Malloc_Initialize();

  p = malloc( 128 );
  rtems_test_assert( p != NULL );
  rtems_test_assert( sbrk_count == 0 );

  p = malloc( 128 );
  rtems_test_assert( p != NULL );
  rtems_test_assert( sbrk_count == 1 );

  puts( "Invalid heap extend" );

  sbrk_count = -1;
  offset     = 256;
  _Memory_Initialize_by_size( area, &Malloc_Heap[ 0 ], offset );
  _Malloc_Initialize();

  errno = 0;
  p = malloc( 256 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == ENOMEM );
  rtems_test_assert( sbrk_count == 2 );

  /* Restore information on real heap */
  malloc_set_heap_pointer( real_heap );

  TEST_END();

  rtems_test_exit(0);
}

/* end of file */
