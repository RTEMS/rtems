/*
 *  RTEMS Malloc Family Implementation
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/score/apimutex.h>
#ifdef RTEMS_NEWLIB
#include <sys/reent.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>    /* sbrk(2) */

#include <chain.h>

Chain_Control RTEMS_Malloc_GC_list;

rtems_id RTEMS_Malloc_Heap;
size_t RTEMS_Malloc_Sbrk_amount;

#ifdef RTEMS_DEBUG
#define MALLOC_STATS
#define MALLOC_DIRTY
#endif

#ifdef MALLOC_STATS
#define MSBUMP(f,n)    rtems_malloc_stats.f += (n)

struct {
    unsigned32  space_available;             /* current size of malloc area */
    unsigned32  malloc_calls;                /* # calls to malloc */
    unsigned32  free_calls;
    unsigned32  realloc_calls;
    unsigned32  calloc_calls;
    unsigned32  max_depth;		     /* most ever malloc'd at 1 time */
    unsigned64  lifetime_allocated;
    unsigned64  lifetime_freed;
} rtems_malloc_stats;

#else			/* No rtems_malloc_stats */
#define MSBUMP(f,n)
#endif

void RTEMS_Malloc_Initialize(
  void   *start,
  size_t  length,
  size_t  sbrk_amount
)
{
  rtems_status_code   status;
  void               *starting_address;
  rtems_unsigned32    old_address;
  rtems_unsigned32    u32_address;

  /*
   *  Initialize the garbage collection list to start with nothing on it.
   */
  Chain_Initialize_empty(&RTEMS_Malloc_GC_list);

  /*
   * If the starting address is 0 then we are to attempt to
   * get length worth of memory using sbrk. Make sure we
   * align the address that we get back.
   */

  starting_address = start;
  RTEMS_Malloc_Sbrk_amount = sbrk_amount;

  if (!starting_address) {
    u32_address = (unsigned int)sbrk(length);

    if (u32_address == (rtems_unsigned32) -1) {
      rtems_fatal_error_occurred( RTEMS_NO_MEMORY );
      /* DOES NOT RETURN!!! */
    }

    if (u32_address & (CPU_HEAP_ALIGNMENT-1)) {
      old_address = u32_address;
      u32_address = (u32_address + CPU_HEAP_ALIGNMENT) & ~(CPU_HEAP_ALIGNMENT-1);

       /*
	* adjust the length by whatever we aligned by
	*/

      length -= u32_address - old_address;
    }

    starting_address = (void *)u32_address;
  }

  /*
   *  If the BSP is not clearing out the workspace, then it is most likely
   *  not clearing out the initial memory for the heap.  There is no 
   *  standard supporting zeroing out the heap memory.  But much code
   *  with UNIX history seems to assume that memory malloc'ed during
   *  initialization (before any free's) is zero'ed.  This is true most
   *  of the time under UNIX because zero'ing memory when it is first
   *  given to a process eliminates the chance of a process seeing data
   *  left over from another process.  This would be a security violation.
   */

  if ( rtems_cpu_configuration_get_do_zero_of_workspace() )
     memset( starting_address, 0, length );

  /*
   *  Unfortunately we cannot use assert if this fails because if this
   *  has failed we do not have a heap and if we do not have a heap
   *  STDIO cannot work because there will be no buffers.
   */

  status = rtems_region_create(
    rtems_build_name( 'H', 'E', 'A', 'P' ),
    starting_address,
    length,
    CPU_HEAP_ALIGNMENT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &RTEMS_Malloc_Heap
  );
  if ( status != RTEMS_SUCCESSFUL )
    rtems_fatal_error_occurred( status );

#ifdef MALLOC_STATS
  /* zero all the stats */
  (void) memset( &rtems_malloc_stats, 0, sizeof(rtems_malloc_stats) );
#endif
  
  MSBUMP(space_available, length);
}

