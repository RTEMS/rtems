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
 * @brief The enumerators of this type determine if the ticks member is
 *   validated in _TOD_Validate().
 */
typedef enum {
  /**
   * @brief Use this option to disable the validation of the ticks member in
   *   _TOD_Validate().
   */
  TOD_DISABLE_TICKS_VALIDATION = 0,

  /**
   * @brief Use this option to enable the validation of the ticks member in
   *   _TOD_Validate().
   */
  TOD_ENABLE_TICKS_VALIDATION = -1
} TOD_Ticks_validation;

/**
 * @brief Validates the time of day.
 *
 * @param the_tod is the reference to the time of day structure to validate or
 *   NULL.
 *
 * @param ticks_validation indicates if the ticks member of the time of day
 *   should be validated.  Use #TOD_ENABLE_TICKS_VALIDATION to validate the
 *   ticks member.  Use #TOD_DISABLE_TICKS_VALIDATION to skip the validation of
 *   the ticks member.
 *
 * @retval RTEMS_SUCCESSFUL @a the_tod references a valid time of day.
 * @retval RTEMS_INVALID_CLOCK @a the_tod references an invalid time of day.
 * @retval RTEMS_INVALID_ADDRESS @a the_tod reference is @c NULL.
 */
rtems_status_code _TOD_Validate(
  const rtems_time_of_day *the_tod,
  TOD_Ticks_validation     ticks_validation
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
