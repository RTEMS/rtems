/*
 *  RTEMS Malloc Family Implementation
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems.h>
#ifdef RTEMS_LIBC
#include <memory.h>
#endif
#include "libcsupport.h"
#ifdef RTEMS_NEWLIB
#include <sys/reent.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

/* 
 *  XXX: Do we really need to duplicate these? It appears that they
 *       only cause typing problems.
 */

#if 0
void *malloc(size_t);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);
void free(void *);
void *sbrk(size_t);
#endif

rtems_id RTEMS_Malloc_Heap;
size_t RTEMS_Malloc_Sbrk_amount;

void RTEMS_Malloc_Initialize(
  void   *start,
  size_t  length,
  size_t  sbrk_amount
)
{
  rtems_status_code   status;
  void               *starting_address;
  rtems_unsigned32    u32_address;

  /*
   * If the starting address is 0 then we are to attempt to
   * get length worth of memory using sbrk. Make sure we
   * align the address that we get back.
   */

  starting_address = start;

  if (!starting_address) {
    u32_address = (unsigned int)sbrk(length);

    if (u32_address == -1) {
      rtems_fatal_error_occurred( RTEMS_NO_MEMORY );
      /* DOES NOT RETURN!!! */
    }

    if (u32_address & (CPU_ALIGNMENT-1)) {
      u32_address = (u32_address + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);
      /* XXX: if we do any alignment .. then length should be shortened */
    }

    starting_address = (void *)u32_address;
  }

  /*
   *  Unfortunately we cannot use assert if this fails because if this
   *  has failed we do not have a heap and if we do not have a heap
   *  STDIO cannot work because there will be no buffers.
   */

  status = rtems_region_create(
    rtems_build_name( 'H', 'E', 'A', 'P' ),
    starting_address,
    length,
    8,                     /* XXX : use CPU dependent RTEMS constant */
    RTEMS_DEFAULT_ATTRIBUTES,
    &RTEMS_Malloc_Heap
  );
  if ( status != RTEMS_SUCCESSFUL )
    rtems_fatal_error_occurred( status );
}

void *malloc(
  size_t  size
)
{
  void              *return_this;
  void              *starting_address;
  rtems_unsigned32   the_size;
  rtems_unsigned32   sbrk_amount;
  rtems_status_code  status;

  if ( !size )
    return (void *) 0;

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

    if (((rtems_unsigned32)starting_address = sbrk(the_size)) == -1)
      return (void *) 0;

    /*
    fprintf(stderr, "Extended the C heap starting at 0x%x for %d bytes\n",
        (unsigned32)starting_address, the_size);
     */

    status = rtems_region_extend(
      RTEMS_Malloc_Heap,
      starting_address,
      the_size
    );
    if ( status != RTEMS_SUCCESSFUL ) {
      sbrk(-the_size);
      return(FALSE);
      errno = ENOMEM;
      return (void *) 0;
    }
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

  return return_this;
}

void *calloc(
  size_t nelem,
  size_t elsize
)
{
  register char *cptr;
  int length;

  length = nelem * elsize;
  cptr = malloc( length );
  if ( cptr )
    memset( cptr, '\0', length );

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

  if ( !ptr )
    return malloc( size );

  if ( !size ) {
    free( ptr );
    return (void *) 0;
  }

  status = rtems_region_get_segment_size( RTEMS_Malloc_Heap, ptr, &old_size );
  if ( status != RTEMS_SUCCESSFUL ) {
    errno = EINVAL;
    return (void *) 0;
  }

  new_area = malloc( size );
  if ( !new_area ) {
    free( ptr );
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

  if ( !ptr )
    return;

  status = rtems_region_return_segment( RTEMS_Malloc_Heap, ptr );
  if ( status != RTEMS_SUCCESSFUL ) {
    errno = EINVAL;
    assert( 0 );
  }
}

/*
 *  "Reentrant" versions of the above routines implemented above.
 */

#ifdef RTEMS_NEWLIB
void *malloc_r(
  struct _reent *ignored,
  size_t  size
)
{
  return malloc( size );
}

void *calloc_r(
  size_t nelem,
  size_t elsize
)
{
  return calloc( nelem, elsize );
}

void *realloc_r(
  void *ptr,
  size_t size
)
{
  return realloc_r( ptr, size );
}

void free_r(
  void *ptr
)
{
  free( ptr );
}
#endif

