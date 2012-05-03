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

char Malloc_Heap[ 128 ] CPU_STRUCTURE_ALIGNMENT;
int sbrk_count;
Heap_Control Heap_Holder;
Heap_Control TempHeap;

/* Heap variables we need to peek and poke at */
extern size_t  RTEMS_Malloc_Sbrk_amount;

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
  Heap_Control *TempHeap;

  sbrk_count = 0;
  offset     = 0;

  puts( "\n\n*** TEST MALLOC 04 ***" );

  /* Safe information on real heap */
  TempHeap = malloc_get_heap_pointer();
  Heap_Holder = *TempHeap;
  rtems_malloc_sbrk_helpers = &rtems_malloc_sbrk_helpers_table;

  puts( "Initialize heap with some memory" );
  offset     = 64;
  sbrk_count = 0;
  RTEMS_Malloc_Initialize( Malloc_Heap, 64, 64 );
  p1 = malloc(64);
  p2 = malloc(64);
  p3 = malloc(48);
  p4 = malloc(48);
  
  puts( "Initialize heap with some memory - return address out of heap" );
  RTEMS_Malloc_Initialize( &Malloc_Heap[1], 64, 64 );
  offset     = 64;
  sbrk_count = -1;
  p1 = malloc( 127 );
  rtems_test_assert( p1 == (void *) NULL );
  rtems_test_assert( errno == ENOMEM );
  

  RTEMS_Malloc_Initialize( Malloc_Heap, 64, 64 );
  puts( "Initialize heap with some unaligned memory" );
  offset     = 65;
  sbrk_count = 0;
  RTEMS_Malloc_Initialize( &Malloc_Heap[1], 64, 64 );
  p1 = malloc(64);
  p2 = malloc(64);
  p3 = malloc(48);

  /* Restore information on real heap */
  malloc_set_heap_pointer( TempHeap );
  rtems_malloc_sbrk_helpers = NULL;

  puts( "*** END OF TEST MALLOC 04 ***" );

  rtems_test_exit(0);
}

/* end of file */
