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

#include <rtems++/rtemsStatusCode.h>

/* ----
    Status Code string table
*/

static const char *status_strings[RTEMS_STATUS_CODES_LAST + 1] =
{
  "RTEMS[00] successful completion",
  "RTEMS[01] task exitted, returned from a thread",
  "RTEMS[02] multiprocessing not configured",
  "RTEMS[03] invalid object name",
  "RTEMS[04] invalid object id",
  "RTEMS[05] too many",
  "RTEMS[06] timed out waiting",
  "RTEMS[07] object deleted while waiting",
  "RTEMS[08] specified size was invalid",
  "RTEMS[09] address specified is invalid",
  "RTEMS[10] number was invalid",
  "RTEMS[11] item has not been initialized",
  "RTEMS[12] resources still outstanding",
  "RTEMS[13] request not satisfied",
  "RTEMS[14] thread is in wrong state",
  "RTEMS[15] thread already in state",
  "RTEMS[16] illegal on calling thread",
  "RTEMS[17] illegal for remote object",
  "RTEMS[18] called from wrong environment",
  "RTEMS[19] invalid thread priority",
  "RTEMS[20] invalid date/time",
  "RTEMS[21] invalid node id",
  "RTEMS[22] directive not configured",
  "RTEMS[23] not owner of resource",
  "RTEMS[24] directive not implemented",
  "RTEMS[25] RTEMS inconsistency detected",
  "RTEMS[26] could not get enough memory"
};

/* ----
    StatusCode
*/

const char *rtemsStatusCode::last_status_string()
{
  return status_string(last_status);
}

const char *rtemsStatusCode::status_string(rtems_status_code status_code)
{
  // mapped from "rtems/rtems/status.h"
  if (status_code <= RTEMS_STATUS_CODES_LAST)
  {
    return status_strings[status_code];
  }

  return "unknown status code";
}

