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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
