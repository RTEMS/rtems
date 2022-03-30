/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief RTEMS LibIO Initialization
 *  @ingroup LibIO
 */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <rtems/libio_.h>

#include <pthread.h>

#include <rtems/sysinit.h>
#include <rtems/score/apimutex.h>

/*
 *  File descriptor Table Information
 */

static API_Mutex_Control rtems_libio_mutex = API_MUTEX_INITIALIZER( "_Libio" );

void rtems_libio_lock( void )
{
  _API_Mutex_Lock( &rtems_libio_mutex );
}

void rtems_libio_unlock( void )
{
  _API_Mutex_Unlock( &rtems_libio_mutex );
}

void *rtems_libio_iop_free_head;

void **rtems_libio_iop_free_tail = &rtems_libio_iop_free_head;

static void rtems_libio_init( void )
{
    uint32_t i;
    rtems_libio_t *iop;

    if (rtems_libio_number_iops > 0)
    {
        iop = rtems_libio_iop_free_head = &rtems_libio_iops[0];
        for (i = 0 ; (i + 1) < rtems_libio_number_iops ; i++, iop++)
          iop->data1 = iop + 1;
        iop->data1 = NULL;
        rtems_libio_iop_free_tail = &iop->data1;
    }
}

RTEMS_SYSINIT_ITEM(
  rtems_libio_init,
  RTEMS_SYSINIT_LIBIO,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
