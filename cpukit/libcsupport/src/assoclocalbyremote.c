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

uint32_t rtems_assoc_local_by_remote(
  const rtems_assoc_t *ap,
  uint32_t             remote_value
)
{
  const rtems_assoc_t *nap;

  nap = rtems_assoc_ptr_by_remote(ap, remote_value);
  if (nap)
    return nap->local_value;

  return 0;
}
