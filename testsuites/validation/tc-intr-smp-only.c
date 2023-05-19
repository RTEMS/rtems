/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrValIntrSmpOnly
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrValIntrSmpOnly spec:/rtems/intr/val/intr-smp-only
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests some @ref RTEMSAPIClassicIntr directives.
 *
 * This test case performs the following actions:
 *
 * - Validate the interrupt lock directives.
 *
 *   - Check that RTEMS_INTERRUPT_LOCK_REFERENCE() expanded to a lock reference
 *     definition.  Check that the lock is available after static
 *     initialization.
 *
 *   - Check that the lock is available after initialization.
 *
 *   - Check that maskable interrupts are disabled before the call to
 *     rtems_interrupt_lock_interrupt_disable() and disabled afterwards.
 *
 *   - Check that the maskable interrupt status is not changed by the
 *     rtems_interrupt_lock_acquire_isr() call.  Check that the lock is no
 *     longer available.
 *
 *   - Check that the maskable interrupt status is restored by the call to
 *     rtems_interrupt_lock_release() according to the ``_lock_context``
 *     parameter.  Check that the lock is available afterwards.
 *
 *   - Check that the maskable interrupt status is not changed by the
 *     rtems_interrupt_lock_destroy() call.
 *
 *   - Initialize the lock using rtems_interrupt_lock_initialize().  Check that
 *     the lock is available after initialization.
 *
 *   - Check that maskable interrupts are disabled before the call to
 *     rtems_interrupt_lock_acquire() and disabled afterwards.  Check that the
 *     lock is no longer available.
 *
 *   - Check that the maskable interrupt status is restored by the call to
 *     rtems_interrupt_lock_release() according to the ``_lock_context``
 *     parameter.  Check that the lock is available afterwards.
 *
 *   - Check that the maskable interrupt status is not changed by the
 *     rtems_interrupt_lock_destroy() call.
 *
 * @{
 */

RTEMS_INTERRUPT_LOCK_DECLARE( static, the_lock )
RTEMS_INTERRUPT_LOCK_DEFINE( static, the_lock, "name " )

/**
 * @brief Validate the interrupt lock directives.
 */
static void RtemsIntrValIntrSmpOnly_Action_0( void )
{
  struct {
    int a;
    RTEMS_INTERRUPT_LOCK_MEMBER( member )
    int b;
  } lock = {
    .member = RTEMS_INTERRUPT_LOCK_INITIALIZER( "name" )
  };

  RTEMS_INTERRUPT_LOCK_REFERENCE( ref, &the_lock )
  rtems_interrupt_lock_context lock_context;

  /*
   * Check that RTEMS_INTERRUPT_LOCK_REFERENCE() expanded to a lock reference
   * definition.  Check that the lock is available after static initialization.
   */
  T_true( ISRLockIsAvailable( ref ) );

  /*
   * Check that the lock is available after initialization.
   */
  T_true( ISRLockIsAvailable( &lock.member ) );

  /*
   * Check that maskable interrupts are disabled before the call to
   * rtems_interrupt_lock_interrupt_disable() and disabled afterwards.
   */
  T_true( AreInterruptsEnabled() );
  T_true( ISRLockIsAvailable( &lock.member ) );
  rtems_interrupt_lock_interrupt_disable( &lock_context );
  T_false( AreInterruptsEnabled() );
  T_true( ISRLockIsAvailable( &lock.member ) );

  /*
   * Check that the maskable interrupt status is not changed by the
   * rtems_interrupt_lock_acquire_isr() call.  Check that the lock is no longer
   * available.
   */
  T_false( AreInterruptsEnabled() );
  T_true( ISRLockIsAvailable( &lock.member ) );
  rtems_interrupt_lock_acquire_isr( &lock.member, &lock_context );
  T_false( AreInterruptsEnabled() );
  T_false( ISRLockIsAvailable( &lock.member ) );

  /*
   * Check that the maskable interrupt status is restored by the call to
   * rtems_interrupt_lock_release() according to the ``_lock_context``
   * parameter.  Check that the lock is available afterwards.
   */
  T_false( AreInterruptsEnabled() );
  T_false( ISRLockIsAvailable( &lock.member ) );
  rtems_interrupt_lock_release( &lock.member, &lock_context );
  T_true( AreInterruptsEnabled() );
  T_true( ISRLockIsAvailable( &lock.member ) );

  /*
   * Check that the maskable interrupt status is not changed by the
   * rtems_interrupt_lock_destroy() call.
   */
  T_true( AreInterruptsEnabled() );
  rtems_interrupt_lock_destroy( &lock.member );
  T_true( AreInterruptsEnabled() );

  /*
   * Initialize the lock using rtems_interrupt_lock_initialize().  Check that
   * the lock is available after initialization.
   */
  rtems_interrupt_lock_initialize( &lock.member, "name" );
  T_true( ISRLockIsAvailable( &lock.member ) );

  /*
   * Check that maskable interrupts are disabled before the call to
   * rtems_interrupt_lock_acquire() and disabled afterwards.  Check that the
   * lock is no longer available.
   */
  T_true( AreInterruptsEnabled() );
  T_true( ISRLockIsAvailable( &lock.member ) );
  rtems_interrupt_lock_acquire( &lock.member, &lock_context );
  T_false( AreInterruptsEnabled() );
  T_false( ISRLockIsAvailable( &lock.member ) );

  /*
   * Check that the maskable interrupt status is restored by the call to
   * rtems_interrupt_lock_release() according to the ``_lock_context``
   * parameter.  Check that the lock is available afterwards.
   */
  T_false( AreInterruptsEnabled() );
  T_false( ISRLockIsAvailable( &lock.member ) );
  rtems_interrupt_lock_release( &lock.member, &lock_context );
  T_true( AreInterruptsEnabled() );
  T_true( ISRLockIsAvailable( &lock.member ) );

  /*
   * Check that the maskable interrupt status is not changed by the
   * rtems_interrupt_lock_destroy() call.
   */
  T_true( AreInterruptsEnabled() );
  rtems_interrupt_lock_destroy( &lock.member );
  T_true( AreInterruptsEnabled() );
}

/**
 * @fn void T_case_body_RtemsIntrValIntrSmpOnly( void )
 */
T_TEST_CASE( RtemsIntrValIntrSmpOnly )
{
  RtemsIntrValIntrSmpOnly_Action_0();
}

/** @} */
