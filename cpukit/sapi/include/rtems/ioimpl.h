/**
 * @file
 *
 * @brief Classic Input/Output Manager Implementation API
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_IOIMPL_H
#define _RTEMS_IOIMPL_H

#include <rtems/io.h>
#include <rtems/score/isrlock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const size_t _IO_Number_of_drivers;

extern rtems_driver_address_table _IO_Driver_address_table[];

extern bool _IO_All_drivers_initialized;

/**
 * @brief Initialization of all device drivers.
 *
 * Initializes all device drivers.
 */
void _IO_Initialize_all_drivers( void );

ISR_LOCK_DECLARE( extern, _IO_Driver_registration_lock )

RTEMS_INLINE_ROUTINE void _IO_Driver_registration_acquire(
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable_and_acquire(
    &_IO_Driver_registration_lock,
    lock_context
  );
}

RTEMS_INLINE_ROUTINE void _IO_Driver_registration_release(
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable(
    &_IO_Driver_registration_lock,
    lock_context
  );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_IOIMPL_H */
