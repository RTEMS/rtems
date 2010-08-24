/**
 * @file
 *
 * @ingroup ClassicChains
 *
 * @brief rtems_chain_get_with_wait() implementation.
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

rtems_status_code rtems_chain_get_with_wait(
  rtems_chain_control *chain,
  rtems_event_set events,
  rtems_interval timeout,
  rtems_chain_node **node_ptr
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_chain_node *node = NULL;

  while (
    sc == RTEMS_SUCCESSFUL
      && (node = rtems_chain_get( chain )) == NULL
  ) {
    rtems_event_set out;
    sc = rtems_event_receive(
      events,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      timeout,
      &out
    );
  }

  *node_ptr = node;

  return sc;
}
