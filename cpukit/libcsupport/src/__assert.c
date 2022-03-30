/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Evaluate Assertion
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 2007-2013.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/bspIo.h>
#include <rtems.h>

#if defined(RTEMS_NEWLIB) && !defined(HAVE___ASSERT_FUNC)
#include <assert.h>

/**
 * Newlib 1.16.0 added this method.  Together these provide an
 * RTEMS safe, low memory implementation.
 */
void __assert_func(
  const char *file,
  int         line,
  const char *func,
  const char *failedexpr
)
{
  rtems_assert_context assert_context = {
    .file = file,
    .line = line,
    .function = func,
    .failed_expression = failedexpr
  };

  printk("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
    failedexpr,
    file,
    line,
    (func) ? ", function: " : "",
    (func) ? func : ""
  );
  rtems_fatal( RTEMS_FATAL_SOURCE_ASSERT, (rtems_fatal_code) &assert_context );
}
#endif
