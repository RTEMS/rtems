/**
 *  @file
 *
 *  @brief Write Character to Stream 
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/bspIo.h>

/**
 * Kernel putk (e.g. puts) function requiring minimal infrastrure.
 */
void putk(const char *s)
{
  const char *p;

  for (p=s ; *p ; p++ )
    BSP_output_char(*p);
  BSP_output_char('\n');
}
