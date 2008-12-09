/** 
 *  @file  rtems/score/timestamp64.h
 *
 *  This include file contains helpers for manipulating 
 *  64-bit integer timestamps.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_TIMESTAMP64_H
#define _RTEMS_SCORE_TIMESTAMP64_H

/**
 *  @defgroup SuperCore Timestamp64
 *
 *  This handler encapsulates functionality related to manipulating
 *  the 64 bit integer implementation of SuperCore Timestamps.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This .h file is not for general use.  It is an alternative
 *  implementation of Timestamps and should only be used that way.
 */
#ifndef _RTEMS_SCORE_TIMESTAMP_H
  #error "Should only be included by rtems/score/timestamp.h"
#endif

/*
 *  Verify something is defined.
 */
#if !defined(CPU_RTEMS_SCORE_TIMESTAMP_IS_INT64)
  #error "SuperCore Timestamp64 implementation included but not defined."
#endif

/**
 *   Define the Timestamp control type.
 */
typedef int64_t Timestamp64_Control;

/** @brief Set Timestamp to Seconds Nanosecond
 *
 *  This method sets the timestamp to the specified seconds and nanoseconds
 *  value.
 *
 *  @param[in] _time points to the timestamp instance to validate.
 *  @param[in] _seconds is the seconds portion of the timestamp
 *  @param[in] _nanoseconds is the nanoseconds portion of the timestamp
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Set( _time, _seconds, _nanoseconds ) \
	  do { \
	    *(_time) = ((int64_t)_seconds * 1000000000); \
	    *(_time) += (int64_t)(_nanoseconds); \
	  } while (0)
#else
  void _Timestamp64_Set(
    Timestamp64_Control *_time,
    long                _seconds,
    long                _nanoseconds
  );
#endif

/** @brief Zero Timestamp
 *
 *  This method sets the timestamp to zero.
 *  value.
 *
 *  @param[in] _time points to the timestamp instance to zero.
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Set_to_zero( _time ) \
	  *(_time) = 0
#else
  void _Timestamp64_Set_to_zero(
    Timestamp64_Control *_time
  );
#endif

/** @brief Is Timestamp Valid
 *
 *  This method determines the validity of a timestamp.
 *
 *  @param[in] _time points to the timestamp instance to validate.
 *
 *  @return This method returns true if @a time is valid and 
 *          false otherwise.
 */
#define _Timestamp64_Is_valid( _time ) \
	(1)

/** @brief Timestamp Less Than Operator
 *
 *  This method is the less than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @return This method returns true if @a _lhs is less than the @a _rhs and 
 *          false otherwise.
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Less_than( _lhs, _rhs ) \
	  (*(_lhs) < *(_rhs))
#else
  bool _Timestamp64_Less_than(
    Timestamp64_Control *_lhs,
    Timestamp64_Control *_rhs
  );
#endif

/** @brief Timestamp Greater Than Operator
 *
 *  This method is the greater than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @return This method returns true if @a _lhs is greater than the @a _rhs and 
 *          false otherwise.
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Greater_than( _lhs, _rhs ) \
	  (*(_lhs) > *(_rhs))
#else
  bool _Timestamp64_Greater_than(
    Timestamp64_Control *_lhs,
    Timestamp64_Control *_rhs
  );
#endif

/** @brief Timestamp equal to Operator
 *
 *  This method is the is equal to than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @return This method returns true if @a _lhs is equal to  @a _rhs and 
 *          false otherwise.
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Equal_to( _lhs, _rhs ) \
	  (*(_lhs) == *(_rhs))
#else
  bool _Timestamp64_Equal_to(
    Timestamp64_Control *_lhs,
    Timestamp64_Control *_rhs
  );
#endif

/** @brief Add to a Timestamp
 *
 *  This routine adds two timestamps.  The second argument is added
 *  to the first.
 *
 *  @param[in] _time points to the base time to be added to
 *  @param[in] _add points to the timestamp to add to the first argument 
 *
 *  @return This method returns the number of seconds @a time increased by.
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Add_to( _time, _add ) \
	  *(_time) += *(_add)
#else
  void _Timestamp64_Add_to(
    Timestamp64_Control *_time,
    Timestamp64_Control *_add
  );
#endif

/** @brief Add to a Timestamp (At Clock Tick)
 *
 *  This routine adds two timestamps.  The second argument is added
 *  to the first.
 *  
 *  @node This routine places a special requirement on the addition
 *        operation.  It must return the number of units that the
 *        seconds field changed as the result of the addition.  Since this
 *        operation is ONLY used as part of processing a clock tick,
 *        it is generally safe to assume that only one second changed.
 *
 *  @param[in] _time points to the base time to be added to
 *  @param[in] _add points to the timestamp to add to the first argument 
 *
 *  @return This method returns the number of seconds @a time increased by.
 */
