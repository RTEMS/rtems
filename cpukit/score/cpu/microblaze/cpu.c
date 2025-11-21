/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUMicroBlaze
 *
 * @brief MicroBlaze architecture support implementation
 */

/*
 * Copyright (c) 2015, Hesham Almatary
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#include <inttypes.h>

#include <rtems/bspIo.h>
#include <rtems/fatal.h>
#include <rtems/score/isr.h>
#include <rtems/score/tls.h>
#include <rtems/score/wkspace.h>

void _CPU_Initialize( void )
{
}

void _CPU_Context_Initialize(
  Context_Control *context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
)
{
  (void) new_level;
  (void) is_fp;

  uint32_t stack = (uint32_t) stack_area_begin;
  uint32_t stack_high = stack + stack_area_size;

  memset( context, 0, sizeof(*context) ) ;

  context->r1 = stack_high - 64;
  context->r15 = (uint32_t) entry_point;

  uint32_t msr;
  _CPU_MSR_GET( msr );
  context->rmsr = msr;

  if ( tls_area != NULL ) {
    context->thread_pointer = _TLS_Initialize_area( tls_area );
  }
}

typedef struct {
  int         id;
  const char *text;
} _CPU_esr_ec_code;

/* See ug984-vivado-microblaze-ref Exception Status Register (ESR) */
static _CPU_esr_ec_code esr_ec_codes[] = {
  { 0, "Stream Exception" },
  { 1, "Unaligned data access exception" },
  { 2, "Illegal op-code exception" },
  { 3, "Instruction bus error exception" },
  { 4, "Data bus error exception" },
  { 5, "Divide exception" },
  { 6, "Floating point unit exception" },
  /* Xilinx docs have 2 errors for id 7 */
  { 7, "Privileged instruction or Stack protection violation exception" },
  { 16, "Data storage exception" },
  { 17, "Instruction storage exception" },
  { 18, "Data TLB miss exception" },
  { 19, "Instruction TLB miss exception" } };

void _CPU_Exception_frame_print( const CPU_Exception_frame *ctx )
{

  printk( "\nR0   = 0x%08" PRIx32 " R17  = %p <- PC fault\n", 0, ctx->r17 );
  printk( "R1   = 0x%08" PRIx32 " R18  = 0x%08" PRIx32 "\n", ctx->r1, ctx->r18 );
  printk( "R2   = 0x%08" PRIx32 " R19  = 0x%08" PRIx32 "\n", ctx->r2, ctx->r19 );
  printk( "R3   = 0x%08" PRIx32 " R20  = 0x%08" PRIx32 "\n", ctx->r3, ctx->r20 );
  printk( "R4   = 0x%08" PRIx32 " R21  = 0x%08" PRIx32 "\n", ctx->r4, ctx->r21 );
  printk( "R5   = 0x%08" PRIx32 " R22  = 0x%08" PRIx32 "\n", ctx->r5, ctx->r22 );
  printk( "R6   = 0x%08" PRIx32 " R23  = 0x%08" PRIx32 "\n", ctx->r6, ctx->r23 );
  printk( "R7   = 0x%08" PRIx32 " R24  = 0x%08" PRIx32 "\n", ctx->r7, ctx->r24 );
  printk( "R8   = 0x%08" PRIx32 " R25  = 0x%08" PRIx32 "\n", ctx->r8, ctx->r25 );
  printk( "R9   = 0x%08" PRIx32 " R26  = 0x%08" PRIx32 "\n", ctx->r9, ctx->r26 );
  printk( "R10  = 0x%08" PRIx32 " R27  = 0x%08" PRIx32 "\n", ctx->r10, ctx->r27 );
  printk( "R11  = 0x%08" PRIx32 " R28  = 0x%08" PRIx32 "\n", ctx->r11, ctx->r28 );
  printk( "R12  = 0x%08" PRIx32 " R29  = 0x%08" PRIx32 "\n", ctx->r12, ctx->r29 );
  printk( "R13  = 0x%08" PRIx32 " R30  = 0x%08" PRIx32 "\n", ctx->r13, ctx->r30 );
  printk( "R14  = %p R31  = 0x%08" PRIx32 "\n", ctx->r14, ctx->r31 );
  printk( "R15  = %p ESR  = 0x%08" PRIx32 "\n", ctx->r15, ctx->esr );
  printk( "R16  = %p EAR  = %p\n", ctx->r16, ctx->ear );

  printk( "MSR  = 0x%08" PRIx32 " %s%s%s%s%s%s%s%s%s%s%s%s\n",
          ctx->msr,
          ( ctx->msr & MICROBLAZE_MSR_VM ) ? "VM " : "",
          ( ctx->msr & MICROBLAZE_MSR_UM ) ? "UM " : "",
          ( ctx->msr & MICROBLAZE_MSR_PVR ) ? "PVR " : "",
          ( ctx->msr & MICROBLAZE_MSR_EIP ) ? "EiP " : "",
          ( ctx->msr & MICROBLAZE_MSR_EE ) ? "EE " : "",
          ( ctx->msr & MICROBLAZE_MSR_DCE ) ? "DCE " : "",
          ( ctx->msr & MICROBLAZE_MSR_DZO ) ? "DZO " : "",
          ( ctx->msr & MICROBLAZE_MSR_ICE ) ? "ICE " : "",
          ( ctx->msr & MICROBLAZE_MSR_FSL ) ? "FSL " : "",
          ( ctx->msr & MICROBLAZE_MSR_BIP ) ? "BiP " : "",
          ( ctx->msr & MICROBLAZE_MSR_C ) ? "C " : "",
          ( ctx->msr & MICROBLAZE_MSR_IE ) ? "IE " : "" );

  const char *esr_ec_txt = "?";
  int         exception_ind = 0;
  int         esr_ec = ctx->esr & 0x1f;
  for ( ; exception_ind < sizeof( esr_ec_codes ) / sizeof( esr_ec_codes[ 0 ] );
        exception_ind++ ) {
    if ( esr_ec_codes[ exception_ind ].id == esr_ec ) {
      esr_ec_txt = esr_ec_codes[ exception_ind ].text;
    }
  }
  printk( "ESR: %s\n", esr_ec_txt );
  printk( "PC:  %p\n", ctx->r17 );
}

