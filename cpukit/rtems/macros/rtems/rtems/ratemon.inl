/*  ratemon.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines in the Rate Monotonic Manager.
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

#ifndef __RATE_MONOTONIC_inl
#define __RATE_MONOTONIC_inl

/*PAGE
 *
 *  _Rate_monotonic_Allocate
 *
 */

#define _Rate_monotonic_Allocate() \
  (Rate_monotonic_Control *) \
    _Objects_Allocate( &_Rate_monotonic_Information )

/*PAGE
 *
 *  _Rate_monotonic_Free
 *
 */

#define _Rate_monotonic_Free( _the_period ) \
  _Objects_Free( &_Rate_monotonic_Information, &(_the_period)->Object )

/*PAGE
 *
 *  _Rate_monotonic_Get
 *
 */

#define _Rate_monotonic_Get( _id, _location ) \
  (Rate_monotonic_Control *) \
    _Objects_Get( &_Rate_monotonic_Information, (_id), (_location) )

/*PAGE
 *
 *  _Rate_monotonic_Is_active
 *
 */

#define _Rate_monotonic_Is_active( _the_period ) \
    ((_the_period)->state == RATE_MONOTONIC_ACTIVE)

/*PAGE
 *
 *  _Rate_monotonic_Is_inactive
 *
 */

#define _Rate_monotonic_Is_inactive( _the_period ) \
    ((_the_period)->state == RATE_MONOTONIC_INACTIVE)

/*PAGE
 *
 *  _Rate_monotonic_Is_expired
 *
 */

#define _Rate_monotonic_Is_expired( _the_period ) \
    ((_the_period)->state == RATE_MONOTONIC_EXPIRED)

/*PAGE
 *
 *  _Rate_monotonic_Is_null
 *
 */

#define _Rate_monotonic_Is_null( _the_period )   ( (_the_period) == NULL  )

#endif
/* end of include file */
