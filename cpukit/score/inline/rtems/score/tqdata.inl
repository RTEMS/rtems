/** 
 *  @file  rtems/score/tqdata.inl
 *
 *  This file contains the static inline implementation of the inlined
 *  routines needed to support the Thread Queue Data.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TQDATA_H
# error "Never use <rtems/score/tqdata.inl> directly; include <rtems/score/tqdata.h> instead."
#endif

#ifndef _RTEMS_SCORE_TQDATA_INL
#define _RTEMS_SCORE_TQDATA_INL

/**
 *  @addtogroup ScoreThreadQ 
 *  @{
 */

/**
 *  This function returns the index of the priority chain on which
 *  a thread of the_priority should be placed.
 */

RTEMS_INLINE_ROUTINE uint32_t   _Thread_queue_Header_number (
  Priority_Control the_priority
)
{
  return (the_priority / TASK_QUEUE_DATA_PRIORITIES_PER_HEADER);
}

/**
 *  This function returns true if the_priority indicates that the
 *  enqueue search should start at the front of this priority
 *  group chain, and false if the search should start at the rear.
 */

RTEMS_INLINE_ROUTINE bool _Thread_queue_Is_reverse_search (
  Priority_Control the_priority
)
{
  return ( the_priority & TASK_QUEUE_DATA_REVERSE_SEARCH_MASK );
}

/**
 *  This routine is invoked to indicate that the specified thread queue is
 *  entering a critical section.
 */
 
RTEMS_INLINE_ROUTINE void _Thread_queue_Enter_critical_section (
  Thread_queue_Control *the_thread_queue
)
{
  the_thread_queue->sync_state = THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;
}

/**
 *  @}
 */

#endif
/* end of include file */
