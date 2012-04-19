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

/* Get number of devices matching search options */
int ambapp_dev_count(struct ambapp_bus *abus, unsigned int options,
      int vendor, int device)
{
  int count = 10000;

  ambapp_for_each(abus, options, vendor, device, ambapp_find_by_idx, &count);

  return 10000 - count;
}
