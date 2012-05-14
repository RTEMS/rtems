/*  fatal.h
 *
 *  Copyright (c) 1998, National Research Council of Canada
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/score/interr.h>
#include <rtems/score/userext.h>

User_extensions_routine bsp_fatal_error_occurred(
  Internal_errors_Source  the_source,
  bool                    is_internal,
  uint32_t                the_error
);
