#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/* generate fatal errors in open_dev_console.c */

#define FATAL_ERROR_TEST_NAME            "14"
#define FATAL_ERROR_DESCRIPTION          "fail to open stdout"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_LIBIO_STDOUT_FD_OPEN_FAILED

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 1

static void force_error(void)
{
  /* we will not run this far */
}

#include "../spfatal_support/spfatalimpl.h"
