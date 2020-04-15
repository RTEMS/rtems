/**
 * @file
 *
 * @brief Run TOD Hook Set
 *
 * @ingroup RTEMSScoreTODHooks
 */

/*
 *  SPDX-License-Identifier: BSD-2-Clause
 *
 *  COPYRIGHT (c) 2019.
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

#include <rtems/score/todimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>

Status_Control _TOD_Hook_Run(
  TOD_Action             action,
  const struct timespec *tod
)
{
  const Chain_Node *the_node;
  Status_Control    status;

  status = STATUS_SUCCESSFUL;

  /*
   * This is assumed to be called only from _TOD_Set() which is supposed
   * to be called only while holding the TOD lock.
   */
  _Assert( _TOD_Is_owner() );

  for (
    the_node = _Chain_Immutable_first( &_TOD_Hooks );
    !_Chain_Is_tail( &_TOD_Hooks, the_node );
    the_node = _Chain_Immutable_next( the_node )
  ) {
    const TOD_Hook *the_hook;

    the_hook = (const TOD_Hook *) the_node;
    status = ( *the_hook->handler )( action, tod );
    if ( status != STATUS_SUCCESSFUL ) {
      break;
    }
  }

  return status;
}
