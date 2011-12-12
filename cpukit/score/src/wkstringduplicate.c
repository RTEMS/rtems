/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/wkspace.h>

#include <string.h>

char *_Workspace_String_duplicate(
  const char *string,
  size_t maxlen
)
{
  size_t n = strnlen(string, maxlen);
  char *dup = _Workspace_Allocate(n + 1);

  if (dup != NULL) {
    dup [n] = '\0';
    memcpy(dup, string, n);
  }

  return dup;
}
