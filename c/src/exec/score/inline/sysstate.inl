/*  sysstates.inl
 *
 *  This file contains the inline implementation of routines regarding the 
 *  system state.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __SYSTEM_STATE_inl
#define __SYSTEM_STATE_inl

/*PAGE
 *
 *  _System_state_Set
 */

STATIC INLINE void _System_state_Set (
  System_state_Codes state
)
{
  _System_state_Current = state;
}

/*PAGE
 *
 *  _System_state_Get
 */

STATIC INLINE System_state_Codes _System_state_Get ( void )
{
  return _System_state_Current;
}

/*PAGE
 *
 *  _System_state_Is_before_initialization
 */

STATIC INLINE boolean _System_state_Is_before_initialization (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_INITIALIZATION);
}

/*PAGE
 *
 *  _System_state_Is_before_multitasking
 */

STATIC INLINE boolean _System_state_Is_before_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_MULTITASKING);
}

/*PAGE
 *
 *  _System_state_Is_begin_multitasking
 */

STATIC INLINE boolean _System_state_Is_begin_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEGIN_MULTITASKING);
}

/*PAGE
 *
 *  _System_state_Is_up
 */

STATIC INLINE boolean _System_state_Is_up (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_UP);
}

/*PAGE
 *
 *  _System_state_Is_failed
 */

STATIC INLINE boolean _System_state_Is_failed (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_FAILED);
}

#endif
/* end of include file */
