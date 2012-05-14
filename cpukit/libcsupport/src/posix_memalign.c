/*
 *  posix_memalign()
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef RTEMS_NEWLIB
#include "malloc_p.h"

#include <stdlib.h>
#include <errno.h>

int posix_memalign(
  void   **pointer,
  size_t   alignment,
  size_t   size
)
{
  /*
   *  Update call statistics
   */
  MSBUMP(memalign_calls, 1);

  if (((alignment - 1) & alignment) != 0 || (alignment < sizeof(void *)))
    return EINVAL;

  /*
   *  rtems_memalign does all of the error checking work EXCEPT
   *  for adding restrictionso on the alignment.
   */
  return rtems_memalign( pointer, alignment, size );
}
#endif
