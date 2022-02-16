/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
 *
 * @brief This source file contains the implementation of
 *   _User_extensions_Add_set().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/smp.h>
#include <rtems/score/percpu.h>

static void _User_extensions_Set_ancestors( void )
{
#if defined(RTEMS_SMP)
  if ( _Chain_Is_empty( &_User_extensions_Switches_list ) ) {
    uint32_t cpu_max;
    uint32_t cpu_index;

    cpu_max = _SMP_Get_processor_maximum();

    for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
       Per_CPU_Control *cpu;

       cpu = _Per_CPU_Get_by_index( cpu_index );
       cpu->ancestor = cpu->executing;
    }
  }
#endif
}

void _User_extensions_Add_set(
  User_extensions_Control *the_extension
)
{
  ISR_lock_Context lock_context;

  _User_extensions_Acquire( &lock_context );
  _Chain_Initialize_node( &the_extension->Node );
  _Chain_Append_unprotected(
    &_User_extensions_List.Active,
    &the_extension->Node
  );
  _User_extensions_Release( &lock_context );

  /*
   * If a switch handler is present, append it to the switch chain.
   */

  if ( the_extension->Callouts.thread_switch != NULL ) {
    the_extension->Switch.thread_switch =
      the_extension->Callouts.thread_switch;

    _Per_CPU_Acquire_all( &lock_context );
    _User_extensions_Set_ancestors();
    _Chain_Initialize_node( &the_extension->Switch.Node );
    _Chain_Append_unprotected(
      &_User_extensions_Switches_list,
      &the_extension->Switch.Node
    );
    _Per_CPU_Release_all( &lock_context );
  }
}