#ifdef RTEMS_NEWLIB
void *malloc(
  size_t  size
)
{
  void              *return_this;
  void              *starting_address;
  rtems_unsigned32   the_size;
  rtems_unsigned32   sbrk_amount;
  rtems_status_code  status;
  Chain_Node        *to_be_freed;

  MSBUMP(malloc_calls, 1);

  if ( !size )
    return (void *) 0;

  /*
   *  Do not attempt to allocate memory if in a critical section or ISR.
   */

  if (_System_state_Is_up(_System_state_Get())) {
    if (_Thread_Dispatch_disable_level > 0)
      return (void *) 0;
 
    if (_ISR_Nest_level > 0)
      return (void *) 0;
  }
 
  /*
   *  If some free's have been deferred, then do them now.
   */
  while ((to_be_freed = Chain_Get(&RTEMS_Malloc_GC_list)) != NULL)
    free(to_be_freed);

  /*
   * Try to give a segment in the current region if there is not
   * enough space then try to grow the region using rtems_region_extend().
   * If this fails then return a NULL pointer.
   */

  status = rtems_region_get_segment(
    RTEMS_Malloc_Heap,
    size,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &return_this
  );

  if ( status != RTEMS_SUCCESSFUL ) {
    /*
     *  Round to the "requested sbrk amount" so hopefully we won't have
     *  to grow again for a while.  This effectively does sbrk() calls
     *  in "page" amounts.
     */

    sbrk_amount = RTEMS_Malloc_Sbrk_amount;

    if ( sbrk_amount == 0 )
      return (void *) 0;

    the_size = ((size + sbrk_amount) / sbrk_amount * sbrk_amount);

    if (((rtems_unsigned32)starting_address = (void *)sbrk(the_size)) 
            == (rtems_unsigned32) -1)
      return (void *) 0;

    status = rtems_region_extend(
      RTEMS_Malloc_Heap,
      starting_address,
      the_size
    );
    if ( status != RTEMS_SUCCESSFUL ) {
      sbrk(-the_size);
      errno = ENOMEM;
      return (void *) 0;
    }
    
    MSBUMP(space_available, the_size);

    status = rtems_region_get_segment(
      RTEMS_Malloc_Heap,
       size,
       RTEMS_NO_WAIT,
       RTEMS_NO_TIMEOUT,
       &return_this
    );
    if ( status != RTEMS_SUCCESSFUL ) {
      errno = ENOMEM;
      return (void *) 0;
    }
  }

#ifdef MALLOC_STATS
  if (return_this)
  {
      unsigned32 actual_size;
      unsigned32 current_depth;
      status = rtems_region_get_segment_size(
                   RTEMS_Malloc_Heap, return_this, &actual_size);
      MSBUMP(lifetime_allocated, actual_size);
      current_depth = rtems_malloc_stats.lifetime_allocated -
                   rtems_malloc_stats.lifetime_freed;
      if (current_depth > rtems_malloc_stats.max_depth)
          rtems_malloc_stats.max_depth = current_depth;
  }
#endif

#ifdef MALLOC_DIRTY
  (void) memset(return_this, 0xCF, size);
#endif

  return return_this;
}

void *calloc(
  size_t nelem,
  size_t elsize
)
{
  register char *cptr;
  int length;

  MSBUMP(calloc_calls, 1);

  length = nelem * elsize;
  cptr = malloc( length );
  if ( cptr )
    memset( cptr, '\0', length );

  MSBUMP(malloc_calls, -1);   /* subtract off the malloc */

  return cptr;
}

void *realloc(
  void *ptr,
  size_t size
)
{
  rtems_unsigned32  old_size;
  rtems_status_code status;
  char *new_area;

  MSBUMP(realloc_calls, 1);

  /*
   *  Do not attempt to allocate memory if in a critical section or ISR.
   */

  if (_System_state_Is_up(_System_state_Get())) {
    if (_Thread_Dispatch_disable_level > 0)
      return (void *) 0;
 
    if (_ISR_Nest_level > 0)
      return (void *) 0;
  }
 
  /*
   * Continue with calloc().
   */
  if ( !ptr )
    return malloc( size );

  if ( !size ) {
    free( ptr );
    return (void *) 0;
  }

  new_area = malloc( size );
 
  MSBUMP(malloc_calls, -1);   /* subtract off the malloc */

  /*
   *  There used to be a free on this error case but it is wrong to 
   *  free the memory per OpenGroup Single UNIX Specification V2
   *  and the C Standard.
   */

  if ( !new_area ) {
    return (void *) 0;
  }

  status = rtems_region_get_segment_size( RTEMS_Malloc_Heap, ptr, &old_size );
  if ( status != RTEMS_SUCCESSFUL ) {
    errno = EINVAL;
    return (void *) 0;
  }

  memcpy( new_area, ptr, (size < old_size) ? size : old_size );
  free( ptr );

  return new_area;

}

