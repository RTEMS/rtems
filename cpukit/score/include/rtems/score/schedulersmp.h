/**
 * @file
 *
 * @brief SMP Scheduler API
 *
 * @ingroup ScoreSchedulerSMP
 */

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

#ifndef _RTEMS_SCORE_SCHEDULERSMP_H
#define _RTEMS_SCORE_SCHEDULERSMP_H

#include <rtems/score/chain.h>
#include <rtems/score/percpu.h>
#include <rtems/score/prioritybitmap.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSchedulerSMP SMP Scheduler
 *
 * @ingroup ScoreScheduler
 *
 * @{
 */

typedef struct {
  Chain_Control Scheduled;
} Scheduler_SMP_Control;

typedef struct {
  Scheduler_SMP_Control Base;
  Chain_Control         Ready;
} Scheduler_simple_SMP_Control;

typedef struct {
  Scheduler_SMP_Control    Base;
  Priority_bit_map_Control Bit_map;
  Chain_Control            Ready[ 1 ];
} Scheduler_priority_SMP_Control;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMP_H */
