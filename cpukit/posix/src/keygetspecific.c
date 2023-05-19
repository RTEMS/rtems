/* SPDX-License-Identifier: BSD-2-Clause */

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
