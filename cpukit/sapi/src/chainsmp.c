/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

void rtems_chain_explicit_extract(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  ISR_Level level;

  _ISR_lock_ISR_disable_and_acquire( &chain->Lock, level );
  _Chain_Extract_unprotected( node );
  _ISR_lock_Release_and_ISR_enable( &chain->Lock, level );
}

rtems_chain_node *rtems_chain_get( rtems_chain_control *chain )
{
  rtems_chain_node *node;
  ISR_Level level;

  _ISR_lock_ISR_disable_and_acquire( &chain->Lock, level );
  node = _Chain_Get_unprotected( &chain->Chain );
  _ISR_lock_Release_and_ISR_enable( &chain->Lock, level );

  return node;
}

void rtems_chain_explicit_insert(
  rtems_chain_control *chain,
  rtems_chain_node *after_node,
  rtems_chain_node *node
)
{
  ISR_Level level;

  _ISR_lock_ISR_disable_and_acquire( &chain->Lock, level );
  _Chain_Insert_unprotected( after_node, node );
  _ISR_lock_Release_and_ISR_enable( &chain->Lock, level );
}

void rtems_chain_append(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  ISR_Level level;

  _ISR_lock_ISR_disable_and_acquire( &chain->Lock, level );
  _Chain_Append_unprotected( &chain->Chain, node );
  _ISR_lock_Release_and_ISR_enable( &chain->Lock, level );
}

void rtems_chain_prepend(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  ISR_Level level;

  _ISR_lock_ISR_disable_and_acquire( &chain->Lock, level );
  _Chain_Prepend_unprotected( &chain->Chain, node );
  _ISR_lock_Release_and_ISR_enable( &chain->Lock, level );
}

bool rtems_chain_append_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  bool was_empty;
  ISR_Level level;

  _ISR_lock_ISR_disable_and_acquire( &chain->Lock, level );
  was_empty = _Chain_Append_with_empty_check_unprotected(
    &chain->Chain,
    node
  );
  _ISR_lock_Release_and_ISR_enable( &chain->Lock, level );

  return was_empty;
}

bool rtems_chain_prepend_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  bool was_empty;
  ISR_Level level;

  _ISR_lock_ISR_disable_and_acquire( &chain->Lock, level );
  was_empty = _Chain_Prepend_with_empty_check_unprotected(
    &chain->Chain,
    node
  );
  _ISR_lock_Release_and_ISR_enable( &chain->Lock, level );

  return was_empty;
}

bool rtems_chain_get_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node **node
)
{
  bool is_empty_now;
  ISR_Level level;

  _ISR_lock_ISR_disable_and_acquire( &chain->Lock, level );
  is_empty_now = _Chain_Get_with_empty_check_unprotected(
    &chain->Chain,
    node
  );
  _ISR_lock_Release_and_ISR_enable( &chain->Lock, level );

  return is_empty_now;
}

#endif /* defined( RTEMS_SMP ) */
