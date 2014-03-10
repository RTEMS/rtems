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
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/chain.h>

#if defined( RTEMS_SMP )

#include <rtems/score/smplock.h>

static SMP_lock_Control chain_lock = SMP_LOCK_INITIALIZER;

static void chain_acquire( ISR_Level *level )
{
  _SMP_lock_ISR_disable_and_acquire( &chain_lock, *level );
}

static void chain_release( ISR_Level *level )
{
  _SMP_lock_Release_and_ISR_enable( &chain_lock, *level );
}

void rtems_chain_extract( rtems_chain_node *node )
{
  ISR_Level level;

  chain_acquire( &level );
  _Chain_Extract_unprotected( node );
  chain_release( &level );
}

rtems_chain_node *rtems_chain_get( rtems_chain_control *chain )
{
  rtems_chain_node *node;
  ISR_Level level;

  chain_acquire( &level );
  node = _Chain_Get_unprotected( chain );
  chain_release( &level );

  return node;
}

void rtems_chain_insert( rtems_chain_node *after_node, rtems_chain_node *node )
{
  ISR_Level level;

  chain_acquire( &level );
  _Chain_Insert_unprotected( after_node, node );
  chain_release( &level );
}

void rtems_chain_append(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  ISR_Level level;

  chain_acquire( &level );
  _Chain_Append_unprotected( chain, node );
  chain_release( &level );
}

void rtems_chain_prepend(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  ISR_Level level;

  chain_acquire( &level );
  _Chain_Prepend_unprotected( chain, node );
  chain_release( &level );
}

bool rtems_chain_append_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  bool was_empty;
  ISR_Level level;

  chain_acquire( &level );
  was_empty = _Chain_Append_with_empty_check_unprotected( chain, node );
  chain_release( &level );

  return was_empty;
}

bool rtems_chain_prepend_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  bool was_empty;
  ISR_Level level;

  chain_acquire( &level );
  was_empty = _Chain_Prepend_with_empty_check_unprotected( chain, node );
  chain_release( &level );

  return was_empty;
}

bool rtems_chain_get_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node **node
)
{
  bool is_empty_now;
  ISR_Level level;

  chain_acquire( &level );
  is_empty_now = _Chain_Get_with_empty_check_unprotected( chain, node );
  chain_release( &level );

  return is_empty_now;
}

#endif /* defined( RTEMS_SMP ) */
