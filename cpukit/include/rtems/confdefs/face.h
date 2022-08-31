/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplApplConfig
 *
 * @brief This header file evaluates configuration options related to
 *   the FACE Technical Standard.
 *
 * The FACE Technical Standard (https://opengroup.org/face) is an
 * open standard designed for safety critical embedded systems. It
 * includes POSIX profiles and requirements that promote safety
 * and portability. As a general rules, the profiles place a minimum
 * on the services which an operating system must provide. Those
 * same profile definitions represent the maximum services which
 * an application may use. 
 */

/*
 * Copyright (C) 2022 On-Line Applications Research Corporation (OAR)
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

#ifndef _RTEMS_CONFDEFS_FACE_H
#define _RTEMS_CONFDEFS_FACE_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#ifdef CONFIGURE_POSIX_TIMERS_FACE_BEHAVIOR

  #include <rtems/posix/timer.h>

  int _POSIX_Timer_Is_allowed(
    clockid_t clock_id
  )
  {
    /*
     * Per the FACE Technical Standard, POSIX timers should not be
     * allowed on CLOCK_REALTIME for safety reasons. If the application
     * wants the FACE behavior, then this method is instantiated.
     */
    if (  clock_id == CLOCK_REALTIME ) {
      return EPERM;
    }

    if ( clock_id != CLOCK_MONOTONIC ) {
      return EINVAL;
    }

    return 0;
  }

#endif /* CONFIGURE_POSIX_TIMERS_FACE_BEHAVIOR */

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_FACE_H */
