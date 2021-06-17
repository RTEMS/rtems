/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief This source file contains the implementation of
 *   rtems_interrupt_handler_iterate().
 */

/*
 * Copyright (C) 2017 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <bsp/irq-generic.h>

/**
 * @brief Iterates over all installed interrupt handler of a vector.
 *
 * @ingroup bsp_interrupt
 *
 * @return In addition to the standard status codes this function returns
 * RTEMS_INTERNAL_ERROR if the BSP interrupt support is not initialized.
 *
 * @see rtems_interrupt_handler_iterate().
 */
static rtems_status_code bsp_interrupt_handler_iterate(
  rtems_vector_number vector,
  rtems_interrupt_per_handler_routine routine,
  void *arg
)
{
  rtems_interrupt_entry *current = NULL;
  rtems_option options = 0;
  rtems_vector_number index = 0;

  /* Check parameters and system state */
  if (!bsp_interrupt_is_initialized()) {
    return RTEMS_INTERNAL_ERROR;
  } else if (!bsp_interrupt_is_valid_vector(vector)) {
    return RTEMS_INVALID_ID;
  } else if (rtems_interrupt_is_in_progress()) {
    return RTEMS_CALLED_FROM_ISR;
  }

  /* Lock */
  bsp_interrupt_lock();

  /* Interate */
  index = bsp_interrupt_handler_index(vector);
  current = &bsp_interrupt_handler_table [index];
  if (!bsp_interrupt_is_empty_handler_entry(current)) {
    do {
      options = bsp_interrupt_is_handler_unique(index) ?
        RTEMS_INTERRUPT_UNIQUE : RTEMS_INTERRUPT_SHARED;
      routine(arg, current->info, options, current->handler, current->arg);
      current = current->next;
    } while (current != NULL);
  }

  /* Unlock */
  bsp_interrupt_unlock();

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_interrupt_handler_iterate(
  rtems_vector_number vector,
  rtems_interrupt_per_handler_routine routine,
  void *arg
)
{
  return bsp_interrupt_handler_iterate(vector, routine, arg);
}
