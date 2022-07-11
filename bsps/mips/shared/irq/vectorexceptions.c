/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *  
 *  Common Code for Vectoring MIPS Exceptions
 *
 *  The actual decoding of the cause register and vector number assignment
 *  is CPU model specific.
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

#include <rtems.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>
#include <rtems/bspIo.h>
#include <bsp/irq-generic.h>

struct regdef
{
  int  offset;
  char *name;
};

static const struct regdef dumpregs[]= {
  { R_RA, "R_RA" }, { R_V0, "R_V0" },     { R_V1, "R_V1" },
  { R_A0, "R_A0" }, { R_A1, "R_A1" },     { R_A2, "R_A2" },
  { R_A3, "R_A3" }, { R_T0, "R_T0" },     { R_T1, "R_T1" },
  { R_T2, "R_T2" }, { R_T3, "R_T3" },     { R_T4, "R_T4" },
  { R_T5, "R_T5" }, { R_T6, "R_T6" },     { R_T7, "R_T7" },
  { R_T8, "R_T8" }, { R_MDLO, "R_MDLO" }, { R_MDHI, "R_MDHI" },
  { R_GP, "R_GP" }, { R_FP, "R_FP" },     { R_AT, "R_AT" },
  { R_EPC,"R_EPC"}, { -1, NULL }
};

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame )
{
  uint32_t *frame_u32;
  int   i, j;

  frame_u32 = (uint32_t *)frame;
  for(i=0; dumpregs[i].offset > -1; i++)
  {
     printk("   %s", dumpregs[i].name);
     for(j=0; j< 7-strlen(dumpregs[i].name); j++) printk(" ");
#if (__mips == 1 ) || (__mips == 32)
     printk("  %08" PRIu32 "%c",
            frame_u32[dumpregs[i].offset], (i%3) ? '\t' : '\n' );
#elif __mips == 3
     printk("  %08" PRIu32 "", frame_u32[2 * dumpregs[i].offset + 1] );
     printk("%08" PRIu32 "%c",
            frame_u32[2 * dumpregs[i].offset], (i%2) ? '\t' : '\n' );
#endif
  }
  printk( "\n" );
}

/*
 *  There are constants defined for these but they should basically
 *  all be close to the same set.
 */

void mips_vector_exceptions( CPU_Interrupt_frame *frame )
{
  uint32_t   cause;
  uint32_t   exc;

  mips_get_cause( cause );
  exc = (cause >> 2) & 0x1f;

  bsp_interrupt_handler_dispatch( exc );
}
