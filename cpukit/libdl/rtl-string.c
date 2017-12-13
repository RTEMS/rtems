/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker String managment.
 */

#include <string.h>

#include <rtems/rtl/rtl-allocator.h>
#include "rtl-string.h"

char*
rtems_rtl_strdup (const char *s1)
{
  size_t len = strlen (s1);
  char*  s2 = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT, len + 1, false);
  if (s2)
  {
    memcpy (s2, s1, len);
    s2[len] = '\0';
  }
  return s2;
}
