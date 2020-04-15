/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2014, 2019 embedded brains GmbH
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

#include <rtems/config.h>
#include <rtems/score/stack.h>
#include <rtems/score/tls.h>
#include <rtems/score/thread.h>

uintptr_t rtems_configuration_get_stack_space_size( void )
{
  uintptr_t space_size;

  space_size = _Stack_Space_size;

  /*
   * In case we have a non-zero TLS size, then we need a TLS area for each
   * thread.  These areas are allocated within the stack area from the stack
   * space.  Ensure that the stack space is large enough to fulfill all requests
   * known at configuration time (so excluding the unlimited option).  It is not
   * possible to estimate the TLS size in the configuration at compile-time.
   * The TLS size is determined at application link-time.
   */
  space_size += _Thread_Initial_thread_count * _TLS_Get_allocation_size();

  return space_size;
}
