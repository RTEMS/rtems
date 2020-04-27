/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Thread-Specific Data Management
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * COPYRIGHT (c) 1989-2007.
 * On-Line Applications Research Corporation (OAR).
 * Copyright (c) 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/keyimpl.h>

/*
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

void *pthread_getspecific(
  pthread_key_t  key
)
{
  Thread_Control            *executing;
  ISR_lock_Context           lock_context;
  POSIX_Keys_Key_value_pair *key_value_pair;
  void                      *value;

  executing = _Thread_Get_executing();
  _POSIX_Keys_Key_value_acquire( executing, &lock_context );

  key_value_pair = _POSIX_Keys_Key_value_find( key, executing );

  if ( key_value_pair != NULL ) {
    value = key_value_pair->value;
  } else {
    value = NULL;
  }

  _POSIX_Keys_Key_value_release( executing, &lock_context );

  return value;
}
