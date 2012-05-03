/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty
  
  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  See header file.

  ------------------------------------------------------------------------
*/

#include <cstring>
#include <rtems++/rtemsTimer.h>

/* ----
    rtemsTimer
*/

rtemsTimer::rtemsTimer(const char* tname)
  : name(0),
    repeat(false),
    id(0)
{
  strcpy(name_str, "    ");
  create(tname);
}

rtemsTimer::rtemsTimer()
  : name(0),
    repeat(false),
    id(0)
{
  strcpy(name_str, "    ");
}

rtemsTimer::~rtemsTimer()
{
  destroy();
}

void rtemsTimer::make_invalid()
{
  strcpy(name_str, "    ");
  name = 0;
  id = 0;
  repeat = false;
}
const rtems_status_code rtemsTimer::create(const char* tname)
{
  if (id)
    return set_status_code(RTEMS_ILLEGAL_ON_SELF);

  strcpy(name_str, "    ");
  for (int c = 0; (c < 4) && (tname[c] != '\0'); c++)
    name_str[c] = tname[c];
  name = rtems_build_name(name_str[0],
                          name_str[1],
                          name_str[2],
                          name_str[3]);
  
  set_status_code(rtems_timer_create(name, &id));

  if (unsuccessful())
  {
    make_invalid();
  }
  
  return last_status_code();
}

const rtems_status_code rtemsTimer::destroy()
{
  if (id)
  {
    set_status_code(rtems_timer_delete(id));
    make_invalid();
  }
  else
    set_status_code(RTEMS_NOT_OWNER_OF_RESOURCE);
  
  return last_status_code();
}
  
void rtemsTimer::common_handler(rtems_id , void *user_data)
{
  rtemsTimer *timer = (rtemsTimer*) user_data;
  
  if (timer->repeat)
    timer->reset();

  timer->triggered();
}
