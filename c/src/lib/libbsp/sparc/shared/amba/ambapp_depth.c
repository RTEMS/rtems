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

/* Get bus depth a device is located at */
int ambapp_depth(struct ambapp_dev *dev)
{
  int depth = 0;

  do {
    dev = ambapp_find_parent(dev);
    depth++;
  } while (dev);

  return depth - 1;
}
