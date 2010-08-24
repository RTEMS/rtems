/**
 * @file
 *
 * @ingroup ClassicChains
 *
 * @brief rtems_chain_append_with_notification() implementation.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

rtems_status_code rtems_chain_append_with_notification(
  rtems_chain_control *chain,
  rtems_chain_node *node,
  rtems_id task,
  rtems_event_set events
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  bool was_empty = rtems_chain_append_with_empty_check( chain, node );

  if ( was_empty ) {
    sc = rtems_event_send( task, events );
  }

  return sc;
}
