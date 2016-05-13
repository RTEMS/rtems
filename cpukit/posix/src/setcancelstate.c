/**
 * @file
 *
 * @brief Setting Cancelability State
 * @ingroup POSIXAPI
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/score/isr.h>
#include <rtems/score/threadimpl.h>

/*
 *  18.2.2 Setting Cancelability State, P1003.1c/Draft 10, p. 183
 */

int pthread_setcancelstate(
  int  state,
  int *oldstate
)
{
  Thread_Life_state new_life_protection;
  Thread_Life_state previous_life_state;

  if ( _ISR_Is_in_progress() ) {
    return EPROTO;
  }

  if ( state == PTHREAD_CANCEL_DISABLE ) {
    new_life_protection = THREAD_LIFE_PROTECTED;
  } else if ( state == PTHREAD_CANCEL_ENABLE ) {
    new_life_protection = 0;
  } else {
    return EINVAL;
  }

  previous_life_state = _Thread_Set_life_protection( new_life_protection );

  if ( oldstate != NULL ) {
    if ( ( previous_life_state & THREAD_LIFE_PROTECTED ) != 0 ) {
      *oldstate = PTHREAD_CANCEL_DISABLE;
    } else {
      *oldstate = PTHREAD_CANCEL_ENABLE;
    }
  }

  return 0;
}