static inline uint32_t _Timestamp64_Add_to_at_tick(
  Timestamp64_Control *_time,
  Timestamp64_Control *_add
)
{
  Timestamp64_Control start = *_time / 1000000000;
  *_time += *_add;
  if ( ((*_time) / 1000000000) != start ) {
    return 1;
  }
  return 0;
}

/** @brief Convert Timestamp to Number of Ticks
 *
 *  This routine convert the @a time timestamp to the corresponding number
 *  of clock ticks.
 *
 *  @param[in] _time points to the time to be converted
 *
 *  @return This method returns the number of ticks computed.
 */
uint32_t _Timestamp64_To_ticks(
  const Timestamp64_Control *time
);

/** @brief Convert Ticks to Timestamp
 *
 *  This routine converts the @a _ticks value to the corresponding
 *  timestamp format @a _time.
 *
 *  @param[in] time points to the timestamp format time result
 *  @param[in] ticks points to the number of ticks to be filled in
 */
void _Timestamp64_From_ticks(
  uint32_t             _ticks,
  Timestamp64_Control *_time
);

/** @brief Subtract Two Timestamp
 *
 *  This routine subtracts two timestamps.  @a result is set to
 *  @a end - @a start.
 *
 *  @param[in] _start points to the starting time
 *  @param[in] _end points to the ending time
 *  @param[in] _result points to the difference between
 *             starting and ending time.
 *
 *  @return This method fills in @a _result.
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Subtract( _start, _end, _result ) \
	  do { \
	     *(_result) = *(_end) - *(_start); \
	  } while (0)
#else
  void _Timestamp64_Subtract(
    Timestamp64_Control *_start,
    Timestamp64_Control *_end,
    Timestamp64_Control *_result
  );
#endif

/** @brief Divide Timestamp By Integer
 *
 *  This routine divides a timestamp by an integer value.  The expected
 *  use is to assist in benchmark calculations where you typically 
 *  divide a duration by a number of iterations.
 *
 *  @param[in] _time points to the total
 *  @param[in] _iterations is the number of iterations
 *  @param[in] _result points to the average time.
 *
 *  @return This method fills in @a result.
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Divide_by_integer( _time, _iterations, _result ) \
	  do { \
	     *(_result) = *(_time) / (_iterations); \
	  } while (0)
#else
  void _Timestamp64_Divide_by_integer(
    Timestamp64_Control *_time,
    uint32_t             _iterations,
    Timestamp64_Control *_result
  );
#endif

/** @brief Divide Timestamp
 *
 *  This routine divides a timestamp by another timestamp.  The 
 *  intended use is for calculating percentages to three decimal points.
 *
 *  @param[in] _lhs points to the left hand number
 *  @param[in] _rhs points to the right hand number
 *  @param[in] _ival_percentage points to the integer portion of the average
 *  @param[in] _fval_percentage points to the thousandths of percentage
 *
 *  @return This method fills in @a result.
 */
void _Timestamp64_Divide(
  const Timestamp64_Control *_lhs,
  const Timestamp64_Control *_rhs,
  uint32_t                  *_ival_percentage,
  uint32_t                  *_fval_percentage
);

/** @brief Get Seconds Portion of Timestamp
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @return The seconds portion of @a _time.
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Get_seconds( _time ) \
	  (*(_time) / 1000000000)
#else
  uint32_t _Timestamp64_Get_seconds(
    Timestamp64_Control *_time
  );
#endif

/** @brief Get Nanoseconds Portion of Timestamp
 *
 *  This method returns the nanoseconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @return The nanoseconds portion of @a _time.
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_Get_nanoseconds( _time ) \
	  (*(_time) % 1000000000)
#else
  uint32_t _Timestamp64_Get_nanoseconds(
    Timestamp64_Control *_time
  );
#endif

/** @brief Convert Timestamp to struct timespec
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _timestamp points to the timestamp
 *  @param[in] _timespec points to the timespec
 */
#if defined(CPU_RTEMS_SCORE_TIMESTAMP_INT64_INLINE)
  #define _Timestamp64_To_timespec( _timestamp, _timespec  ) \
	do { \
	  (_timespec)->tv_sec = *(_timestamp) / 1000000000; \
	  (_timespec)->tv_nsec = *(_timestamp) % 1000000000; \
	} while (0)
#else
  void _Timestamp64_To_timespec(
    Timestamp64_Control *_timestamp,
    struct timespec     *_timespec
  );
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
