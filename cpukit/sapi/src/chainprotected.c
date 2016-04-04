/*
 * Copyright (c) 2013, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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
