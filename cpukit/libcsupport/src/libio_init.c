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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

RTEMS_SYSINIT_ITEM(
  rtems_libio_post_driver,
  RTEMS_SYSINIT_STD_FILE_DESCRIPTORS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
