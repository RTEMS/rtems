/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * @file
 * @ingroup powerpc_gen5200
 * @brief Implementations for interrupt mechanisms for Time Test 27
 */

/*
 * Copyright (C) 2005 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <bsp/fatal.h>
#include <bsp/irq.h>

/*
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 1

static void stub_rtems_irq_enable( const struct __rtems_irq_connect_data__ *i )
{
  (void) i;
}

static void stub_rtems_irq_disable( const struct __rtems_irq_connect_data__ *i )
{
  (void) i;
}

static int stub_rtems_irq_is_enabled(
  const struct __rtems_irq_connect_data__ *i
)
{
  (void) i;
  return 0;
}

static rtems_irq_connect_data clockIrqData = {
  BSP_DECREMENTER,
  0,
  0,
  stub_rtems_irq_enable,
  stub_rtems_irq_disable,
  stub_rtems_irq_is_enabled
};

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  clockIrqData.hdl = handler;
  if ( !BSP_install_rtems_irq_handler( &clockIrqData ) ) {
    printk( "Error installing clock interrupt handler!\n" );
    bsp_fatal( MPC5200_FATAL_TM27_IRQ_INSTALL );
  }
}

#define Cause_tm27_intr()                   \
  do {                                      \
    uint32_t _clicks = 8;                   \
    __asm__ volatile( "mtdec %0"            \
                      : "=r"(( _clicks ))   \
                      : "r"(( _clicks )) ); \
  } while ( 0 )

#define Clear_tm27_intr()                   \
  do {                                      \
    uint32_t _clicks = 0xffffffff;          \
    __asm__ volatile( "mtdec %0"            \
                      : "=r"(( _clicks ))   \
                      : "r"(( _clicks )) ); \
  } while ( 0 )

#define Lower_tm27_intr()              \
  do {                                 \
    uint32_t _msr = 0;                 \
    _ISR_Set_level( 0 );               \
    __asm__ volatile( "mfmsr %0 ;"     \
                      : "=r"( _msr )   \
                      : "r"( _msr ) ); \
    _msr |= 0x8002;                    \
    __asm__ volatile( "mtmsr %0 ;"     \
                      : "=r"( _msr )   \
                      : "r"( _msr ) ); \
  } while ( 0 )

#endif
