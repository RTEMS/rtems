/**
 * @file rtems/sapi/timespec.inl
 *
 * @ingroup 
 *
 * @brief Timespec API.
 */
 
/*
 *  Copyright (c) 2012-.
 *  Krzysztof Miesowicz krzysztof.miesowicz@gmail.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TIMESPEC_H
# error "Never use <rtems/timespec.inl> directly; include <rtems/timespec.h> instead."
#endif

#ifndef _RTEMS_TIMESPEC_INL
#define _RTEMS_TIMESPEC_INL

#include <rtems/score/timespec.h>

/**
 * @addtogroup TimespecAPI Timespec
 *
 * @{
 */

/**
 * @brief Is Timespec Valid
 *
 *  This method determines the validity of a timespec.
 *
 *  @param[in] time is the timespec instance to validate.
 *
 *  @return This method returns true if @a time is valid and
 *          false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_timespec_is_valid(
  const struct timespec *time
)
{
  return _Timespec_Is_valid(time);
}

/**
 * @brief Timespec Less Than Operator
 *
 *  This method is the less than operator for timespecs.
 *
 *  @param[in] lhs is the left hand side timespec
 *  @param[in] rhs is the right hand side timespec
 *
 *  @return This method returns true if @a lhs is less than the @a rhs and
 *          false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_timespec_less_than(
  const struct timespec *lhs,
  const struct timespec *rhs
)
{
  return _Timespec_Less_than(lhs,rhs);
}

/**
 * @brief Add to a Timespec
 *
 *  This routine adds two timespecs.  The second argument is added
 *  to the first.
 *
 *  @param[in] time is the base time to be added to
 *  @param[in] add is the timespec to add to the first argument
 *
 *  @return This method returns the number of seconds @a time increased by.
 */
RTEMS_INLINE_ROUTINE uint32_t rtems_timespec_add_to(
  struct timespec       *time,
  const struct timespec *add
)
{
  return _Timespec_Add_to(time,add);
}

/**
 * @brief Convert Timespec to Number of Ticks
 *
 *  This routine convert the @a time timespec to the corresponding number
 *  of clock ticks.
 *
 *  @param[in] time is the time to be converted
 *
 *  @return This method returns the number of ticks computed.
 */
RTEMS_INLINE_ROUTINE uint32_t rtems_timespec_to_ticks(
  const struct timespec *time
)
{
  return _Timespec_To_ticks(time);
}

/**
 * @brief Convert Ticks to Timespec
 *
 *  This routine converts the @a ticks value to the corresponding
 *  timespec format @a time.
 *
 *  @param[in] time is the timespec format time result
 *  @param[in] ticks is the number of ticks to convert
 */

RTEMS_INLINE_ROUTINE void rtems_timespec_from_ticks(
  uint32_t         ticks,
  struct timespec *time
)
{
  _Timespec_From_ticks(ticks,time);
}

/**
 * @brief Subtract Two Timespec
 *
 *  This routine subtracts two timespecs.  @a result is set to
 *  @a end - @a start.
 *
 *  @param[in] start is the starting time
 *  @param[in] end is the ending time
 *  @param[in] result is the difference between starting and ending time.
 *
 *  @return This method fills in @a result.
 */
RTEMS_INLINE_ROUTINE void rtems_timespec_subtract(
  const struct timespec *start,
  const struct timespec *end,
  struct timespec       *result
)
{
  _Timespec_Subtract(start,end,result);
}

/**
 * @brief Divide Timespec By Integer
 *
 *  This routine divides a timespec by an integer value.  The expected
 *  use is to assist in benchmark calculations where you typically
 *  divide a duration by a number of iterations.
 *
 *  @param[in] time is the total
 *  @param[in] iterations is the number of iterations
 *  @param[in] result is the average time.
 *
 *  @return This method fills in @a result.
 */
RTEMS_INLINE_ROUTINE void rtems_timespec_divide_by_integer(
  const struct timespec *time,
  uint32_t               iterations,
  struct timespec       *result
)
{
  _Timespec_Divide_by_integer(time,iterations,result);
}

/**
 * @brief Divide Timespec
 *
 *  This routine divides a timespec by another timespec.  The
 *  intended use is for calculating percentages to three decimal points.
 *
 *  @param[in] lhs is the left hand number
 *  @param[in] rhs is the right hand number
 *  @param[in] ival_percentage is the integer portion of the average
 *  @param[in] fval_percentage is the thousandths of percentage
 *
 *  @return This method fills in @a result.
 */
RTEMS_INLINE_ROUTINE void rtems_timespec_divide(
  const struct timespec *lhs,
  const struct timespec *rhs,
  uint32_t              *ival_percentage,
  uint32_t              *fval_percentage
)
{
  _Timespec_Divide(lhs,rhs,ival_percentage,fval_percentage);
}

/**
 * @brief Set Timespec to Seconds Nanosecond
 *
 *  This method sets the timespec to the specified seconds and nanoseconds
 *  value.
 *
 *  @param[in] _time points to the timespec instance to validate.
 *  @param[in] _seconds is the seconds portion of the timespec
 *  @param[in] _nanoseconds is the nanoseconds portion of the timespec
 */
RTEMS_INLINE_ROUTINE void rtems_timespec_set(
  struct timespec *_time,
  time_t _seconds,
  uint32_t _nanoseconds
)
{
  _Timespec_Set( _time, _seconds, _nanoseconds );
}

/**
 * @brief Zero Timespec
 *
 *  This method sets the timespec to zero.
 *  value.
 *
 *  @param[in] _time points to the timespec instance to zero.
 */
RTEMS_INLINE_ROUTINE void rtems_timespec_zero( 
  struct timespec *_time
)
{
  _Timespec_Set_to_zero( _time );
}

/**
 * @brief Get Seconds Portion of Timespec
 *
 *  This method returns the seconds portion of the specified timespec
 *
 *  @param[in] _time points to the timespec
 *
 *  @return The seconds portion of @a _time.
 */
RTEMS_INLINE_ROUTINE time_t rtems_timespec_get_seconds(
  struct timespec *_time
)
{
  return _Timespec_Get_seconds( _time );
}

/**
 * @brief Get Nanoseconds Portion of Timespec
 *
 *  This method returns the nanoseconds portion of the specified timespec
 *
 *  @param[in] _time points to the timespec
 *
 *  @return The nanoseconds portion of @a _time.
 */
RTEMS_INLINE_ROUTINE uint32_t rtems_timespec_get_nanoseconds(
  struct timespec *_time
)
{
  return _Timespec_Get_nanoseconds( _time );
}


/**
 * @brief Timespec Greater Than Operator
 *
 *  This method is the greater than operator for timespecs.
 *
 *  @param[in] lhs is the left hand side timespec
 *  @param[in] rhs is the right hand side timespec
 *
 *  @return This method returns true if @a lhs is greater than the @a rhs and
 *          false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_timespec_greater_than(
  const struct timespec *_lhs,
  const struct timespec *_rhs
)
{
  return _Timespec_Greater_than( _lhs, _rhs );
}
/**
 * @brief Timespec equal to Operator
 *
 *  This method is the is equal to than operator for timespecs.
 *
 *  @param[in] lhs is the left hand side timespec
 *  @param[in] rhs is the right hand side timespec
 *
 *  @return This method returns true if @a lhs is equal to  @a rhs and
 *          false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_timespec_equal_to(
  const struct timespec *lhs,
  const struct timespec *rhs
)
{
  return _Timespec_Equal_to( lhs, rhs);
}

/** @} */

#endif
/* end of include file */
