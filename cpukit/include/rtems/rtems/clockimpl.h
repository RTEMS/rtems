/**
 * @file
 *
 * @ingroup RTEMSImplClassicClock
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicClock.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_CLOCKIMPL_H
#define _RTEMS_RTEMS_CLOCKIMPL_H

#include <rtems/rtems/clock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicClock Clock Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Clock Manager implementation.
 *
 * @{
 */

/**
 * @brief TOD Validate
 *
 * This support function returns true if @a the_tod contains
 * a valid time of day, and false otherwise.
 *
 * @param[in] the_tod is the TOD structure to validate
 *
 * @retval This method returns true if the TOD is valid and false otherwise.
 *
 * @note This routine only works for leap-years through 2099.
 */
bool _TOD_Validate(
  const rtems_time_of_day *the_tod
);

/**
 * @brief TOD to Seconds
 *
 * This function returns the number seconds between the epoch and @a the_tod.
 *
 * @param[in] the_tod is the TOD structure to convert to seconds
 *
 * @retval This method returns the number of seconds since epoch represented
 *         by @a the_tod
 */
Watchdog_Interval _TOD_To_seconds(
  const rtems_time_of_day *the_tod
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_CLOCKIMPL_H */
