/*
 * Copyright (c) 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/assoc.h>
#include <rtems/score/statesimpl.h>

static const rtems_assoc_32_pair state_pairs[] = {
  { STATES_WAITING_FOR_MUTEX,              "MTX" },
  { STATES_WAITING_FOR_SEMAPHORE,          "SEM" },
  { STATES_WAITING_FOR_EVENT,              "EV" },
  { STATES_WAITING_FOR_SYSTEM_EVENT,       "SYSEV" },
  { STATES_WAITING_FOR_MESSAGE,            "MSG" },
  { STATES_WAITING_FOR_CONDITION_VARIABLE, "CV" },
  { STATES_WAITING_FOR_FUTEX,              "FTX" },
  { STATES_WAITING_FOR_BSD_WAKEUP,         "WK" },
  { STATES_WAITING_FOR_TIME,               "TIME" },
  { STATES_WAITING_FOR_PERIOD,             "PER" },
  { STATES_WAITING_FOR_SIGNAL,             "SIG" },
  { STATES_WAITING_FOR_BARRIER,            "BAR" },
  { STATES_WAITING_FOR_RWLOCK,             "RW" },
  { STATES_WAITING_FOR_JOIN_AT_EXIT,       "JATX" },
  { STATES_WAITING_FOR_JOIN,               "JOIN" },
  { STATES_SUSPENDED,                      "SUSP" },
  { STATES_WAITING_FOR_SEGMENT,            "SEG" },
  { STATES_LIFE_IS_CHANGING,               "LIFE" },
  { STATES_DEBUGGER,                       "DBG" },
  { STATES_INTERRUPTIBLE_BY_SIGNAL,        "IS" },
  { STATES_WAITING_FOR_RPC_REPLY,          "RPC" },
  { STATES_ZOMBIE,                         "ZOMBI" },
  { STATES_DORMANT,                        "DORM" }
};

size_t rtems_assoc_thread_states_to_string(
  uint32_t  states,
  char     *buffer,
  size_t    buffer_size
)
{
  return rtems_assoc_32_to_string(
    states,
    buffer,
    buffer_size,
    state_pairs,
    RTEMS_ARRAY_SIZE( state_pairs ),
    ":",
    "READY"
  );
}
