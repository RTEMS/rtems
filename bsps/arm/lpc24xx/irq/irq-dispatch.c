/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief LPC24XX interrupt support.
 */

/*
 * Copyright (C) 2008, 2012 embedded brains GmbH & Co. KG
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

#include <rtems/score/armv4.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/lpc24xx.h>

#ifdef ARM_MULTILIB_ARCH_V4

void bsp_interrupt_dispatch(void)
{
  /* Read current vector number */
  rtems_vector_number vector = VICVectAddr;

  /* Enable interrupts in program status register */
  uint32_t psr = _ARMV4_Status_irq_enable();

  /* Dispatch interrupt handlers */
  bsp_interrupt_handler_dispatch(vector);

  /* Restore program status register */
  _ARMV4_Status_restore(psr);

  /* Acknowledge interrupt */
  VICVectAddr = 0;
}

#endif /* ARM_MULTILIB_ARCH_V4 */
