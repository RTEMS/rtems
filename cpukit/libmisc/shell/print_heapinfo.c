/*
 *  Print Heap Information Structure
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#include <rtems.h>
#include <rtems/shell.h>
#include "internal.h"

void rtems_shell_print_heap_info(
  const char       *c,
  Heap_Information *h
)
{
  printf(
    "Number of %s blocks: %" PRId32 "\n"
    "Largest %s block:    %" PRId32 "\n"
    "Total bytes %s:      %" PRId32 "\n",
    c, h->number,
    c, h->largest,
    c, h->total
  );
}
