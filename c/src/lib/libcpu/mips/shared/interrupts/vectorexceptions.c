/*
 *  Common Code for Vectoring MIPS Exceptions
 *
 *  The actual decoding of the cause register and vector number assignment
 *  is CPU model specific.
 *
 *  $Id$
 */

#include <rtems.h>
#include <stdlib.h>
#include "iregdef.h"
#include "idtcpu.h"
#include <rtems/bspIo.h>

char *cause_strings[32] =
{
  /*  0 */ "Int",
  /*  1 */ "TLB Mods",
  /*  2 */ "TLB Load",
  /*  3 */ "TLB Store",
  /*  4 */ "Address Load",
  /*  5 */ "Address Store",
  /*  6 */ "Instruction Bus Error",
  /*  7 */ "Data Bus Error",
  /*  9 */ "Syscall",
  /* 10 */ "Breakpoint",
  /* 11 */ "Reserved Instruction",
  /* 12 */ "Coprocessor Unuseable",
  /* 13 */ "Overflow",
  /* 14 */ "Trap",
  /* 15 */ "Instruction Virtual Coherency Error",
  /* 16 */ "FP Exception",
  /* 17 */ "Reserved 17",
  /* 18 */ "Reserved 17",
  /* 19 */ "Reserved 17",
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

struct regdef
{
  int  offset;
  char *name;
};

struct regdef dumpregs[]= {
  { R_RA, "R_RA" }, { R_V0, "R_V0" },     { R_V1, "R_V1" },
  { R_A0, "R_A0" }, { R_A1, "R_A1" },     { R_A2, "R_A2" },
  { R_A3, "R_A3" }, { R_T0, "R_T0" },     { R_T1, "R_T1" },
  { R_T2, "R_T2" }, { R_T3, "R_T3" },     { R_T4, "R_T4" },
  { R_T5, "R_T5" }, { R_T6, "R_T6" },     { R_T7, "R_T7" },
  { R_T8, "R_T8" }, { R_MDLO, "R_MDLO" }, { R_MDHI, "R_MDHI" },
  { R_GP, "R_GP" }, { R_FP, "R_FP" },     { R_AT, "R_AT" },
  { R_EPC,"R_EPC"}, { -1, NULL }
};

void mips_dump_exception_frame( CPU_Interrupt_frame *frame )
{
  unsigned int *frame_u32;
  int   i, j;

  frame_u32 = (unsigned32 *)frame;
  for(i=0; dumpregs[i].offset > -1; i++)
  {
     printk("   %s", dumpregs[i].name);
     for(j=0; j< 7-strlen(dumpregs[i].name); j++) printk(" ");
     printk("  %08X%c", frame_u32[dumpregs[i].offset], (i%3) ? '\t' : '\n' );
  }
  printk( "\n" );
}

void mips_default_exception_code_handler( int exc, CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  printk( "Unhandled exception %d\n", exc );
  printk( "sr: 0x%08x  cause: 0x%08x --> %s\n", sr, cause,
     cause_strings[(cause >> 2) &0x1f] );
  mips_dump_exception_frame( frame );

  rtems_fatal_error_occurred(1);
}

#define CALL_EXC(_vector,_frame) \
   do { \
        if ( _ISR_Vector_table[_vector] ) \
             (_ISR_Vector_table[_vector])(_vector,_frame); \
          else \
             mips_default_exception_code_handler( _vector, _frame ); \
   } while(0)

/*
 *  There are constants defined for these but they should basically
 *  all be close to the same set.
 */

void mips_vector_exceptions( CPU_Interrupt_frame *frame )
{
  unsigned32 cause;
  unsigned32 exc;

  mips_get_cause( cause );
  exc = (cause >> 2) & 0x1f;

  CALL_EXC( exc, frame );
}

