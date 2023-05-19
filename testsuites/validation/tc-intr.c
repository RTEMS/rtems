/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrValIntr
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
#include <rtems/irq-extension.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrValIntr spec:/rtems/intr/val/intr
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests some @ref RTEMSAPIClassicIntr directives.
 *
 * This test case performs the following actions:
 *
 * - Validate rtems_interrupt_local_disable() and
 *   rtems_interrupt_local_enable().
 *
 *   - Check that maskable interrupts are enabled before the call to
 *     rtems_interrupt_local_disable() and disabled afterwards.
 *
 *   - Check that maskable interrupts are disabled before the call to
 *     rtems_interrupt_local_disable() and disabled afterwards.
 *
 *   - Check that the maskable interrupt status is restored by the call to
 *     rtems_interrupt_local_enable() according to the ``_isr_cookie``
 *     parameter.  In this case maskable interrupts are still disabled
 *     afterwards.
 *
 *   - Check that the maskable interrupt status is restored by the call to
 *     rtems_interrupt_local_enable() according to the ``_isr_cookie``
 *     parameter.  In this case maskable interrupts are enabled afterwards.
 *
 * - Validate the interrupt lock directives.
 *
 *   - Check that maskable interrupts are disabled before the call to
 *     rtems_interrupt_lock_interrupt_disable() and disabled afterwards.
 *
 *   - Check that the maskable interrupt status is not changed by the
 *     rtems_interrupt_lock_acquire_isr() call.
 *
 *   - Check that the maskable interrupt status is restored by the call to
 *     rtems_interrupt_lock_release() according to the ``_lock_context``
 *     parameter.
 *
 *   - Check that maskable interrupts are disabled before the call to
 *     rtems_interrupt_lock_acquire() and disabled afterwards.
 *
 *   - Check that the maskable interrupt status is restored by the call to
 *     rtems_interrupt_lock_release() according to the ``_lock_context``
 *     parameter.
 *
 *   - Check that the maskable interrupt status is not changed by the
 *     rtems_interrupt_lock_destroy() call.
 *
 * - Validate the interrupt entry initialization.
 *
 *   - Check that the entry is properly initialized by
 *     RTEMS_INTERRUPT_ENTRY_INITIALIZER().
 *
 *   - Call rtems_interrupt_entry_initialize().  Check that the entry is
 *     properly initialized by rtems_interrupt_entry_initialize().
 *
 * @{
 */

static void EntryRoutine( void *arg )
{
  (void) arg;
}

static void EntryRoutine2( void *arg )
{
  (void) arg;
}

/**
 * @brief Validate rtems_interrupt_local_disable() and
 *   rtems_interrupt_local_enable().
 */
static void RtemsIntrValIntr_Action_0( void )
{
  rtems_interrupt_level level;
  rtems_interrupt_level level_2;

  /*
   * Check that maskable interrupts are enabled before the call to
   * rtems_interrupt_local_disable() and disabled afterwards.
   */
  T_true( AreInterruptsEnabled() );
  rtems_interrupt_local_disable( level );
  T_false( AreInterruptsEnabled() );

  /*
   * Check that maskable interrupts are disabled before the call to
   * rtems_interrupt_local_disable() and disabled afterwards.
   */
  T_false( AreInterruptsEnabled() );
  rtems_interrupt_local_disable( level_2 );
  T_false( AreInterruptsEnabled() );

  /*
   * Check that the maskable interrupt status is restored by the call to
   * rtems_interrupt_local_enable() according to the ``_isr_cookie`` parameter.
   * In this case maskable interrupts are still disabled afterwards.
   */
  T_false( AreInterruptsEnabled() );
  rtems_interrupt_local_enable( level_2 );
  T_false( AreInterruptsEnabled() );

  /*
   * Check that the maskable interrupt status is restored by the call to
   * rtems_interrupt_local_enable() according to the ``_isr_cookie`` parameter.
   * In this case maskable interrupts are enabled afterwards.
   */
  T_false( AreInterruptsEnabled() );
  rtems_interrupt_local_enable( level );
  T_true( AreInterruptsEnabled() );
}

/**
 * @brief Validate the interrupt lock directives.
 */
static void RtemsIntrValIntr_Action_1( void )
{
  RTEMS_INTERRUPT_LOCK_DEFINE( , lock, "name" );
  rtems_interrupt_lock_context lock_context;

  /*
   * Check that maskable interrupts are disabled before the call to
   * rtems_interrupt_lock_interrupt_disable() and disabled afterwards.
   */
  T_true( AreInterruptsEnabled() );
  rtems_interrupt_lock_interrupt_disable( &lock_context );
  T_false( AreInterruptsEnabled() );

  /*
   * Check that the maskable interrupt status is not changed by the
   * rtems_interrupt_lock_acquire_isr() call.
   */
  T_false( AreInterruptsEnabled() );
  rtems_interrupt_lock_acquire_isr( &lock, &lock_context );
  T_false( AreInterruptsEnabled() );

  /*
   * Check that the maskable interrupt status is restored by the call to
   * rtems_interrupt_lock_release() according to the ``_lock_context``
   * parameter.
   */
  T_false( AreInterruptsEnabled() );
  rtems_interrupt_lock_release( &lock, &lock_context );
  T_true( AreInterruptsEnabled() );

  /*
   * Check that maskable interrupts are disabled before the call to
   * rtems_interrupt_lock_acquire() and disabled afterwards.
   */
  T_true( AreInterruptsEnabled() );
  rtems_interrupt_lock_acquire( &lock, &lock_context );
  T_false( AreInterruptsEnabled() );

  /*
   * Check that the maskable interrupt status is restored by the call to
   * rtems_interrupt_lock_release() according to the ``_lock_context``
   * parameter.
   */
  T_false( AreInterruptsEnabled() );
  rtems_interrupt_lock_release( &lock, &lock_context );
  T_true( AreInterruptsEnabled() );

  /*
   * Check that the maskable interrupt status is not changed by the
   * rtems_interrupt_lock_destroy() call.
   */
  T_true( AreInterruptsEnabled() );
  rtems_interrupt_lock_destroy( &lock );
  T_true( AreInterruptsEnabled() );
}

/**
 * @brief Validate the interrupt entry initialization.
 */
static void RtemsIntrValIntr_Action_2( void )
{
  int entry_arg;
  int entry_arg_2;
  const char entry_info[] = "1";
  const char entry_info_2[] = "1";
  rtems_interrupt_entry entry = RTEMS_INTERRUPT_ENTRY_INITIALIZER(
    EntryRoutine,
    &entry_arg,
    entry_info
  );

  /*
   * Check that the entry is properly initialized by
   * RTEMS_INTERRUPT_ENTRY_INITIALIZER().
   */
  T_eq_ptr( entry.handler, EntryRoutine );
  T_eq_ptr( entry.arg, &entry_arg );
  T_eq_ptr( entry.next, NULL );
  T_eq_ptr( entry.info, entry_info );

  /*
   * Call rtems_interrupt_entry_initialize().  Check that the entry is properly
   * initialized by rtems_interrupt_entry_initialize().
   */
  entry.next = &entry;
  rtems_interrupt_entry_initialize(
    &entry,
    EntryRoutine2,
    &entry_arg_2,
    entry_info_2
  );
  T_eq_ptr( entry.handler, EntryRoutine2 );
  T_eq_ptr( entry.arg, &entry_arg_2 );
  T_eq_ptr( entry.next, NULL );
  T_eq_ptr( entry.info, entry_info_2 );
}

/**
 * @fn void T_case_body_RtemsIntrValIntr( void )
 */
T_TEST_CASE( RtemsIntrValIntr )
{
  RtemsIntrValIntr_Action_0();
  RtemsIntrValIntr_Action_1();
  RtemsIntrValIntr_Action_2();
}

/** @} */
