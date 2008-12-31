/** 
 *  @file  rtems/score/sysstate.inl
 *
 *  This file contains the inline implementation of routines regarding the 
 *  system state.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_SYSSTATE_H
# error "Never use <rtems/score/sysstate.inl> directly; include <rtems/score/sysstate.h> instead."
#endif

#ifndef _RTEMS_SCORE_SYSSTATE_INL
#define _RTEMS_SCORE_SYSSTATE_INL

/**
 *  @addtogroup ScoreSysState 
 *  @{
 */

/**
 *  This routine initializes the system state handler.
 */
 
RTEMS_INLINE_ROUTINE void _System_state_Handler_initialization (
#if defined(RTEMS_MULTIPROCESSING)
  bool  is_multiprocessing
#else
  bool  is_multiprocessing __attribute__((unused))
#endif
)
{
  _System_state_Current = SYSTEM_STATE_BEFORE_INITIALIZATION;
#if defined(RTEMS_MULTIPROCESSING)
    _System_state_Is_multiprocessing = is_multiprocessing;
#endif
}
 
/**
 *  This routine sets the current system state to that specified by
 *  the called.
 */

RTEMS_INLINE_ROUTINE void _System_state_Set (
  System_state_Codes state
)
{
  _System_state_Current = state;
}

/**
 *  This function returns the current system state.
 */

RTEMS_INLINE_ROUTINE System_state_Codes _System_state_Get ( void )
{
  return _System_state_Current;
}

/**
 *  This function returns true if the state is equal to the
 *  "before initialization" state, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _System_state_Is_before_initialization (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_INITIALIZATION);
}

/**
 *  This function returns true if the state is equal to the
 *  "before multitasking" state, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _System_state_Is_before_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_MULTITASKING);
}

/**
 *  This function returns true if the state is equal to the
 *  "begin multitasking" state, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _System_state_Is_begin_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEGIN_MULTITASKING);
}

/**
 *  This function returns true if the state is equal to the
 *  "up" state, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _System_state_Is_up (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_UP);
}

/**
 *  This function returns true if the state is equal to the
 *  "failed" state, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _System_state_Is_failed (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_FAILED);
}

/**@}*/

#endif
/* end of include file */
