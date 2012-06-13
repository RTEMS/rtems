/**
 *  @file  rtems/score/timestamp.h
 *
 *  This include file contains helpers for manipulating timestamps.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TIMESTAMP_H
#define _RTEMS_SCORE_TIMESTAMP_H

/**
 *  @defgroup SuperCore Timestamp
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

#include <rtems/score/cpu.h>
#include <rtems/score/timespec.h>

#ifdef __cplusplus
extern "C" {
#endif

#if ! ( ( CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE \
    && CPU_TIMESTAMP_USE_INT64 == FALSE \
    && CPU_TIMESTAMP_USE_INT64_INLINE == FALSE ) \
  || ( CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == FALSE \
    && CPU_TIMESTAMP_USE_INT64 == TRUE \
    && CPU_TIMESTAMP_USE_INT64_INLINE == FALSE ) \
  || ( CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == FALSE \
    && CPU_TIMESTAMP_USE_INT64 == FALSE \
    && CPU_TIMESTAMP_USE_INT64_INLINE == TRUE ) )
  #error "Invalid SuperCore Timestamp implementations selection."
#endif

#if CPU_TIMESTAMP_USE_INT64 == TRUE || CPU_TIMESTAMP_USE_INT64_INLINE == TRUE
  #include <rtems/score/timestamp64.h>
#endif

/**
 *   Define the Timestamp control type.
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  typedef struct timespec Timestamp_Control;
#else
  typedef Timestamp64_Control Timestamp_Control;
#endif

/**
 *  @brief Set Timestamp to Seconds Nanosecond
 *
 *  This method sets the timestamp to the specified seconds and nanoseconds
 *  value.
 *
 *  @param[in] _time points to the timestamp instance to validate.
 *  @param[in] _seconds is the seconds portion of the timestamp
 *  @param[in] _nanoseconds is the nanoseconds portion of the timestamp
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Set( _time, _seconds, _nanoseconds ) \
          _Timespec_Set( _time, _seconds, _nanoseconds )
#else
  #define _Timestamp_Set( _time, _seconds, _nanoseconds ) \
	  _Timestamp64_Set( _time, _seconds, _nanoseconds )
#endif

/**
 *  @brief Zero Timestamp
 *
 *  This method sets the timestamp to zero.
 *  value.
 *
 *  @param[in] _time points to the timestamp instance to zero.
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Set_to_zero( _time ) \
          _Timespec_Set_to_zero( _time )
#else
  #define _Timestamp_Set_to_zero( _time ) \
	  _Timestamp64_Set_to_zero( _time )
#endif

/**
 *  @brief Is Timestamp Valid
 *
 *  This method determines the validity of a timestamp.
 *
 *  @param[in] _time points to the timestamp instance to validate.
 *
 *  @return This method returns true if @a time is valid and
 *          false otherwise.
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Is_valid( _time ) \
          _Timespec_Is_valid( _time )
#else
  #define _Timestamp_Is_valid( _time ) \
          _Timestamp64_Is_valid( _time )
#endif

/**
 *  @brief Timestamp Less Than Operator
 *
 *  This method is the less than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @return This method returns true if @a _lhs is less than the @a _rhs and
 *          false otherwise.
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Less_than( _lhs, _rhs ) \
          _Timespec_Less_than( _lhs, _rhs )
#else
  #define _Timestamp_Less_than( _lhs, _rhs ) \
	  _Timestamp64_Less_than( _lhs, _rhs )
#endif

/**
 *  @brief Timestamp Greater Than Operator
 *
 *  This method is the greater than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @return This method returns true if @a _lhs is greater than the @a _rhs and
 *          false otherwise.
 */
#define _Timestamp_Greater_than( _lhs, _rhs ) \
  _Timestamp_Less_than( _rhs, _lhs )

