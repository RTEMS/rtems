/*  rtems/posix/intr.inl
 *
 *  This include file contains the macro implementation of the private 
 *  inlined routines for POSIX Interrupt Manager
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
 
#ifndef __RTEMS_POSIX_INTERRUPT_inl
#define __RTEMS_POSIX_INTERRUPT_inl

/*PAGE
 *
 *  _POSIX_Interrupt_Allocate
 */
 
#define _POSIX_Interrupt_Allocate() \
  (POSIX_Interrupt_Handler_control *) \
    _Objects_Allocate( &_POSIX_Interrupt_Handlers_Information )
 
/*PAGE
 *
 *  _POSIX_Interrupt_Free
 */
 
#define _POSIX_Interrupt_Free( _the_intr ) \
  _Objects_Free( &_POSIX_Interrupt_Handlers_Information, &(_the_intr)->Object )
 
/*PAGE
 *
 *  _POSIX_Interrupt_Get
 */
 
#define _POSIX_Interrupt_Get( _id, _location ) \
  (POSIX_Interrupt_Control *) \
    _Objects_Get( &_POSIX_Interrupt_Handlers_Information, (_id), (_location) )
 
/*PAGE
 *
 *  _POSIX_Interrupt_Is_null
 */
 
#define _POSIX_Interrupt_Is_null( _the_intr ) \
  (!(_the_intr))

#endif
/*  end of include file */

