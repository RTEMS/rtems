/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief This source file contains the interrupt get attribute implementation.
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <dev/irq/arm-gic.h>
#include <bsp/irq-generic.h>

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  attributes->is_maskable = true;
  attributes->maybe_enable = true;
  attributes->maybe_disable = true;
  attributes->can_raise = true;

  if ( vector <= ARM_GIC_IRQ_SGI_LAST ) {
    /*
     * It is implementation-defined whether implemented SGIs are permanently
     * enabled, or can be enabled and disabled by writes to GICD_ISENABLER0 and
     * GICD_ICENABLER0.
     */
    attributes->can_raise_on = true;
    attributes->cleared_by_acknowledge = true;
    attributes->trigger_signal = RTEMS_INTERRUPT_NO_SIGNAL;
  } else {
    attributes->can_disable = true;
    attributes->can_clear = true;
    attributes->trigger_signal = RTEMS_INTERRUPT_UNSPECIFIED_SIGNAL;

    /*
     * Interrupt 67 affinity value presents as unimplemented in the
     * configuration of the GICv2 instance used in ZynqMP CPUs.
     */
    if ( vector > ARM_GIC_IRQ_PPI_LAST && vector != 67 ) {
      /* SPI */
      attributes->can_get_affinity = true;
      attributes->can_set_affinity = true;
    }
  }

  return RTEMS_SUCCESSFUL;
}
