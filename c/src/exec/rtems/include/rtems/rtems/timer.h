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
 *     + set timer to fire in context of clock tick 
 *        - after a number of ticks have passed
 *        - when a specified date and time has been reached
 *     + initiate the timer server task
 *     + set timer to fire in context of the timer server task
 *        - after a number of ticks have passed
 *        - when a specified date and time has been reached
 *     + reset a timer
 *     + cancel a time
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
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
#include <rtems/rtems/attr.h>

/*
 *  The following enumerated type details the classes to which a timer
 *  may belong.
 */

typedef enum {
  TIMER_INTERVAL,
  TIMER_INTERVAL_ON_TASK,
  TIMER_TIME_OF_DAY,
  TIMER_TIME_OF_DAY_ON_TASK,
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
 *  Pointer to TCB of the Timer Server.  This is NULL before the
 *  server is executing and task-based timers are not allowed to be
 *  initiated until the server is started.
 */

RTEMS_EXTERN Thread_Control *_Timer_Server;

/*  
 *  The following chains contain the list of interval timers that are
 *  executed in the context of the Timer Server.
 *
 *  NOTE: These are extern'ed because they do not have to be in the
 *        minimum footprint.  They are only really required when 
 *        task-based timers are used.  Since task-based timers can
 *        not be started until the server is initiated, these structures
 *        do not have to be initialized until then.  They are declared
 *        in the same file as _Timer_Server_body.
 */   

extern Chain_Control _Timer_Ticks_chain;
extern Chain_Control _Timer_Seconds_chain;

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
 *  _Timer_Server_body
 *
 *  DESCRIPTION:
 *
 *  This is the server for task based timers.  This task executes whenever
 *  a task-based timer should fire.  It services both "after" and "when"
 *  timers.  It is not created automatically but must be created explicitly
 *  by the application before task-based timers may be initiated.
 */

Thread _Timer_Server_body(
  unsigned32 ignored
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
 *  initiates the timer associated with ID to fire in ticks clock ticks.
 *  When the timer fires, the routine will be invoked in the context
 *  of the rtems_clock_tick directive which is normally invoked as
 *  part of servicing a periodic interupt.
 */

rtems_status_code rtems_timer_fire_after(
  Objects_Id                         id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
);

/*
 *  rtems_timer_server_fire_after
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_server_fire_after directive.  It
 *  initiates the timer associated with ID to fire in ticks clock
 *  ticks.  When the timer fires, the routine will be invoked by the
 *  Timer Server in the context of a task NOT IN THE CONTEXT of the
 *  clock tick interrupt.
 */

rtems_status_code rtems_timer_server_fire_after(
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
 *  When the timer fires, the routine will be invoked in the context
 *  of the rtems_clock_tick directive which is normally invoked as
 *  part of servicing a periodic interupt.
 */

rtems_status_code rtems_timer_fire_when(
  Objects_Id                          id,
  rtems_time_of_day                  *wall_time,
  rtems_timer_service_routine_entry   routine,
  void                               *user_data
);

/*
 *  rtems_timer_server_fire_when
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_server_fire_when directive.  It
 *  initiates the timer associated with ID to fire at wall_time
 *  When the timer fires, the routine will be invoked by the
 *  Timer Server in the context of a task NOT IN THE CONTEXT of the
 *  clock tick interrupt.
 */

rtems_status_code rtems_timer_server_fire_when(
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

/*
 *  rtems_timer_initiate_server
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_initiate_server directive.
 *  It creates and starts the server that executes task-based timers.
 *  It must be invoked before any task-based timers can be initiated.
 */

#define RTEMS_TIMER_SERVER_DEFAULT_PRIORITY -1

rtems_status_code rtems_timer_initiate_server(
  unsigned32           priority,
  unsigned32           stack_size,
  rtems_attribute      attribute_set
);

/*
 *  rtems_timer_get_information
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_timer_get_information directive.
 *  This directive returns information about the timer.
 */

typedef struct {
  Timer_Classes      the_class;
  Watchdog_Interval  initial; 
  Watchdog_Interval  start_time; 
  Watchdog_Interval  stop_time; 
} rtems_timer_information;

rtems_status_code rtems_timer_get_information(
  Objects_Id               id,
  rtems_timer_information *the_info
);

/*
 *  Macros and routines that expose the mechanisms required to service
 *  the Timer Server timer.  These stop the timer, synchronize it with
 *  the current time, and restart it.
 */

extern Watchdog_Control _Timer_Seconds_timer;

#define _Timer_Server_stop_ticks_timer() \
      _Watchdog_Remove( &_Timer_Server->Timer )

#define _Timer_Server_stop_seconds_timer() \
      _Watchdog_Remove( &_Timer_Seconds_timer );

void _Timer_Server_process_ticks_chain(void);
void _Timer_Server_process_seconds_chain(void);

#define _Timer_Server_reset_ticks_timer() \
   do { \
      if ( !_Chain_Is_empty( &_Timer_Ticks_chain ) ) { \
        _Watchdog_Insert_ticks( &_Timer_Server->Timer, \
           ((Watchdog_Control *)_Timer_Ticks_chain.first)->delta_interval ); \
      } \
   } while (0)

#define _Timer_Server_reset_seconds_timer() \
   do { \
      if ( !_Chain_Is_empty( &_Timer_Seconds_chain ) ) { \
        _Watchdog_Insert_seconds( &_Timer_Seconds_timer, \
          ((Watchdog_Control *)_Timer_Seconds_chain.first)->delta_interval ); \
      } \
   } while (0)

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/timer.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
