/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/smplock.h>

#if defined( RTEMS_PROFILING )
SMP_lock_Stats_control _SMP_lock_Stats_control = {
  .Lock = {
    .ticket_lock = {
      .next_ticket = ATOMIC_INITIALIZER_UINT( 0U ),
      .now_serving = ATOMIC_INITIALIZER_UINT( 0U ),
      .Stats = {
        .Node = CHAIN_NODE_INITIALIZER_ONE_NODE_CHAIN(
          &_SMP_lock_Stats_control.Stats_chain
        ),
        .name = "SMP lock stats"
      }
    }
  },
  .Stats_chain = CHAIN_INITIALIZER_ONE_NODE(
    &_SMP_lock_Stats_control.Lock.ticket_lock.Stats.Node
  ),
  .Iterator_chain = CHAIN_INITIALIZER_EMPTY(
    _SMP_lock_Stats_control.Iterator_chain
  )
};
#endif /* defined( RTEMS_PROFILING ) */
