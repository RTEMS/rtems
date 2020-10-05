/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dl-load.h"
#include "dl-o5.h"

#include <inttypes.h>
#include <rtems/test-info.h>

int rtems_main_o5 (void)
{
  /* duplicate symbol in archive */
  return 0;
}