void free(
  void *ptr
)
{
  rtems_status_code status;

  MSBUMP(free_calls, 1);

  if ( !ptr )
    return;

  /*
   *  Do not attempt to free memory if in a critical section or ISR.
   */

  if (_System_state_Is_up(_System_state_Get())) {
    if ((_Thread_Dispatch_disable_level > 0) || (_ISR_Nest_level > 0)) {
      Chain_Append(&RTEMS_Malloc_GC_list, (Chain_Node *)ptr);
      return;
    }
  }
 
#ifdef MALLOC_STATS
  {
      unsigned32        size;
      status = rtems_region_get_segment_size( RTEMS_Malloc_Heap, ptr, &size );
      if ( status == RTEMS_SUCCESSFUL ) {
          MSBUMP(lifetime_freed, size);
      }
  }
#endif
  
  status = rtems_region_return_segment( RTEMS_Malloc_Heap, ptr );
  if ( status != RTEMS_SUCCESSFUL ) {
    errno = EINVAL;
    assert( 0 );
  }
}
/* end if RTEMS_NEWLIB */
#endif

#ifdef MALLOC_STATS
/*
 * Dump the malloc statistics
 * May be called via atexit()  (installable by our bsp) or
 * at any time by user
 */

void malloc_dump(void)
{
    unsigned32 allocated = rtems_malloc_stats.lifetime_allocated -
                     rtems_malloc_stats.lifetime_freed;

    printf("Malloc stats\n");
    printf("  avail:%uk  allocated:%uk (%d%%) "
              "max:%uk (%d%%) lifetime:%Luk freed:%Luk\n",
           (unsigned int) rtems_malloc_stats.space_available / 1024,
           (unsigned int) allocated / 1024,
           /* avoid float! */
           (allocated * 100) / rtems_malloc_stats.space_available,
           (unsigned int) rtems_malloc_stats.max_depth / 1024,
           (rtems_malloc_stats.max_depth * 100) / rtems_malloc_stats.space_available,
           (unsigned64) rtems_malloc_stats.lifetime_allocated / 1024,
           (unsigned64) rtems_malloc_stats.lifetime_freed / 1024);
    printf("  Call counts:   malloc:%d   free:%d   realloc:%d   calloc:%d\n",
           rtems_malloc_stats.malloc_calls,
           rtems_malloc_stats.free_calls,
           rtems_malloc_stats.realloc_calls,
           rtems_malloc_stats.calloc_calls);
}


void malloc_walk(size_t source, size_t printf_enabled)
{
   register Region_Control *the_region;
   Objects_Locations        location;
 
  _RTEMS_Lock_allocator();                      /* to prevent deletion */
   the_region = _Region_Get( RTEMS_Malloc_Heap, &location );
   if ( location == OBJECTS_LOCAL )
   {
      _Heap_Walk( &the_region->Memory, source, printf_enabled );
   }
  _RTEMS_Unlock_allocator();
}

#else

void malloc_dump(void)
{
   return;
}
 
void malloc_walk(size_t source, size_t printf_enabled)
{
   return;
}

#endif

/*
 *  "Reentrant" versions of the above routines implemented above.
 */

#ifdef RTEMS_NEWLIB
void *_malloc_r(
  struct _reent *ignored,
  size_t  size
)
{
  return malloc( size );
}

void *_calloc_r(
  struct _reent *ignored,
  size_t nelem,
  size_t elsize
)
{
  return calloc( nelem, elsize );
}

void *_realloc_r(
  struct _reent *ignored,
  void *ptr,
  size_t size
)
{
  return realloc( ptr, size );
}

void _free_r(
  struct _reent *ignored,
  void *ptr
)
{
  free( ptr );
}

#endif
