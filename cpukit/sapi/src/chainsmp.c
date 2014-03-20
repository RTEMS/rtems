/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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

#if defined( RTEMS_SMP )

#include <rtems/score/smplock.h>

static SMP_lock_Control chain_lock = SMP_LOCK_INITIALIZER("chains");

static void chain_acquire( SMP_lock_Context *lock_context )
{
  _SMP_lock_ISR_disable_and_acquire( &chain_lock, lock_context );
}

static void chain_release( SMP_lock_Context *lock_context )
{
  _SMP_lock_Release_and_ISR_enable( &chain_lock, lock_context );
}

void rtems_chain_extract( rtems_chain_node *node )
{
  SMP_lock_Context lock_context;

  chain_acquire( &lock_context );
  _Chain_Extract_unprotected( node );
  chain_release( &lock_context );
}

rtems_chain_node *rtems_chain_get( rtems_chain_control *chain )
{
  rtems_chain_node *node;
  SMP_lock_Context lock_context;

  chain_acquire( &lock_context );
  node = _Chain_Get_unprotected( chain );
  chain_release( &lock_context );

  return node;
}

void rtems_chain_insert( rtems_chain_node *after_node, rtems_chain_node *node )
{
  SMP_lock_Context lock_context;

  chain_acquire( &lock_context );
  _Chain_Insert_unprotected( after_node, node );
  chain_release( &lock_context );
}

void rtems_chain_append(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  SMP_lock_Context lock_context;

  chain_acquire( &lock_context );
  _Chain_Append_unprotected( chain, node );
  chain_release( &lock_context );
}

void rtems_chain_prepend(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  SMP_lock_Context lock_context;

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
  SMP_lock_Context lock_context;

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
  SMP_lock_Context lock_context;

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
  SMP_lock_Context lock_context;

  chain_acquire( &lock_context );
  is_empty_now = _Chain_Get_with_empty_check_unprotected( chain, node );
  chain_release( &lock_context );

  return is_empty_now;
}

#endif /* defined( RTEMS_SMP ) */
