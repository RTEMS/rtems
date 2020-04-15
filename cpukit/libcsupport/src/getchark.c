/**
 *  @file
 *
 *  @brief Get Character from Stdin
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/bspIo.h>

int getchark(void)
{
  if ( BSP_poll_char )
    return (*BSP_poll_char)();

  return -1;
}
