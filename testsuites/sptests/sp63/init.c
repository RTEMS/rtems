/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
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

#include <rtems/score/heapimpl.h>

const char rtems_test_name[] = "SP 63";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void test_case_one(void);
void test_case_two(void);
void test_case_three(void);

uint32_t      Memory[256];
Heap_Control  Heap;

/*
 *  Exercise case in heapresize.c around line 125 when new_block_size
 *  < min_block_size
 */
void test_case_one(void)
{
  uint32_t           heap_size;
  void              *ptr1;
  uintptr_t          old;
  uintptr_t          avail;
  Heap_Resize_status hc;

  puts( "Init - _Heap_Initialize (for test one) - OK" );
  heap_size = _Heap_Initialize( &Heap, Memory, sizeof(Memory), 8 );
  printf( "Init - Heap size=%" PRIu32 "\n", heap_size );
  rtems_test_assert( heap_size );

  puts( "Init - _Heap_Allocate - too large size (overflow)- not OK");
  ptr1 = _Heap_Allocate( &Heap, UINTPTR_MAX );
  rtems_test_assert( !ptr1 );

  puts( "Init - _Heap_Allocate_aligned - OK");
  ptr1 = _Heap_Allocate_aligned( &Heap, 64, 32 );
  rtems_test_assert( ptr1 );

  puts( "Init - _Heap_Resize_block - OK");
  hc = _Heap_Resize_block( &Heap, ptr1, 4, &old, &avail );
  rtems_test_assert( !hc );
}

/*
 *  Exercise case in heapresize.c around line 140 when next_is_used AND
 *  free_block_size < min_block_size.
 */
void test_case_two(void)
{
  uint32_t           heap_size;
  void              *ptr1;
  uintptr_t          old;
  uintptr_t          avail;
  Heap_Resize_status hc;

  puts( "\nInit - _Heap_Initialize (for test two) - OK" );
  heap_size = _Heap_Initialize( &Heap, Memory, sizeof(Memory), 8 );
  printf( "Init - Heap size=%" PRIu32 "\n", heap_size );
  rtems_test_assert( heap_size );

  puts( "Init - _Heap_Allocate_aligned - OK");
  ptr1 = _Heap_Allocate_aligned( &Heap, 64, 4 );
  rtems_test_assert( ptr1 );

  puts( "Init - _Heap_Resize_block - OK");
  hc = _Heap_Resize_block( &Heap, ptr1, 56, &old, &avail );
  rtems_test_assert( !hc );
}

/*
 *  Exercise case in heapallocatealigned.c around line 223 when ...
 */
void test_case_three(void)
{
  uint32_t           heap_size;
  void              *ptr1;
#if 0
  Heap_Resize_status hc;
#endif
  int pg, al, alloc, sz;

  puts( "Init - _Heap_Allocate_aligned - request impossible - not OK");

#if 0
  heap_size =
     _Heap_Initialize( &Heap, Memory[32], sizeof(Memory), 1 << 16 );
  ptr1 = _Heap_Allocate_aligned( &Heap, 4, 1 << 16 );
  ptr1 = _Heap_Allocate_aligned( &Heap, 256, 1 << 16 );
#endif
#if 1
  for ( sz=32 ; sz <= 80 ; sz+=4 ) {
    for ( pg=2 ; pg < 12 ; pg++ ) {

      for ( al=16 ; al >=4 ; al-- ) {
        for ( alloc=4 ; alloc < sizeof(Memory)/2  ; alloc+=4 ) {
          heap_size =
            _Heap_Initialize( &Heap, &Memory[sz], sizeof(Memory)/2, 1 << pg );
          if ( heap_size != 0 ) {
            do {
              ptr1 = _Heap_Allocate_aligned( &Heap, alloc, 1 <<al );
            } while ( ptr1 );
          }
        }
      }
   }
 }
#endif
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  (void) ignored;

  TEST_BEGIN();

  test_case_one();

  test_case_two();

  test_case_three();

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_MAXIMUM_REGIONS       1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
