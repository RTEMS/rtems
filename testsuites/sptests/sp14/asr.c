/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#include "system.h"
#include "tmacros.h"

rtems_asr Process_asr(
  rtems_signal_set the_signal_set
)
{
  rtems_status_code status;

  printf(
    "ASR - ENTRY - signal => %08" PRIxrtems_signal_set "\n",
     the_signal_set
  );
  switch( the_signal_set ) {
    case RTEMS_SIGNAL_16:
    case RTEMS_SIGNAL_17:
    case RTEMS_SIGNAL_18 | RTEMS_SIGNAL_19:
      break;
    case RTEMS_SIGNAL_0:
    case RTEMS_SIGNAL_1:
      puts( "ASR - rtems_task_wake_after - yield processor" );
      status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
      directive_failed( status, "rtems_task_wake_after yield" );
      break;
    case RTEMS_SIGNAL_3:
      Asr_fired = TRUE;
      break;
  }
  printf(
    "ASR - EXIT  - signal => %08" PRIxrtems_signal_set "\n",
     the_signal_set
  );
}
