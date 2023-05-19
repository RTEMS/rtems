/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassicChains
 *
 * @brief This source file contains the implementation of rtems_chain_append(),
 *   rtems_chain_append_with_empty_check(), rtems_chain_extract(),
 *   rtems_chain_get(), rtems_chain_get_with_empty_check(),
 *   rtems_chain_insert(), rtems_chain_prepend(), and
 *   rtems_chain_prepend_with_empty_check().
 */

/*
 * Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
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

#include <rtems/chain.h>
#include <rtems/rtems/intr.h>

RTEMS_INTERRUPT_LOCK_DEFINE( static, chain_lock, "Chains" )

static void chain_acquire( rtems_interrupt_lock_context *lock_context )
{
  rtems_interrupt_lock_acquire( &chain_lock, lock_context );
}

static void chain_release( rtems_interrupt_lock_context *lock_context )
{
  rtems_interrupt_lock_release( &chain_lock, lock_context );
}

void rtems_chain_extract( rtems_chain_node *node )
{
  rtems_interrupt_lock_context lock_context;

  chain_acquire( &lock_context );
  _Chain_Extract_unprotected( node );
  chain_release( &lock_context );
}

rtems_chain_node *rtems_chain_get( rtems_chain_control *chain )
{
  rtems_chain_node *node;
  rtems_interrupt_lock_context lock_context;

  chain_acquire( &lock_context );
  node = _Chain_Get_unprotected( chain );
  chain_release( &lock_context );

  return node;
}

void rtems_chain_insert( rtems_chain_node *after_node, rtems_chain_node *node )
{
  rtems_interrupt_lock_context lock_context;

  chain_acquire( &lock_context );
  _Chain_Insert_unprotected( after_node, node );
  chain_release( &lock_context );
}

void rtems_chain_append(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  rtems_interrupt_lock_context lock_context;

  chain_acquire( &lock_context );
  _Chain_Append_unprotected( chain, node );
  chain_release( &lock_context );
}

void rtems_chain_prepend(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  rtems_interrupt_lock_context lock_context;

  chain_acquire( &lock_context );
  _Chain_Prepend_unprotected( chain, node );
  chain_release( &lock_context );
}

bool rtems_chain_append_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  bool was_empty;
  rtems_interrupt_lock_context lock_context;

  chain_acquire( &lock_context );
  was_empty = _Chain_Append_with_empty_check_unprotected( chain, node );
  chain_release( &lock_context );

  return was_empty;
}

bool rtems_chain_prepend_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  bool was_empty;
  rtems_interrupt_lock_context lock_context;

  chain_acquire( &lock_context );
  was_empty = _Chain_Prepend_with_empty_check_unprotected( chain, node );
  chain_release( &lock_context );

  return was_empty;
}

bool rtems_chain_get_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node **node
)
{
  bool is_empty_now;
  rtems_interrupt_lock_context lock_context;

  chain_acquire( &lock_context );
  is_empty_now = _Chain_Get_with_empty_check_unprotected( chain, node );
  chain_release( &lock_context );

  return is_empty_now;
}
