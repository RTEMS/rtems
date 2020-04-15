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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/bspIo.h>

/**
 * Kernel putk (e.g. puts) function requiring minimal infrastrure.
 */
int putk(const char *s)
{
  const char *p;
  int len_out = 0;

  for (p=s ; *p ; p++, len_out++ )
    rtems_putc(*p);
  rtems_putc('\n');
  return len_out + 1;
}
