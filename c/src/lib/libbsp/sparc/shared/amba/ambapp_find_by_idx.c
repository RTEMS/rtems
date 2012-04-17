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

/* AMBAPP helper routine to find a device by index. The function is given to
 * ambapp_for_each, the argument may be NULL (find first device) or a pointer
 * to a index which is downcounted until 0 is reached. If the int-pointer
 * points to a value of:
 *   0  - first device is returned
 *   1  - second device is returned
 *   ...
 *
 * The matching device is returned, which will stop the ambapp_for_each search.
 * If zero is returned from ambapp_for_each no device matching the index was
 * found
 */
int ambapp_find_by_idx(struct ambapp_dev *dev, int index, void *pcount)
{
  int *pi = pcount;

  if (pi) {
    if (*pi-- == 0)
      return (int)dev;
    else
      return 0;
  } else {
    /* Satisfied with first matching device, stop search */
    return (int)dev;
  }
}
