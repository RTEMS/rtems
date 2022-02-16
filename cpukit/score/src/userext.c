/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
 *
 * @brief This source file contains the implementation of
 *   _User_extensions_Handler_initialization().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/score/userextimpl.h>

void _User_extensions_Handler_initialization(void)
{
  const User_extensions_Table    *initial_table;
  User_extensions_Switch_control *initial_switch_controls;
  size_t                          n;
  size_t                          i;

  initial_table = _User_extensions_Initial_extensions;
  initial_switch_controls = _User_extensions_Initial_switch_controls;
  n = _User_extensions_Initial_count;

  for ( i = 0 ; i < n ; ++i ) {
    User_extensions_thread_switch_extension callout;

    callout = initial_table[ i ].thread_switch;

    if ( callout != NULL ) {
      User_extensions_Switch_control *c;

      c = &initial_switch_controls[ i ];
      c->thread_switch = callout;
      _Chain_Initialize_node( &c->Node );
      _Chain_Append_unprotected( &_User_extensions_Switches_list, &c->Node );
    }
  }
}
