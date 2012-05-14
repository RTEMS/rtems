/** 
 *  @file  rtems/score/corerwlock.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with the SuperCore RWLock.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORERWLOCK_H
# error "Never use <rtems/score/corerwlock.inl> directly; include <rtems/score/corerwlock.h> instead."
#endif

#ifndef _RTEMS_SCORE_CORERWLOCK_INL
#define _RTEMS_SCORE_CORERWLOCK_INL

/**
 *  @addtogroup ScoreRWLock 
 *  @{
 */

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/**
 *
 *  This method is used to initialize core rwlock attributes.
 *
 *  @param[in] the_attributes pointer to the attributes to initialize.
 */
RTEMS_INLINE_ROUTINE void _CORE_RWLock_Initialize_attributes(
  CORE_RWLock_Attributes *the_attributes
)
{
  the_attributes->XXX = 0;
}


/**@}*/

#endif
/* end of include file */
