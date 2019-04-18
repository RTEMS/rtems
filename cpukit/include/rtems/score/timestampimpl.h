/**
 * @file
 *
 * @ingroup SuperCoreTimeStamp
 *
 * @brief Helpers for Manipulating Timestamps
 *
 * This include file contains helpers for manipulating timestamps.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TIMESTAMPIMPL_H
#define _RTEMS_SCORE_TIMESTAMPIMPL_H

/**
 * @addtogroup SuperCoreTimeStamp
 *
 * @{
 */

#include <rtems/score/timestamp.h>
#include <rtems/score/basedefs.h>

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sets timestamp to specified seconds and nanoseconds.
 *
 * This method sets the timestamp to the specified @a _seconds and @a _nanoseconds
 * value.
 *
 * @param[out] _time The timestamp instance to set.
 * @param _seconds The seconds portion of the timestamp.
 * @param _nanoseconds The nanoseconds portion of the timestamp.
 */
RTEMS_INLINE_ROUTINE void _Timestamp_Set(
  Timestamp_Control *_time,
  time_t             _seconds,
  long               _nanoseconds
)
{
  struct timespec _ts;

  _ts.tv_sec = _seconds;
  _ts.tv_nsec = _nanoseconds;

  *_time = tstosbt(_ts);
}

/**
 * @brief Sets the timestamp to zero.
 *
 * This method sets the timestamp to zero.
 * value.
 *
 * @param[out] _time The timestamp instance to zero.
 */

RTEMS_INLINE_ROUTINE void _Timestamp_Set_to_zero(
  Timestamp_Control *_time
)
{
  *_time = 0;
}

/**
 * @brief Checks if the left hand side timestamp is less than the right one.
 *
 * This method is the less than operator for timestamps.
 *
 * @param _lhs The left hand side timestamp.
 * @param _rhs The right hand side timestamp.
 *
 * @retval true @a _lhs is less than the @a _rhs.
 * @retval false @a _lhs is greater or equal than @a rhs.
 */

RTEMS_INLINE_ROUTINE bool _Timestamp_Less_than(
  const Timestamp_Control *_lhs,
  const Timestamp_Control *_rhs
)
{
  return *_lhs < *_rhs;
}

/**
 * @brief Checks if the left hand side timestamp is greater than the right one.
 *
 * This method is the greater than operator for timestamps.
 *
 * @param _lhs The left hand side timestamp.
 * @param _rhs The right hand side timestamp.
 *
 * @retval true @a _lhs is greater than the @a _rhs.
 * @retval false @a _lhs is less or equal than @a _rhs.
 */

RTEMS_INLINE_ROUTINE bool _Timestamp_Greater_than(
  const Timestamp_Control *_lhs,
  const Timestamp_Control *_rhs
)
{
  return *_lhs > *_rhs;
}

/**
 * @brief Checks if the timestamps are equal.
 *
 * This method is the is equal to than operator for timestamps.
 *
 * @param _lhs The left hand side timestamp.
 * @param _rhs The right hand side timestamp.
 *
 * @retval true @a _lhs is equal to  @a _rhs
 * @retval false @a _lhs is not equal to @a _rhs.
 */

RTEMS_INLINE_ROUTINE bool _Timestamp_Equal_to(
  const Timestamp_Control *_lhs,
  const Timestamp_Control *_rhs
)
{
  return *_lhs == *_rhs;
}

/**
 * @brief Adds two timestamps.
 *
 * This routine adds two timestamps.  The second argument is added
 * to the first.
 *
 * @param[in, out] _time The base time to be added to.
 * @param _add points The timestamp to add to the first argument.
 */
RTEMS_INLINE_ROUTINE void _Timestamp_Add_to(
  Timestamp_Control *_time,
  const Timestamp_Control *_add
)
{
  *_time += *_add;
}

/**
 * @brief Subtracts two timestamps.
 *
 * This routine subtracts two timestamps.  @a result is set to
 * @a end - @a start.
 *
 * @param _start The starting time.
 * @param _end The ending time.
 * @param[out] _result Contains the difference between starting and ending
 *      time after the method call.
 */
RTEMS_INLINE_ROUTINE void _Timestamp_Subtract(
  const Timestamp_Control *_start,
  const Timestamp_Control *_end,
  Timestamp_Control       *_result
)
{
  *_result = *_end - *_start;
}

/**
 * @brief Divides a timestamp by another timestamp.
 *
 * This routine divides a timestamp by another timestamp.  The
 * intended use is for calculating percentages to three decimal points.
 *
 * @param _lhs The left hand number.
 * @param _rhs The right hand number.
 * @param[out] _ival_percentage The integer portion of the average.
 * @param[out] _fval_percentage The thousandths of percentage.
 */
RTEMS_INLINE_ROUTINE void _Timestamp_Divide(
  const Timestamp_Control *_lhs,
  const Timestamp_Control *_rhs,
  uint32_t                *_ival_percentage,
  uint32_t                *_fval_percentage
)
{
  struct timespec _ts_left;
  struct timespec _ts_right;

  _ts_left = sbttots( *_lhs );
  _ts_right = sbttots( *_rhs );

  _Timespec_Divide(
    &_ts_left,
    &_ts_right,
    _ival_percentage,
    _fval_percentage
  );
}

/**
 * @brief Gets seconds portion of timestamp.
 *
 * This method returns the seconds portion of the specified timestamp.
 *
 * @param _time The timestamp.
 *
 * @return The seconds portion of @a _time.
 */
RTEMS_INLINE_ROUTINE time_t _Timestamp_Get_seconds(
  const Timestamp_Control *_time
)
{
  return (*_time >> 32);
}

/**
 * @brief Gets nanoseconds portion of timestamp.
 *
 * This method returns the nanoseconds portion of the specified timestamp.
 *
 * @param _time The timestamp.
 *
 * @return The nanoseconds portion of @a _time.
 */
RTEMS_INLINE_ROUTINE uint32_t _Timestamp_Get_nanoseconds(
  const Timestamp_Control *_time
)
{
  struct timespec _ts;

  _ts = sbttots( *_time );

  return (uint32_t) _ts.tv_nsec;
}

/**
 * @brief Gets the timestamp as nanoseconds.
 *
 * This method returns the timestamp as nanoseconds.
 *
 * @param _time The timestamp.
 *
 * @return The time in nanoseconds.
 */
RTEMS_INLINE_ROUTINE uint64_t _Timestamp_Get_as_nanoseconds(
  const Timestamp_Control *_time
)
{
  struct timespec _ts;

  _ts = sbttots( *_time );

  return _Timespec_Get_as_nanoseconds( &_ts );
}

/**
 * @brief Converts timestamp to struct timespec.
 *
 * @param _timestamp The timestamp.
 * @param[out] _timespec The timespec to be filled in by the method.
 */
RTEMS_INLINE_ROUTINE void _Timestamp_To_timespec(
  const Timestamp_Control *_timestamp,
  struct timespec         *_timespec
)
{
  *_timespec = sbttots( *_timestamp );
}

/**
 * @brief Converts timestamp to struct timeval.
 *
 * @param _timestamp The timestamp.
 * @param[out] _timeval The timeval to be filled in by the method.
 */
RTEMS_INLINE_ROUTINE void _Timestamp_To_timeval(
  const Timestamp_Control *_timestamp,
  struct timeval          *_timeval
)
{
  *_timeval = sbttotv( *_timestamp );
}

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
