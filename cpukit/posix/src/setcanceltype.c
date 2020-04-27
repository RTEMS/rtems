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
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
