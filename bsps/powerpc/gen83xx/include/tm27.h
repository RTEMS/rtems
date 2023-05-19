/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Support file for Timer Test 27.
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_TMTEST27
  #error "This is an RTEMS internal file you must not include directly."
#endif /* _RTEMS_TMTEST27 */

#ifndef TMTESTS_TM27_H
#define TMTESTS_TM27_H

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#define MUST_WAIT_FOR_INTERRUPT 1

static rtems_interrupt_handler tm27_interrupt_handler;

static int tm27_exception_handler( BSP_Exception_frame *frame, unsigned number)
{
	(*tm27_interrupt_handler)( NULL);

	return 0;
}

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  int rv = 0;

  tm27_interrupt_handler = handler;

  rv = ppc_exc_set_handler( ASM_DEC_VECTOR, tm27_exception_handler);
  if (rv < 0) {
    printk( "Error installing clock interrupt handler!\n");
  }
}

#define Cause_tm27_intr() \
  ppc_set_decrementer_register( 8)

#define Clear_tm27_intr() \
  ppc_set_decrementer_register( UINT32_MAX)

#define Lower_tm27_intr() \
  (void) ppc_external_exceptions_enable()

#endif /* TMTESTS_TM27_H */
