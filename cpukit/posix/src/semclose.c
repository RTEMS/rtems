/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIX_SEMAPHORE
 *
 * @brief Close a Named Semaphore
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/posix/semaphoreimpl.h>

int sem_close( sem_t *sem )
{
  POSIX_Semaphore_Control *the_semaphore;
  uint32_t                 open_count;

  POSIX_SEMAPHORE_VALIDATE_OBJECT( sem );

  if ( !_POSIX_Semaphore_Is_named( sem ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  the_semaphore = _POSIX_Semaphore_Get( sem );

  _Objects_Allocator_lock();

  open_count = the_semaphore->open_count;

  if ( open_count == 0 ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( open_count == 1 && _POSIX_Semaphore_Is_busy( sem ) ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( EBUSY );
  }

  the_semaphore->open_count = open_count - 1;
  _POSIX_Semaphore_Delete( the_semaphore );
  _Objects_Allocator_unlock();
  return 0;
}
