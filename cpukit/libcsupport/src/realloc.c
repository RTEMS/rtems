/**
 *  @file
 *
 *  @brief Reallocate Memory Block
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "malloc_p.h"

static void *new_alloc( void *old_ptr, size_t new_size, size_t old_size )
{
  void *new_ptr;

  /*
   *  There used to be a free on this error case but it is wrong to
   *  free the memory per OpenGroup Single UNIX Specification V2
   *  and the C Standard.
   */

  new_ptr = malloc( new_size );
  if ( new_ptr == NULL ) {
    return NULL;
  }

  memcpy( new_ptr, old_ptr, ( new_size < old_size ) ? new_size : old_size );
  free( old_ptr );

  return new_ptr;
}

void *realloc( void *ptr, size_t size )
{
  Heap_Control       *heap;
  Heap_Resize_status  status;
  uintptr_t           old_size;
  uintptr_t           avail_size;

  if ( size == 0 ) {
    free( ptr );
    return NULL;
  }

  if ( ptr == NULL ) {
    return malloc( size );
  }

  heap = RTEMS_Malloc_Heap;

  switch ( _Malloc_System_state() ) {
    case MALLOC_SYSTEM_STATE_NORMAL:
      _RTEMS_Lock_allocator();
      _Malloc_Process_deferred_frees();
      status = _Heap_Resize_block( heap, ptr, size, &old_size, &avail_size );
      _RTEMS_Unlock_allocator();
      break;
    case MALLOC_SYSTEM_STATE_NO_PROTECTION:
      status = _Heap_Resize_block( heap, ptr, size, &old_size, &avail_size );
      break;
    default:
      return NULL;
  }

  switch ( status ) {
    case HEAP_RESIZE_SUCCESSFUL:
      return ptr;
    case HEAP_RESIZE_UNSATISFIED:
      return new_alloc( ptr, size, old_size );
    default:
      errno = EINVAL;
      return NULL;
  }
}
#endif
