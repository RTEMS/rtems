/*  rtems/posix/intr.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX Interrupt Manager
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
 
#ifndef __RTEMS_POSIX_INTERRUPT_inl
#define __RTEMS_POSIX_INTERRUPT_inl

/*PAGE
 *
 *  _POSIX_Interrupt_Allocate
 */
 
RTEMS_INLINE_ROUTINE POSIX_Interrupt_Handler_control *
  _POSIX_Interrupt_Allocate( void )
{
  return (POSIX_Interrupt_Handler_control *) 
    _Objects_Allocate( &_POSIX_Interrupt_Handlers_Information );
}
 
/*PAGE
 *
 *  _POSIX_Interrupt_Free
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Interrupt_Free (
  POSIX_Interrupt_Handler_control *the_intr
)
{
  _Objects_Free( &_POSIX_Interrupt_Handlers_Information, &the_intr->Object );
}
 
/*PAGE
 *
 *  _POSIX_Interrupt_Get
 */
 
RTEMS_INLINE_ROUTINE POSIX_Interrupt_Control *_POSIX_Interrupt_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (POSIX_Interrupt_Control *)
    _Objects_Get( &_POSIX_Interrupt_Handlers_Information, id, location );
}
 
/*PAGE
 *
 *  _POSIX_Interrupt_Is_null
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Interrupt_Is_null (
  POSIX_Interrupt_Handler_control *the_intr
)
{
  return !the_intr;
}

#endif
/*  end of include file */

