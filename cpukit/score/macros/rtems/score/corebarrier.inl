/*  macros/corebarrier.inl
 *
 *  This include file contains all of the inlined routines associated
 *  with core barriers.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_COREBARRIER_INL
#define _RTEMS_SCORE_COREBARRIER_INL

/*
 *  _CORE_barrier_Is_automatic
 */

#define _CORE_barrier_Is_automatic( _the_attribute ) \
  ( (_the_attribute)->discipline == CORE_BARRIER_AUTOMATIC_RELEASE)

/*
 *  _CORE_barrier_Get_number_of_waiting_threads
 */
 
#define _CORE_barrier_Get_number_of_waiting_threads( _the_barrier ) \
  ( (_the_barrier)->number_of_waiting_threads )

#endif
/* end of include file */
