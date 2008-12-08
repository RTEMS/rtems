/** 
 *  @file  rtems/score/timestamp.h
 *
 *  This include file contains helpers for manipulating timestamps.
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

#ifndef _RTEMS_SCORE_TIMESTAMP_H
#define _RTEMS_SCORE_TIMESTAMP_H

/**
 *  @defgroup SuperCore Timestamp
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

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/timespec.h>

#define RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC

/**
 *   Define the Timestamp control type.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  typedef struct timespec Timestamp_Control;
#else
  #error "No Alternative implementations for SuperCore Timestamp."
#endif

/** @brief Set Timestamp to Seconds Nanosecond
 *
 *  This method sets the timestamp to the specified seconds and nanoseconds
 *  value.
 *
 *  @param[in] _time points to the timestamp instance to validate.
 *  @param[in] _seconds is the seconds portion of the timestamp
 *  @param[in] _nanoseconds is the nanoseconds portion of the timestamp
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Set( _time, _seconds, _nanoseconds ) \
          do { \
             (_time)->tv_sec = (_seconds); \
             (_time)->tv_nsec = (_nanoseconds); \
          } while (0)
#endif

/** @brief Zero Timestamp
 *
 *  This method sets the timestamp to zero.
 *  value.
 *
 *  @param[in] _time points to the timestamp instance to zero.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Set_to_zero( _time ) \
          do { \
             (_time)->tv_sec = 0; \
             (_time)->tv_nsec = 0; \
          } while (0)
#endif

/** @brief Is Timestamp Valid
 *
 *  This method determines the validity of a timestamp.
 *
 *  @param[in] time is the timestamp instance to validate.
 *
 *  @return This method returns true if @a time is valid and 
 *          false otherwise.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Is_valid( _time ) \
          _Timespec_Is_valid( _time )
#endif

/** @brief Timestamp Less Than Operator
 *
 *  This method is the less than operator for timestamps.
 *
 *  @param[in] lhs is the left hand side timestamp
 *  @param[in] rhs is the right hand side timestamp
 *
 *  @return This method returns true if @a lhs is less than the @a rhs and 
 *          false otherwise.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Less_than( _lhs, _rhs ) \
          _Timespec_Less_than( _lhs, _rhs )
#endif

/** @brief Timestamp Greater Than Operator
 *
 *  This method is the greater than operator for timestamps.
 *
 *  @param[in] lhs is the left hand side timestamp
 *  @param[in] rhs is the right hand side timestamp
 *
 *  @return This method returns true if @a lhs is greater than the @a rhs and 
 *          false otherwise.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Greater_than( _lhs, _rhs ) \
          _Timespec_Greater_than( _lhs, _rhs )
#endif

/** @brief Timestamp equal to Operator
 *
 *  This method is the is equal to than operator for timestamps.
 *
 *  @param[in] lhs is the left hand side timestamp
 *  @param[in] rhs is the right hand side timestamp
 *
 *  @return This method returns true if @a lhs is equal to  @a rhs and 
 *          false otherwise.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Equal_to( _lhs, _rhs ) \
          _Timespec_Equal_to( _lhs, _rhs )
#endif

/** @brief Add to a Timestamp
 *
 *  This routine adds two timestamps.  The second argument is added
 *  to the first.
 *
 *  @param[in] time points to the base time to be added to
 *  @param[in] add points to the timestamp to add to the first argument 
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Add_to( _time, _add ) \
          _Timespec_Add_to( _time, _add )
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
 *  @param[in] time points to the base time to be added to
 *  @param[in] add points to the timestamp to add to the first argument 
 *
 *  @return This method returns the number of seconds @a time increased by.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Add_to_at_tick( _time, _add ) \
          _Timespec_Add_to( _time, _add )
#endif

/** @brief Convert Timestamp to Number of Ticks
 *
 *  This routine convert the @a time timestamp to the corresponding number
 *  of clock ticks.
 *
 *  @param[in] time points to the time to be converted
 *
 *  @return This method returns the number of ticks computed.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_To_ticks( _time ) \
          _Timespec_To_ticks( _time )
#endif

/** @brief Convert Ticks to Timestamp
 *
 *  This routine converts the @a ticks value to the corresponding
 *  timestamp format @a time.
 *
 *  @param[in] time points to the timestamp format time result
 *  @param[in] ticks points to the the number of ticks to be filled in
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_From_ticks( _ticks, _time ) \
          _Timespec_From_ticks( _ticks, _time )
#endif

/** @brief Subtract Two Timestamp
 *
 *  This routine subtracts two timestamps.  @a result is set to
 *  @a end - @a start.
 *
 *  @param[in] start points to the starting time
 *  @param[in] end points to the ending time
 *  @param[in] result points to the difference between starting and ending time.
 *
 *  @return This method fills in @a result.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Subtract( _start, _end, _result ) \
          _Timespec_Subtract( _start, _end, _result )
#endif

/** @brief Divide Timestamp By Integer
 *
 *  This routine divides a timestamp by an integer value.  The expected
 *  use is to assist in benchmark calculations where you typically 
 *  divide a duration by a number of iterations.
 *
 *  @param[in] time points to the total
 *  @param[in] iterations is the number of iterations
 *  @param[in] result points to the average time.
 *
 *  @return This method fills in @a result.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Divide_by_integer( _time, _iterations, _result ) \
          _Timespec_Divide_by_integer(_time, _iterations, _result )
#endif

/** @brief Divide Timestamp
 *
 *  This routine divides a timestamp by another timestamp.  The 
 *  intended use is for calculating percentages to three decimal points.
 *
 *  @param[in] lhs points to the left hand number
 *  @param[in] rhs points to the right hand number
 *  @param[in] ival_percentage is the integer portion of the average
 *  @param[in] fval_percentage is the thousandths of percentage
 *
 *  @return This method fills in @a result.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Divide( _lhs, _rhs, _ival_percentage, _fval_percentage ) \
          _Timespec_Divide( _lhs, _rhs, _ival_percentage, _fval_percentage )
#endif

/** @brief Get Seconds Portion of Timestamp
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @return The seconds portion of @a _time.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Get_seconds( _time ) \
          ((_time)->tv_sec)
#endif

/** @brief Get Nanoseconds Portion of Timestamp
 *
 *  This method returns the nanoseconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @return The nanoseconds portion of @a _time.
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_Get_nanoseconds( _time ) \
          ((_time)->tv_nsec)
#endif

/** @brief Convert Timestamp to struct timespec
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _timestamp points to the timestamp
 *  @param[in] _timespec points to the timespec
 */
#if defined(RTEMS_SCORE_TIMESTAMP_IS_STRUCT_SPEC)
  #define _Timestamp_To_timespec( _timestamp, _timespec  ) \
          *(_timespec) = *(_timestamp) 
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
