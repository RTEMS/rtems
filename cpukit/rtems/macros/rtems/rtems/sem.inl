/*  sem.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Semaphore Manager.
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

#ifndef __SEMAPHORE_inl
#define __SEMAPHORE_inl

/*PAGE
 *
 *  _Semaphore_Allocate
 *
 */

#define _Semaphore_Allocate() \
  (Semaphore_Control *) _Objects_Allocate( &_Semaphore_Information )

/*PAGE
 *
 *  _Semaphore_Free
 *
 */

#define _Semaphore_Free( _the_semaphore ) \
  _Objects_Free( &_Semaphore_Information, &(_the_semaphore)->Object )

/*PAGE
 *
 *  _Semaphore_Get
 *
 */

#define _Semaphore_Get( _id, _location ) \
  (Semaphore_Control *) \
  _Objects_Get( &_Semaphore_Information, (_id), (_location) )

#define _Semaphore_Get_interrupt_disable( _id, _location, _level ) \
  (Semaphore_Control *) \
  _Objects_Get_isr_disable( \
    &_Semaphore_Information, (_id), (_location), (_level) )

/*PAGE
 *
 *  _Semaphore_Is_null
 *
 */

#define _Semaphore_Is_null( _the_semaphore ) \
  ( (_the_semaphore) == NULL )

#endif
/*  end of include file */
