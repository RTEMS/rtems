/*  ratemon.inl
 *
 *  This file contains the static inline  implementation of the inlined
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
 *  DESCRIPTION:
 *
 *  This function allocates a period control block from
 *  the inactive chain of free period control blocks.
 */

RTEMS_INLINE_ROUTINE Rate_monotonic_Control *_Rate_monotonic_Allocate( void )
{
  return (Rate_monotonic_Control *)
    _Objects_Allocate( &_Rate_monotonic_Information );
}

/*PAGE
 *
 *  _Rate_monotonic_Free
 *
 *  DESCRIPTION:
 *
 *  This routine allocates a period control block from
 *  the inactive chain of free period control blocks.
 */

RTEMS_INLINE_ROUTINE void _Rate_monotonic_Free (
  Rate_monotonic_Control *the_period
)
{
  _Objects_Free( &_Rate_monotonic_Information, &the_period->Object );
}

/*PAGE
 *
 *  _Rate_monotonic_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps period IDs to period control blocks.
 *  If ID corresponds to a local period, then it returns
 *  the_period control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_period is undefined.
 */

RTEMS_INLINE_ROUTINE Rate_monotonic_Control *_Rate_monotonic_Get (
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
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_period is in the ACTIVE state,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Rate_monotonic_Is_active (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_ACTIVE);
}

/*PAGE
 *
 *  _Rate_monotonic_Is_inactive
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_period is in the ACTIVE state,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Rate_monotonic_Is_inactive (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_INACTIVE);
}

/*PAGE
 *
 *  _Rate_monotonic_Is_expired
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_period is in the EXPIRED state,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Rate_monotonic_Is_expired (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_EXPIRED);
}

/*PAGE
 *
 *  _Rate_monotonic_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_period is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Rate_monotonic_Is_null (
  Rate_monotonic_Control *the_period
)
{
  return (the_period == NULL);
}

#endif
/* end of include file */
