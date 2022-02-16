/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief POSIX Function Installs signal Handler
 */

/*
 *  signal(2) - Install signal handler
 *
 *  COPYRIGHT (c) 1989-1999.
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

#include <signal.h>
#include <errno.h>

#ifndef HAVE_SIGHANDLER_T
  typedef void (*sighandler_t)(int);
#endif

sighandler_t signal(
  int           signum,
  sighandler_t  handler
)
{
  struct sigaction s;
  struct sigaction old;

  s.sa_handler = handler ;
  sigemptyset(&s.sa_mask);

  /*
   *  Depending on which system we want to behave like, one of
   *  the following versions should be chosen.
   */

/* #define signal_like_linux */

#if defined(signal_like_linux)
  s.sa_flags   = SA_RESTART | SA_INTERRUPT | SA_NOMASK;
  s.sa_restorer= NULL ;
#elif defined(signal_like_SVR4)
  s.sa_flags   = SA_RESTART;
#else
  s.sa_flags   = 0;
#endif

  sigaction( signum, &s, &old );
  return (sighandler_t) old.sa_handler;
}
