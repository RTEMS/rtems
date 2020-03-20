/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief ARM7M CPU Initialize
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 2011 Sebastian Huber
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

#include <rtems/score/armv7m.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void _CPU_Initialize( void )
{
  /*
   * The exception handler used to carry out the thead dispatching must have
   * the lowest priority possible.  No other exception handlers must have this
   * priority if they use services that may lead to a thread dispatch.  See
   * also "ARMv7-M Architecture Reference Manual, Issue D" section B1.5.4
   * "Exception priorities and preemption".
   */
  _ARMV7M_Set_exception_priority(
    ARMV7M_VECTOR_SVC,
    ARMV7M_EXCEPTION_PRIORITY_LOWEST
  );
  _ARMV7M_Set_exception_priority(
    ARMV7M_VECTOR_PENDSV,
    ARMV7M_EXCEPTION_PRIORITY_LOWEST
  );
}

#endif /* ARM_MULTILIB_ARCH_V7M */
