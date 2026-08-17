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

static const char *const cause_strings[32] = {
  /*  0 */ "Int",
  /*  1 */ "TLB Mods",
  /*  2 */ "TLB Load",
  /*  3 */ "TLB Store",
  /*  4 */ "Address Load",
  /*  5 */ "Address Store",
  /*  6 */ "Instruction Bus Error",
  /*  7 */ "Data Bus Error",
  /*  8 */ "Syscall",
  /*  9 */ "Breakpoint",
  /* 10 */ "Reserved Instruction",
  /* 11 */ "Coprocessor Unuseable",
  /* 12 */ "Overflow",
  /* 13 */ "Trap",
  /* 14 */ "Instruction Virtual Coherency Error",
  /* 15 */ "FP Exception",
  /* 16 */ "Reserved 16",
  /* 17 */ "Reserved 17",
  /* 18 */ "Reserved 18",
  /* 19 */ "Reserved 19",
  /* 20 */ "Reserved 20",
  /* 21 */ "Reserved 21",
  /* 22 */ "Reserved 22",
  /* 23 */ "Watch",
  /* 24 */ "Reserved 24",
  /* 25 */ "Reserved 25",
  /* 26 */ "Reserved 26",
  /* 27 */ "Reserved 27",
  /* 28 */ "Reserved 28",
  /* 29 */ "Reserved 29",
  /* 30 */ "Reserved 30",
  /* 31 */ "Data Virtual Coherency Error"
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
  int       i;
  size_t    j;

  printk(
    "   CAUSE   %08" PRIx32 " --> %s\n",
    (uint32_t) frame->cause,
    cause_strings[ ( (uint32_t) frame->cause >> 2 ) & 0x1f ]
  );

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

/*
 * The frame of the exception being vectored, or NULL outside such a vector.
 * bsp_interrupt_handler_default() uses it to tell an exception which no
 * handler consumed from a spurious interrupt.
 */
CPU_Exception_frame *mips_exception_frame;

void mips_vector_exceptions( CPU_Interrupt_frame *frame )
{
  (void) frame;

  uint32_t   cause;
  uint32_t   exc;

  mips_get_cause( cause );
  exc = (cause >> 2) & 0x1f;

  mips_exception_frame = frame;
  bsp_interrupt_handler_dispatch( exc );
  mips_exception_frame = NULL;
}