/**
 *  @brief Timestamp equal to Operator
 *
 *  This method is the is equal to than operator for timestamps.
 *
 *  @param[in] _lhs points to the left hand side timestamp
 *  @param[in] _rhs points to the right hand side timestamp
 *
 *  @return This method returns true if @a _lhs is equal to  @a _rhs and
 *          false otherwise.
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Equal_to( _lhs, _rhs ) \
          _Timespec_Equal_to( _lhs, _rhs )
#else
  #define _Timestamp_Equal_to( _lhs, _rhs ) \
	  _Timestamp64_Equal_to( _lhs, _rhs )
#endif

/**
 *  @brief Add to a Timestamp
 *
 *  This routine adds two timestamps.  The second argument is added
 *  to the first.
 *
 *  @param[in] _time points to the base time to be added to
 *  @param[in] _add points to the timestamp to add to the first argument
 *
 *  @return This method returns the number of seconds @a time increased by.
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Add_to( _time, _add ) \
          _Timespec_Add_to( _time, _add )
#else
  #define _Timestamp_Add_to( _time, _add ) \
	  _Timestamp64_Add_to( _time, _add )
#endif

/**
 *  @brief Convert Timestamp to Number of Ticks
 *
 *  This routine convert the @a time timestamp to the corresponding number
 *  of clock ticks.
 *
 *  @param[in] _time points to the time to be converted
 *
 *  @return This method returns the number of ticks computed.
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_To_ticks( _time ) \
          _Timespec_To_ticks( _time )
#else
  #define _Timestamp_To_ticks( _time ) \
          _Timestamp64_To_ticks( _time )
#endif

/**
 *  @brief Convert Ticks to Timestamp
 *
 *  This routine converts the @a _ticks value to the corresponding
 *  timestamp format @a _time.
 *
 *  @param[in] _time points to the timestamp format time result
 *  @param[in] _ticks points to the number of ticks to be filled in
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_From_ticks( _ticks, _time ) \
          _Timespec_From_ticks( _ticks, _time )
#else
  #define _Timestamp_From_ticks( _ticks, _time ) \
          _Timestamp64_From_ticks( _ticks, _time )
#endif

/**
 *  @brief Subtract Two Timestamp
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
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Subtract( _start, _end, _result ) \
          _Timespec_Subtract( _start, _end, _result )
#else
  #define _Timestamp_Subtract( _start, _end, _result ) \
	  _Timestamp64_Subtract( _start, _end, _result )
#endif

/**
 *  @brief Divide Timestamp By Integer
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
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Divide_by_integer( _time, _iterations, _result ) \
          _Timespec_Divide_by_integer(_time, _iterations, _result )
#else
  #define _Timestamp_Divide_by_integer( _time, _iterations, _result ) \
	  _Timestamp64_Divide_by_integer( _time, _iterations, _result )
#endif

/**
 *  @brief Divide Timestamp
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
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Divide( _lhs, _rhs, _ival_percentage, _fval_percentage ) \
          _Timespec_Divide( _lhs, _rhs, _ival_percentage, _fval_percentage )
#else
  #define _Timestamp_Divide( _lhs, _rhs, _ival_percentage, _fval_percentage ) \
          _Timestamp64_Divide( _lhs, _rhs, _ival_percentage, _fval_percentage )
#endif

/**
 *  @brief Get Seconds Portion of Timestamp
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @return The seconds portion of @a _time.
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Get_seconds( _time ) \
          _Timespec_Get_seconds( _time )
#else
  #define _Timestamp_Get_seconds( _time ) \
	  _Timestamp64_Get_seconds( _time )
#endif

/**
 *  @brief Get Nanoseconds Portion of Timestamp
 *
 *  This method returns the nanoseconds portion of the specified timestamp
 *
 *  @param[in] _time points to the timestamp
 *
 *  @return The nanoseconds portion of @a _time.
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  #define _Timestamp_Get_nanoseconds( _time ) \
          _Timespec_Get_nanoseconds( _time )
#else
  #define _Timestamp_Get_nanoseconds( _time ) \
	  _Timestamp64_Get_nanoseconds( _time )
#endif

/**
 *  @brief Convert Timestamp to struct timespec
 *
 *  This method returns the seconds portion of the specified timestamp
 *
 *  @param[in] _timestamp points to the timestamp
 *  @param[in] _timespec points to the timespec
 */
#if CPU_TIMESTAMP_USE_STRUCT_TIMESPEC == TRUE
  /* in this case we know they are the same type so use simple assignment */
  #define _Timestamp_To_timespec( _timestamp, _timespec  ) \
          *(_timespec) = *(_timestamp)
#else
  #define _Timestamp_To_timespec( _timestamp, _timespec  ) \
	  _Timestamp64_To_timespec( _timestamp, _timespec  )
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
