/* SPDX-License-Identifier: BSD-2-Clause */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*  Test __assert_func with NULL function name
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
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

#include <assert.h>
#include <string.h>

#define FATAL_ERROR_TEST_NAME            "10"
#define FATAL_ERROR_DESCRIPTION          "asserting with non-NULL strings..."
#define FATAL_ERROR_EXPECTED_SOURCE      RTEMS_FATAL_SOURCE_ASSERT
#define FATAL_ERROR_EXPECTED_ERROR_CHECK spfatal10_is_expected_error

#define ASSERT_FILE "testcase.h"
#define ASSERT_LINE 38
#define ASSERT_FUNC "Init"
#define ASSERT_FEXP "forced"

static inline bool spfatal10_is_expected_error( rtems_fatal_code error )
{
  const rtems_assert_context *assert_context =
    (const rtems_assert_context *) error;

  return strcmp( assert_context->file, ASSERT_FILE ) == 0
    && assert_context->line == ASSERT_LINE
    && strcmp( assert_context->function, ASSERT_FUNC ) == 0
    && strcmp( assert_context->failed_expression, ASSERT_FEXP ) == 0;
}

static void force_error(void)
{
  __assert_func( ASSERT_FILE, ASSERT_LINE, ASSERT_FUNC, ASSERT_FEXP );

  /* we will not run this far */
}

#include "../spfatal_support/spfatalimpl.h"
