/*  timer.h
 *
 *  This include file contains all the constants, structures, and
 *  prototypes associated with the Timer Manager.  This manager provides
 *  facilities to configure, initiate, cancel, and delete timers which will
 *  fire at specified intervals of time.
 *
 *  Directives provided are:
 *
 *     + create a timer
 *     + get an ID of a timer
 *     + delete a timer
 *     + set a timer to fire after a number of ticks have passed
 *     + set a timer to fire when a specified date and time has been reached
 *     + reset a timer
 *     + cancel a time
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __RTEMS_TIMER_h
#define __RTEMS_TIMER_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*
 *  The following enumerated type details the classes to which a timer
 *  may belong.
 */

typedef enum {
  TIMER_INTERVAL,
  TIMER_TIME_OF_DAY,
  TIMER_DORMANT
} Timer_Classes;

/*
 *  The following types define a pointer to a timer service routine.
 */
 
typedef void rtems_timer_service_routine;
 
typedef rtems_timer_service_routine ( *rtems_timer_service_routine_entry )(
                 rtems_id,
                 void *
             );

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

RTEMS_EXTERN Objects_Information  _Timer_Information;

/*
 *  The following records define the control block used to manage
 *  each timer.
 */

typedef struct {
  Objects_Control  Object;
  Watchdog_Control Ticker;
  Timer_Classes    the_class;
}   Timer_Control;

/*
 *  _Timer_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Timer_Manager_initialization(
  unsigned32 maximum_timers
);

/*
 *  rtems_timer_create
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_create directive.  The
 *  timer will have the name name.  It returns the id of the
 *  created timer in ID.
 */

rtems_status_code rtems_timer_create(
  rtems_name    name,
  Objects_Id   *id
);

/*
 *  rtems_timer_ident
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_ident directive.
 *  This directive returns the timer ID associated with name.
 *  If more than one timer is named name, then the timer
 *  to which the ID belongs is arbitrary.
 */

rtems_status_code rtems_timer_ident(
  rtems_name    name,
  Objects_Id   *id
);

/*
 *  rtems_timer_cancel
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_cancel directive.  It is used
 *  to stop the timer associated with ID from firing.
 */

rtems_status_code rtems_timer_cancel(
  Objects_Id id
);

/*
 *  rtems_timer_delete
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_delete directive.  The
 *  timer indicated by ID is deleted.
 */

rtems_status_code rtems_timer_delete(
  Objects_Id id
);

/*
 *  rtems_timer_fire_after
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_fire_after directive.  It
 *  initiates the timer associated with ID to fire in ticks clock
 *  ticks.  When the timer fires, the routine will be invoked.
 */

rtems_status_code rtems_timer_fire_after(
  Objects_Id                         id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
);

/*
 *  rtems_timer_fire_when
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_fire_when directive.  It
 *  initiates the timer associated with ID to fire at wall_time
 *  When the timer fires, the routine will be invoked.
 */

rtems_status_code rtems_timer_fire_when(
  Objects_Id                          id,
  rtems_time_of_day                  *wall_time,
  rtems_timer_service_routine_entry   routine,
  void                               *user_data
);

/*
 *  rtems_timer_reset
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_reset directive.  It is used
 *  to reinitialize the interval timer associated with ID just as if
 *  rtems_timer_fire_after were re-invoked with the same arguments that
 *  were used to initiate this timer.
 */

rtems_status_code rtems_timer_reset(
  Objects_Id id
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/timer.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
