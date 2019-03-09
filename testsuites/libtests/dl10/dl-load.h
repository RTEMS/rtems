/*
 * Copyright (c) 2014, 2018 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <string.h>

#if !defined(_DL_LOAD_H_)
#define _DL_LOAD_H_

static inline const char* dl_localise_file (const char* file)
{
  return (const char*) strstr (file, "testsuites");
}

int dl_load_test(void);

#endif
