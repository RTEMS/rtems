/**
 * @file
 *
 * @ingroup ClassicTimerImpl
 *
 * @brief Classic Timer Implementation
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TIMER_INL
#define _RTEMS_RTEMS_TIMER_INL

#include <rtems/rtems/timer.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdogimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicTimerImpl Classic Timer Implementation
 *
 * @ingroup ClassicTimer
 *
 * @{
 */

/**
 *  @brief Instantiate RTEMS Timer Data
 *
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_TIMER_EXTERN
#define RTEMS_TIMER_EXTERN extern
#endif

typedef struct Timer_server_Control Timer_server_Control;

/**
 * @brief Method used for task based timers.
 */
typedef void (*Timer_server_Method)(
  Timer_server_Control *timer_server,
  Timer_Control        *timer
);

typedef struct {
  /**
   * @brief This watchdog that will be registered in the system tick mechanic
   * for timer server wake-up.
   */
  Watchdog_Control System_watchdog;

  /**
   * @brief Remaining delta of the system watchdog.
   */
  Watchdog_Interval system_watchdog_delta;

  /**
   * @brief Unique identifier of the context which deals currently with the
   * system watchdog.
   */
  Thread_Control *system_watchdog_helper;

  /**
   * @brief Each insert and tickle operation increases the generation count so
   * that the system watchdog dealer notices updates of the watchdog chain.
   */
  uint32_t generation;

  /**
   * @brief Watchdog header managed by the timer server.
   */
  Watchdog_Header Header;

  /**
   * @brief Last time snapshot of the timer server.
   *
   * The units may be ticks or seconds.
   */
  Watchdog_Interval last_snapshot;

  /**
   * @brief Current time snapshot of the timer server.
   *
   * The units may be ticks or seconds.
   */
  Watchdog_Interval current_snapshot;
} Timer_server_Watchdogs;

struct Timer_server_Control {
  /**
   * @brief The cancel method of the timer server.
   */
  Timer_server_Method cancel;

  /**
   * @brief The schedule operation method of the timer server.
   */
  Timer_server_Method schedule_operation;

  /**
   * @brief Interval watchdogs triggered by the timer server.
   */
  Timer_server_Watchdogs Interval_watchdogs;

  /**
   * @brief TOD watchdogs triggered by the timer server.
   */
  Timer_server_Watchdogs TOD_watchdogs;
};

/**
 * @brief Pointer to default timer server control block.
 *
 * This value is @c NULL when the default timer server is not initialized.
 */
RTEMS_TIMER_EXTERN Timer_server_Control *volatile _Timer_server;

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
RTEMS_TIMER_EXTERN Objects_Information  _Timer_Information;

/**
 *  @brief Timer Manager Initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Timer_Manager_initialization(void);

/**
 *  @brief Timer_Allocate
 *
 *  This function allocates a timer control block from
 *  the inactive chain of free timer control blocks.
 */
RTEMS_INLINE_ROUTINE Timer_Control *_Timer_Allocate( void )
{
  return (Timer_Control *) _Objects_Allocate( &_Timer_Information );
}

/**
 *  @brief Timer_Free
 *
 *  This routine frees a timer control block to the
 *  inactive chain of free timer control blocks.
 */
RTEMS_INLINE_ROUTINE void _Timer_Free (
  Timer_Control *the_timer
)
{
  _Objects_Free( &_Timer_Information, &the_timer->Object );
}

/**
 *  @brief Timer_Get
 *
 *  This function maps timer IDs to timer control blocks.
 *  If ID corresponds to a local timer, then it returns
 *  the timer control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the returned value is undefined.
 */
RTEMS_INLINE_ROUTINE Timer_Control *_Timer_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Timer_Control *)
    _Objects_Get( &_Timer_Information, id, location );
}

/**
 *  @brief Timer_Is_interval_class
 *
 *  This function returns TRUE if the class is that of an INTERVAL
 *  timer, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Timer_Is_interval_class (
  Timer_Classes the_class
)
{
  return (the_class == TIMER_INTERVAL) || (the_class == TIMER_INTERVAL_ON_TASK);
}

/**
 *  @brief Timer_Is_time_of_day_class
 *
 *  This function returns TRUE if the class is that of an INTERVAL
 *  timer, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Timer_Is_timer_of_day_class (
  Timer_Classes the_class
)
{
  return ( the_class == TIMER_TIME_OF_DAY );
}

/**
 *  @brief Timer_Is_dormant_class
 *
 *  This function returns TRUE if the class is that of a DORMANT
 *  timer, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Timer_Is_dormant_class (
  Timer_Classes the_class
)
{
  return ( the_class == TIMER_DORMANT );
}

void _Timer_Cancel( Timer_Control *the_timer );

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
