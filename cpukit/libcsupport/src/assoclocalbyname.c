/**
 * @file
 *
 * @brief RTEMS Associate Local by Name
 * @ingroup Associativity Routines
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define INSIDE_ASSOC

#include <rtems.h>
#include <rtems/assoc.h>

#include <string.h>             /* strcat, strcmp */

uint32_t rtems_assoc_local_by_name(
  const rtems_assoc_t *ap,
  const char          *name
)
{
  const rtems_assoc_t *nap;

  nap = rtems_assoc_ptr_by_name(ap, name);
  if (nap)
    return nap->local_value;

  return 0;
}
