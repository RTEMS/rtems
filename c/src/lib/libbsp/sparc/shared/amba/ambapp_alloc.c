/*
 *  AMBA Plug & Play routines
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <ambapp.h>

int ambapp_alloc_dev(struct ambapp_dev *dev, void *owner)
{
  if (dev->owner)
    return -1;
  dev->owner = owner;
  return 0;
}

void ambapp_free_dev(struct ambapp_dev *dev)
{
  dev->owner = 0;
}
