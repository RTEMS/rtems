/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief ada supplmental functions
 */

/*
 * COPYRIGHT (C) 1989-2010 On-Line Applications Research Corporation (OAR).
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

#include <errno.h>
#include <pthread.h>

#include <rtems/config.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/score/stackimpl.h>

/*
 *  _ada_pthread_minimum_stack_size
 *
 *  This routine returns the minimum stack size so the GNAT RTS can
 *  allocate enough stack for Ada tasks.
 */

size_t _ada_pthread_minimum_stack_size( void )
{
  /*
   *  Eventually this may need to include a per cpu family calculation
   *  but for now, this will do.
   */

  return PTHREAD_MINIMUM_STACK_SIZE * 2;
}

uint32_t _ada_microseconds_per_tick(void)
{
  return rtems_configuration_get_microseconds_per_tick();
}
