/**
 *  @file  rtems/score/timespec.h
 *
 *  This include file contains helpers for manipulating timespecs.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TIMESPEC_H
#define _RTEMS_SCORE_TIMESPEC_H

/**
 *  @defgroup Timespec Helpers
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to manipulating
 *  POSIX struct timespecs.
 */
/**@{*/

#include <stdbool.h> /* bool */
#include <stdint.h> /* uint32_t */
#include <time.h> /* struct timespec */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set timespec to seconds nanosecond.
 *
 *  This method sets the timespec to the specified seconds and nanoseconds
 *  value.
 *
 *  @param[in] _time points to the timespec instance to validate.
 *  @param[in] _seconds is the seconds portion of the timespec
 *  @param[in] _nanoseconds is the nanoseconds portion of the timespec
 */
#define _Timespec_Set( _time, _seconds, _nanoseconds ) \
	do { \
	   (_time)->tv_sec = (_seconds); \
	   (_time)->tv_nsec = (_nanoseconds); \
	} while (0)

/**
 * @brief Sets the Timespec to Zero
 *
 *  This method sets the timespec to zero.
 *  value.
 *
 *  @param[in] _time points to the timespec instance to zero.
 */
#define _Timespec_Set_to_zero( _time ) \
	do { \
	   (_time)->tv_sec = 0; \
	   (_time)->tv_nsec = 0; \
	} while (0)

/**
 * @brief Get seconds portion of timespec.
 *
 *  This method returns the seconds portion of the specified timespec
 *
 *  @param[in] _time points to the timespec
 *
 *  @retval The seconds portion of @a _time.
 */
#define _Timespec_Get_seconds( _time ) \
	((_time)->tv_sec)

/**
 * @brief Get nanoseconds portion of timespec.
 *
 *  This method returns the nanoseconds portion of the specified timespec
 *
 *  @param[in] _time points to the timespec
 *
 *  @retval The nanoseconds portion of @a _time.
 */
#define _Timespec_Get_nanoseconds( _time ) \
	((_time)->tv_nsec)

/**
 *  @brief Get the timestamp as nanoseconds.
 *
 *  This method returns the timestamp as nanoseconds.
 *
 *  @param[in] time points to the timestamp.
 *
 *  @retval The time in nanoseconds.
 */
uint64_t _Timespec_Get_as_nanoseconds(
  const struct timespec *time
);

/**
 * @brief Check if timespec is valid.
 *
 *  This method determines the validity of a timespec.
 *
 *  @param[in] time is the timespec instance to validate.
 *
 *  @retval This method returns true if @a time is valid and
 *          false otherwise.
 */
bool _Timespec_Is_valid(
  const struct timespec *time
);

/**
 *  @brief The Timespec "less than" operator.
 *
 *  This method is the less than operator for timespecs.
 *
 *  @param[in] lhs is the left hand side timespec
 *  @param[in] rhs is the right hand side timespec
 *
 *  @retval This method returns true if @a lhs is less than the @a rhs and
 *          false otherwise.
 */
bool _Timespec_Less_than(
  const struct timespec *lhs,
  const struct timespec *rhs
);

/**
 * @brief The Timespec "greater than" operator.
 *
 *  This method is the greater than operator for timespecs.
 *
 *  @param[in] _lhs is the left hand side timespec
 *  @param[in] _rhs is the right hand side timespec
 *
 *  @retval This method returns true if @a lhs is greater than the @a rhs and
 *          false otherwise.
 */
#define _Timespec_Greater_than( _lhs, _rhs ) \
  _Timespec_Less_than( _rhs, _lhs )

/**
 * @brief The Timespec "equal to" operator.
 *
 *  This method is the is equal to than operator for timespecs.
 *
 *  @param[in] lhs is the left hand side timespec
 *  @param[in] rhs is the right hand side timespec
 *
 *  @retval This method returns true if @a lhs is equal to  @a rhs and
 *          false otherwise.
 */
#define _Timespec_Equal_to( lhs, rhs ) \
  ( ((lhs)->tv_sec  == (rhs)->tv_sec) &&   \
    ((lhs)->tv_nsec == (rhs)->tv_nsec)     \
  )

/**
 *  @brief Add two timespecs.
 *
 *  This routine adds two timespecs.  The second argument is added
 *  to the first.
 *
 *  @param[in] time is the base time to be added to
 *  @param[in] add is the timespec to add to the first argument
 *
 *  @retval This method returns the number of seconds @a time increased by.
 */
uint32_t _Timespec_Add_to(
  struct timespec       *time,
  const struct timespec *add
);

/**
 * @brief Convert timespec to number of ticks.
 *
 *  This routine convert the @a time timespec to the corresponding number
 *  of clock ticks.
 *
 *  @param[in] time is the time to be converted
 *
 *  @retval This method returns the number of ticks computed.
 */
uint32_t _Timespec_To_ticks(
  const struct timespec *time
);

/**
 * @brief Convert ticks to timespec.
 *
 *  This routine converts the @a ticks value to the corresponding
 *  timespec format @a time.
 *
 *  @param[in] time is the timespec format time result
 *  @param[in] ticks is the number of ticks to convert
 */
void _Timespec_From_ticks(
  uint32_t         ticks,
  struct timespec *time
);

/**
 * @brief Subtract two timespec.
 *
 *  This routine subtracts two timespecs.  @a result is set to
 *  @a end - @a start.
 *
 *  @param[in] start is the starting time
 *  @param[in] end is the ending time
 *  @param[in] result is the difference between starting and ending time.
 *
 *  @retval This method fills in @a result.
 */
void _Timespec_Subtract(
  const struct timespec *start,
  const struct timespec *end,
  struct timespec       *result
);

/**
 * @brief Divide timespec by an integer.
 *
 *  This routine divides a timespec by an integer value.  The expected
 *  use is to assist in benchmark calculations where you typically
 *  divide a duration by a number of iterations.
 *
 *  @param[in] time is the total
 *  @param[in] iterations is the number of iterations
 *  @param[in] result is the average time.
 *
 *  @retval This method fills in @a result.
 */
void _Timespec_Divide_by_integer(
  const struct timespec *time,
  uint32_t               iterations,
  struct timespec       *result
);

/**
 * @brief Divide a timespec by anonther timespec.
 *
 *  This routine divides a timespec by another timespec.  The
 *  intended use is for calculating percentages to three decimal points.
 *
 *  @param[in] lhs is the left hand number
 *  @param[in] rhs is the right hand number
 *  @param[in] ival_percentage is the integer portion of the average
 *  @param[in] fval_percentage is the thousandths of percentage
 *
 *  @retval This method fills in @a result.
 */
void _Timespec_Divide(
  const struct timespec *lhs,
  const struct timespec *rhs,
  uint32_t              *ival_percentage,
  uint32_t              *fval_percentage
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
