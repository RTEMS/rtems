/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty

  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  rtemsTimer class.

  This class allows the user to create a RTEMS timer.

  The trigger method is called when the timer expires. The method is
  called using the thread which calls the RTEMS 'rtems_clock_tick'
  method.

  Timers are always local to a node.

  ------------------------------------------------------------------------ */

#if !defined(_rtemsTimer_h_)
#define _rtemsTimer_h_

#include <rtems++/rtemsStatusCode.h>

/* ----
    rtemsTimer
*/

class rtemsTimer
  : public rtemsStatusCode
{
public:
  // only the first 4 characters of the name are taken,

  // create a timer object
  rtemsTimer(const char* name);
  rtemsTimer();

  // destroies the actual object
  virtual ~rtemsTimer();

  // create or destroy (delete) the timer
  virtual const rtems_status_code create(const char* name);
  virtual const rtems_status_code destroy();

  // timer control
  inline const rtems_status_code fire_after(const rtems_interval micro_secs);
  inline const rtems_status_code repeat_fire_at(const rtems_interval micro_secs);
  inline const rtems_status_code fire_when(const rtems_time_of_day& when);

  inline const rtems_status_code cancel();
  inline const rtems_status_code reset();

  // object id, and name
  const rtems_id id_is() const { return id; }
  const rtems_name name_is() const { return name; }
  const char *name_string() const { return name_str; }

protected:

  // triggered method is called when the timer fires
  virtual void triggered() = 0;

private:
  // not permitted
  rtemsTimer(const rtemsTimer& timer);
  rtemsTimer& operator=(const rtemsTimer& timer);

  // make this object reference an invalid RTEMS object
  void make_invalid();

  // semaphore name
  rtems_name name;
  char name_str[5];

  // repeat true restart the timer when it fires
  bool repeat;

  // the rtems id, object handle
  rtems_id id;

  // common timer handler
  static void common_handler(rtems_id id, void *user_data);
};

const rtems_status_code rtemsTimer::fire_after(const rtems_interval micro_secs)
{
  repeat = false;
  rtems_interval usecs = micro_secs &&
    (micro_secs < rtems_configuration_get_microseconds_per_tick()) ?
    rtems_configuration_get_microseconds_per_tick()  : micro_secs;
  return set_status_code(rtems_timer_fire_after(id,
                                                RTEMS_MICROSECONDS_TO_TICKS(usecs),
                                                common_handler,
                                                this));
}

const rtems_status_code rtemsTimer::repeat_fire_at(const rtems_interval micro_secs)
{
  repeat = true;
  rtems_interval usecs = micro_secs &&
    (micro_secs < rtems_configuration_get_microseconds_per_tick()) ?
    rtems_configuration_get_microseconds_per_tick()  : micro_secs;
  return set_status_code(rtems_timer_fire_after(id,
                                                RTEMS_MICROSECONDS_TO_TICKS(usecs),
                                                common_handler,
                                                this));
}

const rtems_status_code rtemsTimer::fire_when(const rtems_time_of_day& when)
{
  return set_status_code(rtems_timer_fire_when(id,
                                               (rtems_time_of_day*) &when,
                                               common_handler,
                                               this));
}

const rtems_status_code rtemsTimer::cancel()
{
  repeat = false;
  return set_status_code(rtems_timer_cancel(id));
}

const rtems_status_code rtemsTimer::reset()
{
  return set_status_code(rtems_timer_reset(id));
}

#endif  // _rtemsTimer_h_
