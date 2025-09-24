/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreIntErr
 *
 * @brief This source file contains the definition of ::_System_state_Current
 *   and the implementation of _Terminate() and _Internal_error().
 */

/*
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

#include <rtems/score/cpuimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/userextimpl.h>

System_state_Codes _System_state_Current;

void _Terminate(
  Internal_errors_Source the_source,
  Internal_errors_t      the_error
)
{
  ISR_Level level;

  /*
   * Make sure that interrupts don't interfere with the fatal error handling on
   * this processor.  This reduces the likelihood to end up in a recursive
   * fatal error handling sequence.
   */
  _ISR_Local_disable( level );
  (void) level;

  _User_extensions_Fatal( the_source, the_error );

  /*
   * Everything after invoking the fatal extensions is essentially dead code.
   * At least one fatal extension of the initial extension sets should not
   * return and for example reset the system.  See section "System Termination
   * Procedure" in the RTEMS Classic API Guide.
   *
   * The following code is only executed in badly configured applications.
   */
  _CPU_Thread_Idle_body( 0 );
}

void _Internal_error( Internal_errors_Core_list core_error )
{
  _Terminate( INTERNAL_ERROR_CORE, core_error );
}
