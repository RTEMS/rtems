/**
 * @file
 *
 * @ingroup RTEMSScoreAPIMutex
 *
 * @brief Structures for the implementation of mutexes.
 */

/*
 * Copyright (c) 2015, 2017 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SCORE_MUTEXIMPL_H
#define _RTEMS_SCORE_MUTEXIMPL_H

#include <rtems/score/threadqimpl.h>

/**
 * @addtogroup RTEMSScoreAPIMutex
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  Thread_queue_Syslock_queue Queue;
} Mutex_Control;

typedef struct {
  Mutex_Control Mutex;
  unsigned int nest_level;
} Mutex_recursive_Control;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* _RTEMS_SCORE_MUTEXIMPL_H */
