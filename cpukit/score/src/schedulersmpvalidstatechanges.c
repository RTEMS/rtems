/**
 * @file
 *
 * @ingroup ScoreSchedulerSMP
 *
 * @brief SMP Scheduler Implementation
 */

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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

/*
 * Table with all valid state transitions.  It is used in
 * _Scheduler_SMP_Node_change_state() in case RTEMS_DEBUG is defined.
 */
const bool _Scheduler_SMP_Node_valid_state_changes[ 3 ][ 3 ] = {
  /* FROM / TO       BLOCKED SCHEDULED READY */
  /* BLOCKED    */ { false,  true,     true },
  /* SCHEDULED  */ { true,   false,    true },
  /* READY      */ { true,   true,     false }
};
