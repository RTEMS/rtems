/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
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
