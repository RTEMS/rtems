/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
  Memory_Area area;
  Memory_Information mem = {
    .count = 1,
    .areas = &area
  };

  void *p;

  TEST_BEGIN();

  /* Safe information on real heap */
  real_heap = malloc_get_heap_pointer();
  malloc_set_heap_pointer( &TempHeap );

  rtems_heap_set_sbrk_amount( 0 );

  puts( "No sbrk() amount" );

  sbrk_count = 0;
  offset     = 256;
  _Memory_Initialize_by_size( &area, &Malloc_Heap[ 0 ], offset );
  RTEMS_Malloc_Initialize( &mem, NULL );

  errno = 0;
  p = malloc( 256 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == ENOMEM );
  rtems_test_assert( sbrk_count == 0 );

  rtems_heap_set_sbrk_amount( 256 );

  puts( "Misaligned extend" );

  sbrk_count = 0;
  offset     = 256;
  _Memory_Initialize_by_size( &area, &Malloc_Heap[ 0 ], offset );
  RTEMS_Malloc_Initialize( &mem, NULL );

  p = malloc(1);
  rtems_test_assert( p != NULL );
  rtems_test_assert( sbrk_count == 0 );

  p = malloc(257);
  rtems_test_assert( p != NULL );
  rtems_test_assert( sbrk_count == 1 );

  puts( "Not enough sbrk() space" );

  sbrk_count = 0;
  offset     = 256;
  _Memory_Initialize_by_size( &area, &Malloc_Heap[ 0 ], offset );
  RTEMS_Malloc_Initialize( &mem, NULL );

  errno = 0;
  p = malloc( sizeof( Malloc_Heap ) );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == ENOMEM );
  rtems_test_assert( sbrk_count == 1 );

  puts( "Valid heap extend" );

  sbrk_count = 0;
  offset     = 256;
  _Memory_Initialize_by_size( &area, &Malloc_Heap[ 0 ], offset );
  RTEMS_Malloc_Initialize( &mem, NULL );

  p = malloc( 128 );
  rtems_test_assert( p != NULL );
  rtems_test_assert( sbrk_count == 0 );

  p = malloc( 128 );
  rtems_test_assert( p != NULL );
  rtems_test_assert( sbrk_count == 1 );

  puts( "Invalid heap extend" );

  sbrk_count = -1;
  offset     = 256;
  _Memory_Initialize_by_size( &area, &Malloc_Heap[ 0 ], offset );
  RTEMS_Malloc_Initialize( &mem, NULL );

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
