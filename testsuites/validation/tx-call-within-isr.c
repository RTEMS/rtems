/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of CallWithinISRClear(),
 *   CallWithinISRGetVector(), CallWithinISR(), CallWithinISRRaise(),
 *   CallWithinISRSubmit(), and CallWithinISRWait().
 */

/*
 * Copyright (C) 2021, 2022 embedded brains GmbH & Co. KG
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

#include "tx-support.h"

#include <rtems/sysinit.h>
#include <rtems/score/chainimpl.h>

#include <bsp.h>
#include <bsp/irq-generic.h>

/* Some target architectures need this variable for <tm27.h> */
uint32_t Interrupt_nest;

#define _RTEMS_TMTEST27

#include <tm27.h>

typedef struct {
  Chain_Control pending;
  RTEMS_INTERRUPT_LOCK_MEMBER( lock )
} CallWithinISRContext;

static CallWithinISRContext CallWithinISRInstance = {
#if defined( RTEMS_SMP )
  .lock = RTEMS_INTERRUPT_LOCK_INITIALIZER( "CallWithinISR" ),
#endif
  .pending = CHAIN_INITIALIZER_EMPTY( CallWithinISRInstance.pending )
};

void CallWithinISRRaise( void )
{
  Cause_tm27_intr();
}

void CallWithinISRClear( void )
{
  Clear_tm27_intr();
}

#ifdef TM27_USE_VECTOR_HANDLER
static rtems_isr CallWithinISRHandler( rtems_vector_number arg )
#else
static void CallWithinISRHandler( void *arg )
#endif
{
  CallWithinISRContext *ctx;

  (void) arg;
  ctx = &CallWithinISRInstance;

  CallWithinISRClear();

  while ( true ) {
    rtems_interrupt_lock_context lock_context;
    CallWithinISRRequest        *request;

    rtems_interrupt_lock_acquire( &ctx->lock, &lock_context );
    request = (CallWithinISRRequest *)
      _Chain_Get_unprotected( &ctx->pending );
    rtems_interrupt_lock_release( &ctx->lock, &lock_context );

    if ( request == NULL ) {
      break;
    }

    ( *request->handler )( request->arg );
    _Atomic_Store_uint( &request->done, 1, ATOMIC_ORDER_RELEASE );
  }
}

void CallWithinISR( void ( *handler )( void * ), void *arg )
{
  CallWithinISRRequest request;

  request.handler = handler;
  request.arg = arg;
  CallWithinISRSubmit( &request );
  CallWithinISRWait( &request );
}

void CallWithinISRSubmit( CallWithinISRRequest *request )
{
  CallWithinISRContext        *ctx;
  rtems_interrupt_lock_context lock_context;

  ctx = &CallWithinISRInstance;

  rtems_interrupt_lock_acquire( &ctx->lock, &lock_context );
  _Atomic_Store_uint( &request->done, 0, ATOMIC_ORDER_RELAXED );
  _Chain_Initialize_node( &request->node );
  _Chain_Append_unprotected( &ctx->pending, &request->node );
  rtems_interrupt_lock_release( &ctx->lock, &lock_context );

  CallWithinISRRaise();
}

void CallWithinISRWait( const CallWithinISRRequest *request )
{
  while ( _Atomic_Load_uint( &request->done, ATOMIC_ORDER_ACQUIRE ) == 0 ) {
    /* Wait */
  }
}

#if !defined( TM27_INTERRUPT_VECTOR_DEFAULT )
static void CallWithinISRIsHandlerInstalled(
  void                   *arg,
  const char             *info,
  rtems_option            option,
  rtems_interrupt_handler handler,
  void                   *handler_arg
)
{
  (void) info;
  (void) option;
  (void) handler_arg;

  if ( handler == CallWithinISRHandler && handler_arg == NULL ) {
    *(bool *) arg = true;
  }
}
#endif

rtems_vector_number CallWithinISRGetVector( void )
{
#if defined( TM27_INTERRUPT_VECTOR_DEFAULT )
  return TM27_INTERRUPT_VECTOR_DEFAULT;
#else
  rtems_vector_number vector;

  for ( vector = 0; vector < BSP_INTERRUPT_VECTOR_COUNT; ++vector ) {
    bool installed;

    installed = false;
    (void) rtems_interrupt_handler_iterate(
      vector,
      CallWithinISRIsHandlerInstalled,
      &installed
    );

    if ( installed ) {
      return vector;
    }
  }

  return UINT32_MAX;
#endif
}

rtems_vector_number GetSoftwareInterruptVector( void )
{
#if defined( TM27_INTERRUPT_VECTOR_ALTERNATIVE )
  return TM27_INTERRUPT_VECTOR_ALTERNATIVE;
#else
  return UINT32_MAX;
#endif
}

rtems_status_code RaiseSoftwareInterrupt( rtems_vector_number vector )
{
#if defined( TM27_INTERRUPT_VECTOR_ALTERNATIVE )
  if ( vector == TM27_INTERRUPT_VECTOR_ALTERNATIVE ) {
    return _TM27_Raise_alternative();
  }
#endif

  return rtems_interrupt_raise( vector );
}

rtems_status_code ClearSoftwareInterrupt( rtems_vector_number vector )
{
#if defined( TM27_INTERRUPT_VECTOR_ALTERNATIVE )
  if ( vector == TM27_INTERRUPT_VECTOR_ALTERNATIVE ) {
    return _TM27_Clear_alternative();
  }
#endif

  return rtems_interrupt_clear( vector );
}

static void CallWithinISRInitialize( void )
{
  Install_tm27_vector( CallWithinISRHandler );
}

RTEMS_SYSINIT_ITEM(
  CallWithinISRInitialize,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
