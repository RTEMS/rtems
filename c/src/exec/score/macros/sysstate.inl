/*  sysstates.inl
 *
 *  This file contains the macro implementation of routines regarding the 
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
 *  sysstate.inl,v 1.2 1995/05/31 16:49:39 joel Exp
 */

#ifndef __SYSTEM_STATE_inl
#define __SYSTEM_STATE_inl

/*PAGE
 *
 *  _System_state_Set
 */

#define _System_state_Set( _state ) \
  _System_state_Current = (_state)

/*PAGE
 *
 *  _System_state_Get
 */

#define _System_state_Get() \
  (_System_state_Current)

/*PAGE
 *
 *  _System_state_Is_before_initialization
 */

#define _System_state_Is_before_initialization( _state ) \
  ((_state) == SYSTEM_STATE_BEFORE_INITIALIZATION)

/*PAGE
 *
 *  _System_state_Is_before_multitasking
 */

#define _System_state_Is_before_multitasking( _state ) \
  ((_state) == SYSTEM_STATE_BEFORE_MULTITASKING)

/*PAGE
 *
 *  _System_state_Is_begin_multitasking
 */

#define _System_state_Is_begin_multitasking( _state ) \
  ((_state) == SYSTEM_STATE_BEGIN_MULTITASKING)

/*PAGE
 *
 *  _System_state_Is_up
 */

#define _System_state_Is_up( _state ) \
  ((_state) == SYSTEM_STATE_UP)

/*PAGE
 *
 *  _System_state_Is_failed
 */

#define _System_state_Is_failed( _state ) \
  ((_state) == SYSTEM_STATE_FAILED)

#endif
/* end of include file */
