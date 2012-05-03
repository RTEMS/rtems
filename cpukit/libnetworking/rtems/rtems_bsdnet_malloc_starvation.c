/*
 *  Routine called when malloc() is not succeeding.  This can be overridden
 *  by a BSP.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <rtems/rtems_bsdnet.h>

void
rtems_bsdnet_malloc_starvation(void)
{
  printf ("rtems_bsdnet_malloc still waiting.\n");
}
