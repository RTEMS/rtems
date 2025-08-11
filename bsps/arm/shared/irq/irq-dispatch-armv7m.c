/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2011, 2012 Sebastian Huber
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

#include <rtems/score/armv7m.h>

#include <bsp/irq-generic.h>
#include <bsp/armv7m-irq.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void _ARMV7M_NVIC_Interrupt_dispatch(void)
{
  rtems_vector_number vector =
    ARMV7M_SCB_ICSR_VECTACTIVE_GET(_ARMV7M_SCB->icsr);

  _ARMV7M_Interrupt_service_enter();
  bsp_interrupt_handler_dispatch(ARMV7M_IRQ_OF_VECTOR(vector));
  _ARMV7M_Interrupt_service_leave();
}

#endif /* ARM_MULTILIB_ARCH_V7M */
