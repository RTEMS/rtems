/**
 * @file
 *
 * @brief SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerSMP
 */

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

#ifndef _RTEMS_SCORE_SCHEDULERSMPIMPL_H
#define _RTEMS_SCORE_SCHEDULERSMPIMPL_H

#include <rtems/score/schedulersmp.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/thread.h>
#include <rtems/score/percpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup ScoreSchedulerSMP
 *
 * @{
 */

static inline Scheduler_SMP_Control *_Scheduler_SMP_Instance( void )
{
  return _Scheduler.information;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMPIMPL_H */
