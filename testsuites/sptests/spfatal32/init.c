/* SPDX-License-Identifier: BSD-2-Clause */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 * Copyright (c) 2018 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdlib.h>

#define FATAL_ERROR_TEST_NAME       "32"
#define FATAL_ERROR_DESCRIPTION     "invalid free of heap memory"
#define FATAL_ERROR_EXPECTED_SOURCE RTEMS_FATAL_SOURCE_INVALID_HEAP_FREE
#define FATAL_ERROR_EXPECTED_ERROR  1

/*
 * Disable for the specific test case.
 */
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"

static void force_error(void)
{
  uintptr_t invalid = 1;
  free((void *) invalid);
}

#include "../spfatal_support/spfatalimpl.h"
