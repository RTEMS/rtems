/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief Intel I386 Interrupt Macros
 *
 * Formerly contained in and extracted from libcpu/i386/cpu.h
 *
 *  Applications must not include this file directly.
 */

/*
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
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

/**
 * @defgroup RTEMSScoreCPUi386Interrupt Processor Dependent Interrupt Management
 *
 * @ingroup RTEMSScoreCPUi386
 *
 * @brief i386 Interrupt Management
 */
/**@{**/

#ifndef _RTEMS_SCORE_INTERRUPTS_H
#define _RTEMS_SCORE_INTERRUPTS_H

#ifndef ASM

struct 	__rtems_raw_irq_connect_data__;

typedef void (*rtems_raw_irq_hdl)		(void);
typedef void (*rtems_raw_irq_enable)		(const struct __rtems_raw_irq_connect_data__*);
typedef void (*rtems_raw_irq_disable)		(const struct __rtems_raw_irq_connect_data__*);
typedef int  (*rtems_raw_irq_is_enabled)	(const struct __rtems_raw_irq_connect_data__*);

/**
 * @name Interrupt Level Macros
 * 
 */
/**@{**/
#if !defined(I386_DISABLE_INLINE_ISR_DISABLE_ENABLE)
#define i386_disable_interrupts( _level ) \
  { \
    __asm__ volatile ( "pushf ; \
                    cli ; \
                    pop %0" \
                   : "=rm" ((_level)) \
    ); \
  }

#define i386_enable_interrupts( _level )  \
  { \
    __asm__ volatile ( "push %0 ; \
                    popf" \
                    : : "rm" ((_level)) : "cc" \
    ); \
  }

#define i386_flash_interrupts( _level ) \
  { \
    __asm__ volatile ( "push %0 ; \
                    popf ; \
                    cli" \
                    : : "rm" ((_level)) : "cc" \
    ); \
  }

#define i386_get_interrupt_level( _level ) \
  do { \
    uint32_t   _eflags; \
    \
    __asm__ volatile ( "pushf ; \
                    pop %0" \
                    : "=rm" ((_eflags)) \
    ); \
    \
    _level = (_eflags & EFLAGS_INTR_ENABLE) ? 0 : 1; \
  } while (0)
#else
uint32_t i386_disable_interrupts( void );
void i386_enable_interrupts(uint32_t level);
void i386_flash_interrupts(uint32_t level);
void i386_set_interrupt_level(uint32_t new_level);
uint32_t i386_get_interrupt_level( void );
#endif /* PARAVIRT */

/** @} */

/**@}**/
#endif
#endif
