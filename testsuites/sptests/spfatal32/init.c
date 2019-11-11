#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdint.h>
#include <stdlib.h>

#define FATAL_ERROR_TEST_NAME       "32"
#define FATAL_ERROR_DESCRIPTION     "invalid free of heap memory"
#define FATAL_ERROR_EXPECTED_SOURCE RTEMS_FATAL_SOURCE_INVALID_HEAP_FREE
#define FATAL_ERROR_EXPECTED_ERROR  1

static void force_error(void)
{
  uintptr_t invalid = 1;
  free((void *) invalid);
}

#include "../spfatal_support/spfatalimpl.h"
