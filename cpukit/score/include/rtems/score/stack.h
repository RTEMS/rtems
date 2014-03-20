/**
 *  @file  rtems/score/stack.h
 *
 *  @brief Information About the Thread Stack Handler
 *
 *  This include file contains all information about the thread
 *  Stack Handler.  This Handler provides mechanisms which can be used to
 *  initialize and utilize stacks.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_STACK_H
#define _RTEMS_SCORE_STACK_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreStack Stack Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which is used in the management
 *  of thread stacks.
 */
/**@{*/

/**
 *  The following constant defines the minimum stack size which every
 *  thread must exceed.
 */
#define STACK_MINIMUM_SIZE  CPU_STACK_MINIMUM_SIZE

/**
 *  The following defines the control block used to manage each stack.
 */
typedef struct {
  /** This is the stack size. */
  size_t      size;
  /** This is the low memory address of stack. */
  void       *area;
}   Stack_Control;

/**
 *  This variable contains the the minimum stack size;
 *
 *  @note It is instantiated and set by User Configuration via confdefs.h.
 */
extern uint32_t rtems_minimum_stack_size;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
