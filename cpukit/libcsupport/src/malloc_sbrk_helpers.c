/*
 *  RTEMS Malloc -- SBRK Support Plugin
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h> /* sbrk */

#include <rtems.h>
#include <rtems/malloc.h>
#include "malloc_p.h"

#include <errno.h>

size_t  RTEMS_Malloc_Sbrk_amount;

static void *malloc_sbrk_initialize(
  void  *starting_address,
  size_t length
)
{
  uintptr_t     old_address;
  uintptr_t     uaddress;

  RTEMS_Malloc_Sbrk_amount = length;

  /*
   * If the starting address is 0 then we are to attempt to
   * get length worth of memory using sbrk. Make sure we
   * align the address that we get back.
   */

  if (!starting_address) {
    uaddress = (uintptr_t)sbrk(length);

    if (uaddress == (uintptr_t) -1) {
      rtems_fatal_error_occurred( RTEMS_NO_MEMORY );
      /* DOES NOT RETURN!!! */
    }

    if (uaddress & (CPU_HEAP_ALIGNMENT-1)) {
      old_address = uaddress;
      uaddress = (uaddress + CPU_HEAP_ALIGNMENT) & ~(CPU_HEAP_ALIGNMENT-1);

      /*
       * adjust the length by whatever we aligned by
       */
      length -= uaddress - old_address;
    }

    starting_address = (void *)uaddress;
  }
  return starting_address;
}

static void *malloc_sbrk_extend_and_allocate(
  size_t size
)
{
  uint32_t  sbrk_amount;
  void     *starting_address;
  uint32_t  the_size;
  void     *return_this;

  /*
   *  Round to the "requested sbrk amount" so hopefully we won't have
   *  to grow again for a while.  This effectively does sbrk() calls
   *  in "page" amounts.
   */

  sbrk_amount = RTEMS_Malloc_Sbrk_amount;

  if ( sbrk_amount == 0 )
    return (void *) 0;

  the_size = ((size + sbrk_amount) / sbrk_amount * sbrk_amount);

  starting_address = (void *) sbrk(the_size);
  if ( starting_address == (void*) -1 )
    return (void *) 0;

  if ( !_Protected_heap_Extend(
          RTEMS_Malloc_Heap, starting_address, the_size) ) {
    sbrk(-the_size);
    errno = ENOMEM;
    return (void *) 0;
  }

  MSBUMP(space_available, the_size);

  return_this = _Protected_heap_Allocate( RTEMS_Malloc_Heap, size );
  return return_this;
}


rtems_malloc_sbrk_functions_t rtems_malloc_sbrk_helpers_table = {
  malloc_sbrk_initialize,
  malloc_sbrk_extend_and_allocate
};
