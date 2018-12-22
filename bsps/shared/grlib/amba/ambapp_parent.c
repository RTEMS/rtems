/*
 *  AMBA Plug & Play routines
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <grlib/ambapp.h>

struct ambapp_dev *ambapp_find_parent(struct ambapp_dev *dev)
{
  while (dev->prev) {
    if (dev == dev->prev->children)
      return dev->prev;
    dev = dev->prev;
  }
  return NULL;
}
