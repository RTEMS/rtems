/*  ratemon.inl
 *
 *  This file contains the static inline  implementation of the inlined
 *  routines in the Rate Monotonic Manager.
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

#ifndef __RATE_MONOTONIC_inl
#define __RATE_MONOTONIC_inl

/*PAGE
 *
 *  _Rate_monotonic_Allocate
 *
 */

STATIC INLINE Rate_monotonic_Control *_Rate_monotonic_Allocate( void )
{
  return (Rate_monotonic_Control *)
    _Objects_Allocate( &_Rate_monotonic_Information );
}

/*PAGE
 *
 *  _Rate_monotonic_Free
 *
 */

STATIC INLINE void _Rate_monotonic_Free (
  Rate_monotonic_Control *the_period
)
{
  _Objects_Free( &_Rate_monotonic_Information, &the_period->Object );
}

/*PAGE
 *
 *  _Rate_monotonic_Get
 *
 */

STATIC INLINE Rate_monotonic_Control *_Rate_monotonic_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Rate_monotonic_Control *)
    _Objects_Get( &_Rate_monotonic_Information, id, location );
}

/*PAGE
 *
 *  _Rate_monotonic_Is_active
 *
 */

STATIC INLINE boolean _Rate_monotonic_Is_active (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_ACTIVE);
}

/*PAGE
 *
 *  _Rate_monotonic_Is_inactive
 *
 */

STATIC INLINE boolean _Rate_monotonic_Is_inactive (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_INACTIVE);
}

/*PAGE
 *
 *  _Rate_monotonic_Is_expired
 *
 */

STATIC INLINE boolean _Rate_monotonic_Is_expired (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_EXPIRED);
}

/*PAGE
 *
 *  _Rate_monotonic_Is_null
 *
 */

STATIC INLINE boolean _Rate_monotonic_Is_null (
  Rate_monotonic_Control *the_period
)
{
  return (the_period == NULL);
}

#endif
/* end of include file */
