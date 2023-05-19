/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Sets the Cancelability Type of Calling Thread to value given in type 
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <pthread.h>
#include <errno.h>

#include <rtems/score/isr.h>
#include <rtems/score/threadimpl.h>

/*
 *  18.2.2 Setting Cancelability State, P1003.1c/Draft 10, p. 183
 */

int pthread_setcanceltype(
  int  type,
  int *oldtype
)
{
  Thread_Life_state set_life_state;
  Thread_Life_state previous_life_state;

  if ( _ISR_Is_in_progress() ) {
    return EPROTO;
  }

  if ( type == PTHREAD_CANCEL_DEFERRED ) {
    set_life_state = THREAD_LIFE_CHANGE_DEFERRED;
  } else if ( type == PTHREAD_CANCEL_ASYNCHRONOUS ) {
    set_life_state = 0;
  } else {
    return EINVAL;
  }

  previous_life_state = _Thread_Change_life(
    THREAD_LIFE_CHANGE_DEFERRED,
    set_life_state,
    0
  );

  if ( oldtype != NULL ) {
    if ( ( previous_life_state & THREAD_LIFE_CHANGE_DEFERRED ) != 0 ) {
      *oldtype = PTHREAD_CANCEL_DEFERRED;
    } else {
      *oldtype = PTHREAD_CANCEL_ASYNCHRONOUS;
    }
  }

  return 0;
}
