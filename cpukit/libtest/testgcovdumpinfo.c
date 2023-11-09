/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of
 *   rtems_test_gcov_dump_info().
 */

/*
 * Copyright (C) 2021, 2023 embedded brains GmbH & Co. KG
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

#include <rtems/test-info.h>

#include <rtems/test-gcov.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/hash.h>
#include <rtems/bspIo.h>

ISR_LOCK_DEFINE( static, gcov_dump_lock, "gcov dump" );

static bool gcov_dump_done;

static Hash_Context gcov_hash;

static void gcov_put_char( int c, void *arg )
{
  uint8_t byte;

  rtems_put_char( c, arg );
  byte = (uint8_t) c;
  _Hash_Add_data( &gcov_hash, &byte, sizeof( byte ) );
}

void rtems_test_gcov_dump_info( void )
{
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &gcov_dump_lock, &lock_context );

  if ( !gcov_dump_done ) {
    Hash_Control result;
    uint8_t      byte;

    gcov_dump_done = true;

    _IO_Printf( rtems_put_char, NULL, "\n*** BEGIN OF GCOV INFO BASE64 ***\n" );
    _Hash_Initialize( &gcov_hash );
    _Gcov_Dump_info_base64( gcov_put_char, NULL );
    _IO_Printf( rtems_put_char, NULL, "\n*** END OF GCOV INFO BASE64 ***\n" );
    byte = '\n';
    _Hash_Add_data( &gcov_hash, &byte, sizeof( byte ) );
    _Hash_Finalize( &gcov_hash, &result );
    _IO_Printf(
      rtems_put_char,
      NULL,
      "*** GCOV INFO SHA256 %s ***\n",
      _Hash_Get_string( &result )
    );
  }

  _ISR_lock_Release_and_ISR_enable( &gcov_dump_lock, &lock_context );
}
