 /**
 * @file src/sup_fs_get_sym_start_loc.c
 */

/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

/*
 *  rtems_filesystem_get_sym_start_loc
 *
 *  Function to determine if path is absolute or relative
 *
 *  Parameters:
 *
 *  path : IN  - path to be checked
 *  index: OUT - 0, if relative, 1 if absolute
 *  loc  : OUT - location info of root fs if absolute
 *               location info of current fs if relative
 *
 *  Returns: void
 */

/* Includes */

#include "rtems/libio_.h"
  
void rtems_filesystem_get_sym_start_loc(const char *path,
					int *index,
					rtems_filesystem_location_info_t *loc)
{
  if (rtems_filesystem_is_separator(path[0])) {
      *loc = rtems_filesystem_root;
      *index = 1;
    }
    else {
      *index = 0;
    }
}
