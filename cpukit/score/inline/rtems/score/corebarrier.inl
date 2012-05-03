/** 
 *  @file  rtems/score/corebarrier.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with the SuperCore barrier.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREBARRIER_H
# error "Never use <rtems/score/corebarrier.inl> directly; include <rtems/score/corebarrier.h> instead."
#endif

#ifndef _RTEMS_SCORE_COREBARRIER_INL
#define _RTEMS_SCORE_COREBARRIER_INL

/**
 *  @addtogroup ScoreBarrier 
 *  @{
 */

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/**
 *  This function returns true if the automatic release attribute is
 *  enabled in the @a attribute_set and false otherwise.
 *
 *  @param[in] the_attribute is the attribute set to test
 *  @return true if the priority attribute is enabled
 */
RTEMS_INLINE_ROUTINE bool _CORE_barrier_Is_automatic(
  CORE_barrier_Attributes *the_attribute
)
{
   return
     (the_attribute->discipline == CORE_BARRIER_AUTOMATIC_RELEASE);
}

/**
 *  This routine returns the number of threads currently waiting at the barrier.
 *
 *  @param[in] the_barrier is the barrier to obtain the number of blocked
 *             threads for
 *  @return the current count of this barrier
 */
RTEMS_INLINE_ROUTINE uint32_t  _CORE_barrier_Get_number_of_waiting_threads(
  CORE_barrier_Control  *the_barrier
)
{
  return the_barrier->number_of_waiting_threads;
}

/**@}*/

#endif
/* end of include file */
