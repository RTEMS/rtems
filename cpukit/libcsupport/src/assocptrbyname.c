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


const rtems_assoc_t *rtems_assoc_ptr_by_name(
  const rtems_assoc_t *ap,
  const char          *name
)
{
  const rtems_assoc_t *default_ap = 0;

  if (rtems_assoc_is_default(ap))
    default_ap = ap++;

  for ( ; ap->name; ap++)
    if (strcmp(ap->name, name) == 0)
	return ap;

  return default_ap;
}
