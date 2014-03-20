/*
 *  This is a shared BSP post driver hook designed to open
 *  /dev/console for stdin, stdout, and stderr if it exists.
 *  Newlib will automatically associate the file descriptors
 *  with the first three files opened.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/libio.h>

#include <bsp/bootcard.h>

void bsp_postdriver_hook(void)
{
  (*rtems_libio_post_driver_helper)();
}