void _CPU_ISR_Set_level( uint32_t level )
{
  uint32_t microblaze_switch_reg;

  _CPU_MSR_GET( microblaze_switch_reg );

  if ( level == 0 ) {
    microblaze_switch_reg |= MICROBLAZE_MSR_IE;
  } else {
    microblaze_switch_reg &= ~( MICROBLAZE_MSR_IE );
  }

  _CPU_MSR_SET( microblaze_switch_reg );
}

uint32_t _CPU_ISR_Get_level( void )
{
  uint32_t level;

  _CPU_MSR_GET( level );

  /* This is unique. The MSR register contains an interrupt enable flag where
   * most other architectures have an interrupt disable flag. */
  return ( level & MICROBLAZE_MSR_IE ) == 0;
}

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
)
{
  *old_handler = _ISR_Vector_table[ vector ];
  _ISR_Vector_table[ vector ] = new_handler;
}

void *_CPU_Thread_Idle_body( uintptr_t ignored )
{
  (void) ignored;

  while ( true ) {
  }
}

MicroBlaze_Exception_handler installed_exception_handler = NULL;

void _MicroBlaze_Exception_install_handler(
  MicroBlaze_Exception_handler  new_handler,
  MicroBlaze_Exception_handler *old_handler
)
{
  if ( old_handler != NULL ) {
    *old_handler = installed_exception_handler;
  }

  installed_exception_handler = new_handler;
}

void _MicroBlaze_Exception_handle( CPU_Exception_frame *ef )
{
  /* EiP is not set for user exceptions which are unused and not hooked */
  if (
    ( ef->msr & MICROBLAZE_MSR_EIP ) != 0
    && installed_exception_handler != NULL
  ) {
    installed_exception_handler( ef );
  }

  rtems_fatal( RTEMS_FATAL_SOURCE_EXCEPTION, (rtems_fatal_code) ef );
}

MicroBlaze_Exception_handler installed_debug_handler = NULL;

void _MicroBlaze_Debug_install_handler(
  MicroBlaze_Exception_handler  new_handler,
  MicroBlaze_Exception_handler *old_handler
)
{
  if ( old_handler != NULL ) {
    *old_handler = installed_debug_handler;
  }

  installed_debug_handler = new_handler;
}

void _MicroBlaze_Debug_handle( CPU_Exception_frame *ef )
{
  /* BiP is not set for software debug events, set it here */
  ef->msr |= MICROBLAZE_MSR_BIP;

  if ( installed_debug_handler != NULL ) {
    installed_debug_handler( ef );
  }

  rtems_fatal( RTEMS_FATAL_SOURCE_EXCEPTION, (rtems_fatal_code) ef );
}
