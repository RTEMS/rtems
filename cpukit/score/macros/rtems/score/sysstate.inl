/*  sysstates.inl
 *
 *  This file contains the macro implementation of routines regarding the 
 *  system state.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __SYSTEM_STATE_inl
#define __SYSTEM_STATE_inl

/*PAGE
 *
 *  _System_state_Handler_initialization
 */
 
#define _System_state_Handler_initialization( _is_multiprocessing ) \
  do { \
    _System_state_Current = SYSTEM_STATE_BEFORE_INITIALIZATION; \
    _System_state_Is_multiprocessing = (_is_multiprocessing); \
  } while ( 0 )

/*PAGE
 *
 *  _System_state_Set
 */

#define _System_state_Set( _state ) \
  do { \
    _System_state_Current = (_state); \
  } while ( 0 )

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
