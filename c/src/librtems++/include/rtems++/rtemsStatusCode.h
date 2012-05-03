/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty

  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  rtemsStatusCode controls and manages status codes from the RTEMS kernel.

  ------------------------------------------------------------------------
*/

#if !defined(_rtemsStatusCode_h_)
#define _rtemsStatusCode_h_

#include <rtems.h>

/* ----
    rtemsStatusCode
*/

class rtemsStatusCode
{
public:

  rtemsStatusCode() { last_status = RTEMS_NOT_CONFIGURED; }

  const bool successful() { return last_status == RTEMS_SUCCESSFUL; }
  const bool unsuccessful() { return last_status != RTEMS_SUCCESSFUL; }

  // return the last status code
  const rtems_status_code last_status_code() { return last_status; }

  // return the last status as a string
  const char *last_status_string();

  const char *status_string(rtems_status_code status_code);

protected:
  const rtems_status_code set_status_code(const rtems_status_code status)
  { return (last_status = status); }

private:

  // public at the moment, this might change
  rtems_status_code last_status;
};

#endif  // _rtemsStatusCode_h_
