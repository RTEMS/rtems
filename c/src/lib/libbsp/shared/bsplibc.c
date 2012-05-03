/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

#include <bsp/bootcard.h>

void bsp_libc_init(
  void *heap_begin,
  uintptr_t heap_size,
  size_t sbrk_amount
)
{
    RTEMS_Malloc_Initialize( heap_begin, heap_size, sbrk_amount );

    /*
     *  Init the RTEMS libio facility to provide UNIX-like system
     *  calls for use by newlib (ie: provide open, close, etc)
     *  Uses malloc() to get area for the iops, so must be after malloc init
     */
    if (rtems_libio_init_helper)
	(*rtems_libio_init_helper)();

    /*
     * Set up for the libc handling.
     */
    libc_init();
}
