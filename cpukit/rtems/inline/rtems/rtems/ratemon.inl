/**
 * @file rtems/rtems/ratemon.inl
 *
 *  This file contains the static inline  implementation of the inlined
 *  routines in the Rate Monotonic Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_RATEMON_H
# error "Never use <rtems/rtems/ratemon.inl> directly; include <rtems/rtems/ratemon.h> instead."
#endif

#ifndef _RTEMS_RTEMS_RATEMON_INL
#define _RTEMS_RTEMS_RATEMON_INL

/**
 *  @addtogroup ClassicRateMon
 *  @{
 */

/**
 *  @brief Rate_monotonic_Allocate
 *
 *  This function allocates a period control block from
 *  the inactive chain of free period control blocks.
 */
RTEMS_INLINE_ROUTINE Rate_monotonic_Control *_Rate_monotonic_Allocate( void )
{
  return (Rate_monotonic_Control *)
    _Objects_Allocate( &_Rate_monotonic_Information );
}

/**
 *  @brief Rate_monotonic_Free
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

/**
 *  @brief Rate_monotonic_Get
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

/**
 *  @brief Rate_monotonic_Is_active
 *
 *  This function returns TRUE if the_period is in the ACTIVE state,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Rate_monotonic_Is_active (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_ACTIVE);
}

/**
 *  @brief Rate_monotonic_Is_inactive
 *
 *  This function returns TRUE if the_period is in the ACTIVE state,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Rate_monotonic_Is_inactive (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_INACTIVE);
}

/**
 *  @brief Rate_monotonic_Is_expired
 *
 *  This function returns TRUE if the_period is in the EXPIRED state,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Rate_monotonic_Is_expired (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_EXPIRED);
}

/**
 *  @brief Rate_monotonic_Is_null
 *
 *  This function returns TRUE if the_period is NULL and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Rate_monotonic_Is_null (
  Rate_monotonic_Control *the_period
)
{
  return (the_period == NULL);
}

/**@}*/

#endif
/* end of include file */
