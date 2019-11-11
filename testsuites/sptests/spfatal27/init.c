#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstrasse 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "27"
#define FATAL_ERROR_DESCRIPTION          "libio init no posix key left"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_LIBIO_USER_ENV_KEY_CREATE_FAILED

#define CONFIGURE_MAXIMUM_POSIX_KEYS (-1)
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS (0)

static void force_error(void)
{
  /* we should not reach this */
}

#include "../spfatal_support/spfatalimpl.h"
