/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMIPSHurricane
 *
 * @brief Global BSP definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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

#ifndef LIBBSP_MIPS_HURRICANE_BSP_H
#define LIBBSP_MIPS_HURRICANE_BSP_H

/**
 * @defgroup RTEMSBSPsMIPSHurricane Hurricane
 *
 * @ingroup RTEMSBSPsMIPS
 *
 * @brief Hurricane Board Support Package.
 *
 * @{
 */

#ifndef ASM

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <libcpu/rm5231.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void WriteDisplay( char * string );

extern uint32_t mips_get_timer( void );

#define BSP_FEATURE_IRQ_EXTENSION
#define BSP_SHARED_HANDLER_SUPPORT      1

#define CPU_CLOCK_RATE_MHZ     (200)
#define CLOCKS_PER_MICROSECOND ( CPU_CLOCK_RATE_MHZ ) /* equivalent to CPU clock speed in MHz */

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 *
 *  NOTE: This macro generates a warning like "integer constant out
 *        of range" which is safe to ignore.  In 64 bit mode, unsigned32
 *        types are actually 64 bits long so that comparisons between
 *        unsigned32 types and pointers are valid.  The warning is caused
 *        by code in the delay macro that is necessary for 64 bit mode.
 */

#define rtems_bsp_delay( microseconds ) \
  { \
     uint32_t _end_clock = \
          mips_get_timer() + microseconds * CLOCKS_PER_MICROSECOND; \
     _end_clock %= 0x100000000;  /* make sure result is 32 bits */ \
     \
     /* handle timer overflow, if necessary */ \
     while ( _end_clock < mips_get_timer() );  \
     \
     while ( _end_clock > mips_get_timer() ); \
  }

/* Constants */

#define RAM_START 0
#define RAM_END   0x100000

/*
 * Prototypes for methods called from .S for dependency tracking
 */
void init_tlb(void);
void resettlb(int i);

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

/** @} */

#endif	/* __HURRICANE_BSP_h */
