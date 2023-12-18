/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of
 *   HasInterruptVectorEntriesInstalled().
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tx-support.h"

#include <rtems/irq-extension.h>
#include <rtems/test.h>

#include <bsp/irq-generic.h>

static bool HasRequiredAttributes(
  const rtems_interrupt_attributes *required,
  const rtems_interrupt_attributes *actual
)
{
  if ( required == NULL ) {
    return true;
  }

  if ( required->can_get_affinity && !actual->can_get_affinity ) {
    return false;
  }

  if ( required->can_raise && !actual->can_raise ) {
    return false;
  }

  if ( required->can_raise_on && !actual->can_raise_on ) {
    return false;
  }

  return true;
}

rtems_vector_number GetValidInterruptVectorNumber(
  const rtems_interrupt_attributes *required
)
{
  rtems_vector_number vector;

  for ( vector = 0; vector < BSP_INTERRUPT_VECTOR_COUNT; ++vector ) {
    rtems_status_code          sc;
    rtems_interrupt_attributes attr;

    sc = rtems_interrupt_get_attributes( vector, &attr );

    if ( sc == RTEMS_SUCCESSFUL && HasRequiredAttributes( required, &attr ) ) {
      break;
    }
  }

  return vector;
}

rtems_vector_number GetTestableInterruptVector(
  const rtems_interrupt_attributes *required
)
{
  rtems_vector_number vector;

  for ( vector = 0; vector < BSP_INTERRUPT_VECTOR_COUNT; ++vector ) {
    rtems_status_code          sc;
    rtems_interrupt_attributes attr;

    sc = rtems_interrupt_get_attributes( vector, &attr );

    if ( sc != RTEMS_SUCCESSFUL ) {
      continue;
    }

    if ( !attr.is_maskable ) {
      continue;
    }

    if ( !HasRequiredAttributes( required, &attr ) ) {
      continue;
    }

    if ( HasInterruptVectorEntriesInstalled( vector ) ) {
      continue;
    }

    if ( attr.can_enable && attr.can_disable ) {
      break;
    }

    if (
      attr.maybe_enable && attr.maybe_disable &&
      !attr.can_be_triggered_by_message &&
      attr.trigger_signal == RTEMS_INTERRUPT_NO_SIGNAL
    ) {
      rtems_status_code sc;
      bool              enabled;

      (void) rtems_interrupt_vector_enable( vector );
      sc = rtems_interrupt_vector_is_enabled( vector, &enabled );

      if ( sc == RTEMS_SUCCESSFUL && enabled ) {
        (void) rtems_interrupt_vector_disable( vector );
        break;
      }
    }
  }

  if ( vector == BSP_INTERRUPT_VECTOR_COUNT ) {
    vector = GetSoftwareInterruptVector();
  }

  return vector;
}

static void HasInstalled(
  void                   *arg,
  const char             *info,
  rtems_option            options,
  rtems_interrupt_handler handler_routine,
  void                   *handler_arg
)
{
  bool *has_installed_entries;

  (void) info;
  (void) options;
  (void) handler_routine;
  (void) handler_arg;

  has_installed_entries = arg;
  *has_installed_entries = true;
}

bool HasInterruptVectorEntriesInstalled( rtems_vector_number vector )
{
  bool              has_installed_entries;
  rtems_status_code sc;

  has_installed_entries = false;
  sc = rtems_interrupt_handler_iterate(
    vector,
    HasInstalled,
    &has_installed_entries
  );
  T_quiet_rsc_success( sc );

  return has_installed_entries;
}
