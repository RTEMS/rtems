/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup psxtests
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#define CONFIGURE_INIT
#include "system.h"
#include <signal.h>
#include <errno.h>
#include <reent.h>

const char rtems_test_name[] = "PSXSIGNAL 9";

static void Handler_1( int signo )
{
  TEST_END();
  rtems_test_exit(0);
}

void *POSIX_Init( void *argument )
{
  int              status;
  struct sigaction act;

  TEST_BEGIN();

  /* Hook signals that can be generated from machine exceptions */
  act.sa_handler = Handler_1;
  act.sa_flags   = 0;
  status = sigaction( SIGFPE, &act, NULL );
  rtems_test_assert( !status );
  status = sigaction( SIGILL, &act, NULL );
  rtems_test_assert( !status );
  status = sigaction( SIGSEGV, &act, NULL );
  rtems_test_assert( !status );

  /* Generate machine exception */
  _CPU_Instruction_illegal();

  return NULL;
}
