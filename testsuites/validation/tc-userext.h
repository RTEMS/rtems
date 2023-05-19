/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreInterrValTerminate
 *
 * @brief This header file provides functions used by corresponding test suite.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#ifndef _TC_USEREXT_H
#define _TC_USEREXT_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

void ThreadBeginExtension0( rtems_tcb *executing );

bool ThreadCreateExtension0( rtems_tcb *executing, rtems_tcb *created );

void ThreadDeleteExtension0( rtems_tcb *executing, rtems_tcb *deleted );

void ThreadExittedExtension0( rtems_tcb *executing );

void ThreadRestartExtension0( rtems_tcb *executing, rtems_tcb *restarted );

void ThreadStartExtension0( rtems_tcb *executing, rtems_tcb *started );

void ThreadSwitchExtension0( rtems_tcb *executing, rtems_tcb *heir );

void ThreadTerminateExtension0( rtems_tcb *executing );

void FatalExtension0(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
);

void ThreadBeginExtension1( rtems_tcb *executing );

bool ThreadCreateExtension1( rtems_tcb *executing, rtems_tcb *created );

void ThreadDeleteExtension1( rtems_tcb *executing, rtems_tcb *deleted );

void ThreadExittedExtension1( rtems_tcb *executing );

void ThreadRestartExtension1( rtems_tcb *executing, rtems_tcb *restarted );

void ThreadStartExtension1( rtems_tcb *executing, rtems_tcb *started );

void ThreadSwitchExtension1( rtems_tcb *executing, rtems_tcb *heir );

void ThreadTerminateExtension1( rtems_tcb *executing );

void FatalExtension1(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
);

#ifdef __cplusplus
}
#endif

#endif /* _TC_USEREXT_H */
