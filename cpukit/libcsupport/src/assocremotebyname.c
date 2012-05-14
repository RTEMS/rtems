/*
 * assoc.c
 *      rtems assoc routines
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define INSIDE_ASSOC

#include <rtems.h>
#include <rtems/assoc.h>

#include <string.h>             /* strcat, strcmp */

uint32_t rtems_assoc_remote_by_name(
  const rtems_assoc_t *ap,
  const char          *name
)
{
  const rtems_assoc_t *nap;

  nap = rtems_assoc_ptr_by_name(ap, name);
  if (nap)
    return nap->remote_value;

  return 0;
}
