/**
 * @file rtems/rtems/barrier.inl
 */

/*
 *  This file contains the macro implementation of the inlined
 *  routines from the Barrier Manager.
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

#ifndef _RTEMS_RTEMS_BARRIER_INL
#define _RTEMS_RTEMS_BARRIER_INL

/*
 *  _Barrier_Allocate
 */
#define _Barrier_Allocate() \
  (Barrier_Control *) _Objects_Allocate( &_Barrier_Information )

/*
 *  _Barrier_Free
 */
#define _Barrier_Free( _the_barrier ) \
  _Objects_Free( &_Barrier_Information, &(_the_barrier)->Object )

/*
 *  _Barrier_Get
 */

#define _Barrier_Get( _id, _location ) \
  (Barrier_Control *) \
  _Objects_Get( &_Barrier_Information, (_id), (_location) )

/*
 *  _Barrier_Is_null
 */
#define _Barrier_Is_null( _the_barrier ) \
  ( (_the_barrier) == NULL )

#endif
/*  end of include file */
