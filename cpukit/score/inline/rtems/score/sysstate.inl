/** 
 * @file
 *
 * @ingroup ScoreSysState
 *
 * @brief System State Handler API.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SYSSTATE_H
# error "Never use <rtems/score/sysstate.inl> directly; include <rtems/score/sysstate.h> instead."
#endif

#ifndef _RTEMS_SCORE_SYSSTATE_INL
#define _RTEMS_SCORE_SYSSTATE_INL

/**
 * @addtogroup ScoreSysState 
 *
 * @{
 */

RTEMS_INLINE_ROUTINE void _System_state_Set (
  System_state_Codes state
)
{
  _System_state_Current = state;
}
 
RTEMS_INLINE_ROUTINE void _System_state_Handler_initialization (
#if defined(RTEMS_MULTIPROCESSING)
  bool  is_multiprocessing
#else
  bool  is_multiprocessing __attribute__((unused))
#endif
)
{
  _System_state_Set( SYSTEM_STATE_BEFORE_INITIALIZATION );
#if defined(RTEMS_MULTIPROCESSING)
    _System_state_Is_multiprocessing = is_multiprocessing;
#endif
}

RTEMS_INLINE_ROUTINE System_state_Codes _System_state_Get ( void )
{
  return _System_state_Current;
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_before_initialization (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_INITIALIZATION);
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_before_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_MULTITASKING);
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_begin_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEGIN_MULTITASKING);
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_shutdown (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_SHUTDOWN);
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_up (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_UP);
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_failed (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_FAILED);
}

/**@}*/

#endif
/* end of include file */
