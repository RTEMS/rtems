/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the implementation of
 *   directive bindings of the Interrupt Manager for languages other than
 *   C/C++.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
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

#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/intr.h>

#if !defined(RTEMS_SMP)

/*
 *  Undefine all of these is normally a macro and we want a real body in
 *  the library for other language bindings.
 */
#undef rtems_interrupt_disable
#undef rtems_interrupt_enable
#undef rtems_interrupt_flash

/*
 *  Prototype them to avoid warnings
 */
rtems_interrupt_level rtems_interrupt_disable( void );
void rtems_interrupt_enable( rtems_interrupt_level previous_level );
void rtems_interrupt_flash( rtems_interrupt_level previous_level );

/*
 *  Now define real bodies
 */
rtems_interrupt_level rtems_interrupt_disable( void )
{
  rtems_interrupt_level previous_level;

  _ISR_Local_disable( previous_level );

  return previous_level;
}

void rtems_interrupt_enable(
  rtems_interrupt_level previous_level
)
{
  _ISR_Local_enable( previous_level );
}

void rtems_interrupt_flash(
  rtems_interrupt_level previous_level
)
{
  _ISR_Local_flash( previous_level );
}

#endif /* RTEMS_SMP */

#undef rtems_interrupt_is_in_progress
bool rtems_interrupt_is_in_progress( void );

bool rtems_interrupt_is_in_progress( void )
{
  return _ISR_Is_in_progress();
}
