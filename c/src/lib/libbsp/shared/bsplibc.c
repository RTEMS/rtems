/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

extern rtems_configuration_table  BSP_Configuration;

void bsp_libc_init(
  void       *heap_start,
  uint32_t    heap_size,
  int         use_sbrk
)
{
    RTEMS_Malloc_Initialize( heap_start, heap_size, use_sbrk );

    /*
     *  Init the RTEMS libio facility to provide UNIX-like system
     *  calls for use by newlib (ie: provide open, close, etc)
     *  Uses malloc() to get area for the iops, so must be after malloc init
     */

    rtems_libio_init();

    /*
     * Set up for the libc handling.
     */

    if ( BSP_Configuration.ticks_per_timeslice > 0 )
        libc_init(1);                /* reentrant if possible */
    else
        libc_init(0);                /* non-reentrant */
}
