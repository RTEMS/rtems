/**
 *  @file  rtems/score/timestamp.h
 *
 *  @brief Helpers for Manipulating Timestamps
 *
 *  This include file contains helpers for manipulating timestamps.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TIMESTAMP_H
#define _RTEMS_SCORE_TIMESTAMP_H

/**
 *  @defgroup SuperCoreTimeStamp Score Timestamp
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to manipulating
 *  SuperCore Timestamps.  SuperCore Timestamps may be used to
 *  represent time of day, uptime, or intervals.
 *
 *  The key attribute of the SuperCore Timestamp handler is that it
 *  is a completely opaque handler.  There can be multiple implementations
 *  of the required functionality and with a recompile, RTEMS can use
 *  any implementation.  It is intended to be a simple wrapper.
 *
 *  This handler can be implemented as either struct timespec or
 *  unsigned64 bit numbers.  The use of a wrapper class allows the
 *  the implementation of timestamps to change on a per architecture
 *  basis.  This is an important option as the performance of this
 *  handler is critical.
 */
/**@{*/

#include <sys/time.h>

#include <rtems/score/basedefs.h>
#include <rtems/score/timespec.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *   Define the Timestamp control type.
 */
typedef struct bintime Timestamp_Control;

/**
 *  @brief Set timestamp to specified seconds and nanoseconds.
 *
 *  This method sets the timestamp to the specified @a _seconds and @a _nanoseconds
 *  value.
 *
 *  @param[in] _time points to the timestamp instance to validate.
 *  @param[in] _seconds is the seconds portion of the timestamp
 *  @param[in] _nanoseconds is the nanoseconds portion of the timestamp
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

  timespec2bintime( &_ts, _time );
}

/**
 *  @brief Sets the timestamp to zero.
 *
 *  This method sets the timestamp to zero.
 *  value.
 *
 *  @param[in] _time points to the timestamp instance to zero.
 */

RTEMS_INLINE_ROUTINE void _Timestamp_Set_to_zero(
  Timestamp_Control *_time
)
{
  _time->sec = 0;
  _time->frac = 0;
}

/**
 *  @brief Less than operator for timestamps.
 *
 *  This method is the less than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @retval This method returns true if @a _lhs is less than the @a _rhs and
 *          false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Timestamp_Less_than(
  const Timestamp_Control *_lhs,
  const Timestamp_Control *_rhs
)
{
  if ( _lhs->sec < _rhs->sec )
    return true;

  if ( _lhs->sec > _rhs->sec )
    return false;

  return _lhs->frac < _rhs->frac;
}

/**
 *  @brief Greater than operator for timestamps.
 *
 *  This method is the greater than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @retval This method returns true if @a _lhs is greater than the @a _rhs and
 *          false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Timestamp_Greater_than(
  const Timestamp_Control *_lhs,
  const Timestamp_Control *_rhs
)
{
  if ( _lhs->sec > _rhs->sec )
    return true;

  if ( _lhs->sec < _rhs->sec )
    return false;

  return _lhs->frac > _rhs->frac;
}

/**
 *  @brief Equal to than operator for timestamps.
 *
 *  This method is the is equal to than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @retval This method returns true if @a _lhs is equal to  @a _rhs and
 *          false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Timestamp_Equal_to(
  const Timestamp_Control *_lhs,
  const Timestamp_Control *_rhs
)
{
  return _lhs->sec == _rhs->sec && _lhs->frac == _rhs->frac;
}

/**
 *  @brief Adds two timestamps.
 *
 *  This routine adds two timestamps.  The second argument is added
 *  to the first.
 *
 *  @param[in] _time points to the base time to be added to
 *  @param[in] _add points to the timestamp to add to the first argument
 *
 *  @retval This method returns the number of seconds @a time increased by.
 */
RTEMS_INLINE_ROUTINE time_t _Timestamp_Add_to(
  Timestamp_Control *_time,
  const Timestamp_Control *_add
)
{
  time_t seconds = _time->sec;

  bintime_add( _time, _add );

  return _time->sec - seconds;
}

/**
 *  @brief Subtracts two timestamps.
 *
 *  This routine subtracts two timestamps.  @a result is set to
 *  @a end - @a start.
 *
 *  @param[in] _start points to the starting time
 *  @param[in] _end points to the ending time
 *  @param[in] _result points to the difference between
 *             starting and ending time.
 *
 *  @retval This method fills in @a _result.
 */
RTEMS_INLINE_ROUTINE void _Timestamp_Subtract(
  const Timestamp_Control *_start,
  const Timestamp_Control *_end,
  Timestamp_Control       *_result
)
{
  _result->sec = _end->sec;
  _result->frac = _end->frac;

  bintime_sub( _result, _start );
}

/**
 *  @brief Divides a timestamp by another timestamp.
 *
 *  This routine divides a timestamp by another timestamp.  The
 *  intended use is for calculating percentages to three decimal points.
 *
 *  @param[in] _lhs points to the left hand number
 *  @param[in] _rhs points to the right hand number
 *  @param[in] _ival_percentage points to the integer portion of the average
 *  @param[in] _fval_percentage points to the thousandths of percentage
 *
 *  @retval This method fills in @a result.
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

  bintime2timespec( _lhs, &_ts_left );
  bintime2timespec( _rhs, &_ts_right );

  _Timespec_Divide(
    &_ts_left,
    &_ts_right,
    _ival_percentage,
    _fval_percentage
  );
}

/**
 *  @brief Get seconds portion of timestamp.
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @retval The seconds portion of @a _time.
 */
RTEMS_INLINE_ROUTINE time_t _Timestamp_Get_seconds(
  const Timestamp_Control *_time
)
{
  return _time->sec;
}

/**
 *  @brief Get nanoseconds portion of timestamp.
 *
 *  This method returns the nanoseconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @retval The nanoseconds portion of @a _time.
 */
RTEMS_INLINE_ROUTINE uint32_t _Timestamp_Get_nanoseconds(
  const Timestamp_Control *_time
)
{
  struct timespec _ts;

  bintime2timespec( _time, &_ts );

  return _ts.tv_nsec;
}

/**
 *  @brief Get the timestamp as nanoseconds.
 *
 *  This method returns the timestamp as nanoseconds.
 *
 *  @param[in] _time points to the timestamp
 *
 *  @retval The time in nanoseconds.
 */
RTEMS_INLINE_ROUTINE uint64_t _Timestamp_Get_as_nanoseconds(
  const Timestamp_Control *_time
)
{
  struct timespec _ts;

  bintime2timespec( _time, &_ts );

  return _Timespec_Get_as_nanoseconds( &_ts );
}

/**
 *  @brief Convert timestamp to struct timespec.
 *
 *  This method returns the seconds portion of the specified @a _timestamp.
 *
 *  @param[in] _timestamp points to the timestamp
 *  @param[in] _timespec points to the timespec
 */
RTEMS_INLINE_ROUTINE void _Timestamp_To_timespec(
  const Timestamp_Control *_timestamp,
  struct timespec         *_timespec
)
{
  bintime2timespec( _timestamp, _timespec );
}

/**
 *  @brief Convert timestamp to struct timeval.
 *
 *  @param[in] _timestamp points to the timestamp
 *  @param[in] _timeval points to the timeval
 */
RTEMS_INLINE_ROUTINE void _Timestamp_To_timeval(
  const Timestamp_Control *_timestamp,
  struct timeval          *_timeval
)
{
  bintime2timeval( _timestamp, _timeval );
}

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
