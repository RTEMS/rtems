/* SPDX-License-Identifier: BSD-2-Clause */

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

  if ( ptr == NULL ) {
    return malloc( size );
  }

  if ( size == 0 ) {
    free( ptr );
    return NULL;
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
