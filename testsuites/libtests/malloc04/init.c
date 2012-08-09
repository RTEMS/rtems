/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"
#include <rtems/libcsupport.h>
#include <rtems/malloc.h>
#include <errno.h>

/* configuration information */
/* At top of file to have access to configuration variables */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
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

char Malloc_Heap[ 128 ] CPU_STRUCTURE_ALIGNMENT;
int sbrk_count;
Heap_Control TempHeap;

size_t offset;

void * sbrk(ptrdiff_t incr)
{
  void *p = (void *) -1;

  printf( "sbrk(%td)\n", incr );
  if ( sbrk_count == -1 ) {
    p = (void *) (NULL - 2);
  } else if ( offset + incr < sizeof(Malloc_Heap) ) {
     p = &Malloc_Heap[ offset ];
     offset += incr;
  } else {
    if ( sbrk_count == 0 )
      p = (void *) rtems_task_create;
    sbrk_count++;
  }

  sbrk_count++;
  return p;
}

void *p1, *p2, *p3, *p4;

rtems_task Init(
  rtems_task_argument argument
)
{
  Heap_Control *real_heap;
  Heap_Area area;

  sbrk_count = 0;
  offset     = 0;

  puts( "\n\n*** TEST MALLOC 04 ***" );

  /* Safe information on real heap */
  real_heap = malloc_get_heap_pointer();
  malloc_set_heap_pointer( &TempHeap );

  rtems_heap_set_sbrk_amount( 64 );

  puts( "Initialize heap with some memory" );
  offset     = 64;
  sbrk_count = 0;
  area.begin = &Malloc_Heap [0];
  area.size = 64;
  RTEMS_Malloc_Initialize( &area, 1, NULL );
  p1 = malloc(64);
  p2 = malloc(64);
  p3 = malloc(48);
  p4 = malloc(48);
  
  puts( "Initialize heap with some memory - return address out of heap" );
  area.begin = &Malloc_Heap [1];
  area.size = 64;
  RTEMS_Malloc_Initialize( &area, 1, NULL );
  offset     = 64;
  sbrk_count = -1;
  p1 = malloc( 127 );
  rtems_test_assert( p1 == (void *) NULL );
  rtems_test_assert( errno == ENOMEM );
  

  area.begin = &Malloc_Heap [0];
  area.size = 64;
  RTEMS_Malloc_Initialize( &area, 1, NULL );
  puts( "Initialize heap with some unaligned memory" );
  offset     = 65;
  sbrk_count = 0;
  area.begin = &Malloc_Heap [1];
  area.size = 64;
  RTEMS_Malloc_Initialize( &area, 1, NULL );
  p1 = malloc(64);
  p2 = malloc(64);
  p3 = malloc(48);

  /* Restore information on real heap */
  malloc_set_heap_pointer( real_heap );

  puts( "*** END OF TEST MALLOC 04 ***" );

  rtems_test_exit(0);
}

/* end of file */
