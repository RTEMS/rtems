/*  thread.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Thread handler.
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

#ifndef __THREAD_inl
#define __THREAD_inl

/*PAGE
 *
 *  _Thread_Stop_multitasking
 *
 */

STATIC INLINE void _Thread_Stop_multitasking( void )
{
  _Context_Switch( &_Thread_Executing->Registers, &_Thread_BSP_context );
}

/*PAGE
 *
 *  _Thread_Is_executing
 *
 */

STATIC INLINE boolean _Thread_Is_executing (
  Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Executing );
}

/*PAGE
 *
 *  _Thread_Is_heir
 *
 */

STATIC INLINE boolean _Thread_Is_heir (
  Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Heir );
}

/*PAGE
 *
 *  _Thread_Is_executing_also_the_heir
 *
 */

STATIC INLINE boolean _Thread_Is_executing_also_the_heir( void )
{
  return ( _Thread_Executing == _Thread_Heir );
}

/*PAGE
 *
 *  _Thread_Resume
 *
 */

STATIC INLINE void _Thread_Resume (
  Thread_Control *the_thread
)
{
  _Thread_Clear_state( the_thread, STATES_SUSPENDED );
}

/*PAGE
 *
 *  _Thread_Unblock
 *
 */

STATIC INLINE void _Thread_Unblock (
  Thread_Control *the_thread
)
{
  _Thread_Clear_state( the_thread, STATES_BLOCKED );
}

/*PAGE
 *
 *  _Thread_Restart_self
 *
 */

STATIC INLINE void _Thread_Restart_self( void )
{
  if ( _Thread_Executing->fp_context != NULL )
    _Context_Restore_fp( &_Thread_Executing->fp_context );

  _CPU_Context_Restart_self( &_Thread_Executing->Registers );
}

/*PAGE
 *
 *  _Thread_Calculate_heir
 *
 */

STATIC INLINE void _Thread_Calculate_heir( void )
{
  _Thread_Heir = (Thread_Control *)
    _Thread_Ready_chain[ _Priority_Get_highest() ].first;
}

/*PAGE
 *
 *  _Thread_Is_allocated_fp
 *
 */

STATIC INLINE boolean _Thread_Is_allocated_fp (
  Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Allocated_fp );
}

/*PAGE
 *
 *  _Thread_Deallocate_fp
 *
 */

STATIC INLINE void _Thread_Deallocate_fp( void )
{
  _Thread_Allocated_fp = NULL;
}

/*PAGE
 *
 *  _Thread_Disable_dispatch
 *
 */

STATIC INLINE void _Thread_Disable_dispatch( void )
{
  _Thread_Dispatch_disable_level += 1;
}

/*PAGE
 *
 *  _Thread_Enable_dispatch
 *
 */

#if ( CPU_INLINE_ENABLE_DISPATCH == TRUE )
STATIC INLINE void _Thread_Enable_dispatch()
{
  if ( (--_Thread_Dispatch_disable_level) == 0 )
    _Thread_Dispatch();
}
#endif

#if ( CPU_INLINE_ENABLE_DISPATCH == FALSE )
void _Thread_Enable_dispatch( void );
#endif

/*PAGE
 *
 *  _Thread_Unnest_dispatch
 *
 */

STATIC INLINE void _Thread_Unnest_dispatch( void )
{
  _Thread_Dispatch_disable_level -= 1;
}

/*PAGE
 *
 *  _Thread_Is_dispatching_enabled
 *
 */

STATIC INLINE boolean _Thread_Is_dispatching_enabled( void )
{
  return ( _Thread_Dispatch_disable_level == 0 );
}

/*PAGE
 *
 *  _Thread_Is_context_switch_necessary
 *
 */

STATIC INLINE boolean _Thread_Is_context_switch_necessary( void )
{
  return ( _Context_Switch_necessary );
}

/*PAGE
 *
 *  _Thread_Dispatch_initialization
 *
 */

STATIC INLINE void _Thread_Dispatch_initialization( void )
{
  _Thread_Dispatch_disable_level = 1;
}

/*PAGE
 *
 *  _Thread_Is_null
 *
 */

STATIC INLINE boolean _Thread_Is_null (
  Thread_Control *the_thread
)
{
  return ( the_thread == NULL );
}

/*PAGE
 *
 *  _Thread_Get
 *
 */

STATIC INLINE Thread_Control *_Thread_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ) {
     _Thread_Disable_dispatch();
     *location = OBJECTS_LOCAL;
     return( _Thread_Executing );
  }

  return (Thread_Control *)
          _Objects_Get( &_Thread_Information, id, location );
}

#endif
/* end of include file */
