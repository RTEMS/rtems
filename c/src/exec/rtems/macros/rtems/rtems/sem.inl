/*  sem.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Semaphore Manager.
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

/*PAGE
 *
 *  _Semaphore_Is_null
 *
 */

#define _Semaphore_Is_null( _the_semaphore ) \
  ( (_the_semaphore) == NULL )

#endif
/*  end of include file */
