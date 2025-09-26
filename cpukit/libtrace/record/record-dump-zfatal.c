/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020, 2024 embedded brains GmbH & Co. KG
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

#include <rtems/recorddump.h>

#include <rtems/bspIo.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/timecounter.h>

#if ISR_LOCK_NEEDS_OBJECT
static ISR_lock_Control _Record_Dump_base64_zlib_lock =
  ISR_LOCK_INITIALIZER( "Record Dump base64 zlib" );
#endif

static bool _Record_Dump_base64_zlib_done;

static rtems_record_dump_base64_zlib_context _Record_Dump_base64_zlib_context;

void _Record_Fatal_dump_base64_zlib(
  Internal_errors_Source source,
  bool                   always_set_to_false,
  Internal_errors_t      code
)
{
  (void) always_set_to_false;

  ISR_lock_Context     lock_context;
  Per_CPU_Control     *cpu_self;
  rtems_record_context record_context;
  sbintime_t           now;

  cpu_self = _Per_CPU_Get();

  now = _Timecounter_Sbinuptime();
  rtems_record_prepare_critical( &record_context, cpu_self );
  rtems_record_add(
    &record_context,
    RTEMS_RECORD_UPTIME_LOW,
    (uint32_t) ( now >> 0 )
  );
  rtems_record_add(
    &record_context,
    RTEMS_RECORD_UPTIME_HIGH,
    (uint32_t) ( now >> 32 )
  );
  rtems_record_commit_critical( &record_context );

  rtems_record_prepare_critical( &record_context, cpu_self );
  rtems_record_add(
    &record_context,
    RTEMS_RECORD_FATAL_SOURCE,
    source
  );
  rtems_record_add(
    &record_context,
    RTEMS_RECORD_FATAL_CODE,
    code
  );
  rtems_record_commit_critical( &record_context );

#if defined(RTEMS_SMP)
  if (
    source == RTEMS_FATAL_SOURCE_SMP &&
    code == SMP_FATAL_SHUTDOWN_RESPONSE
  ) {
    return;
  }

  /* Request other online processors to shutdown to stop event generation */
  _SMP_Request_shutdown();
#endif

  _ISR_lock_Acquire( &_Record_Dump_base64_zlib_lock, &lock_context );

  if ( !_Record_Dump_base64_zlib_done ) {
    _Record_Dump_base64_zlib_done = true;

    printk( "\n*** BEGIN OF RECORDS BASE64 ZLIB ***\n" );
    rtems_record_dump_zlib_base64(
      &_Record_Dump_base64_zlib_context,
      rtems_put_char,
      NULL
    );
    printk( "\n*** END OF RECORDS BASE64 ZLIB ***\n" );
  }

  _ISR_lock_Release( &_Record_Dump_base64_zlib_lock, &lock_context );
}
