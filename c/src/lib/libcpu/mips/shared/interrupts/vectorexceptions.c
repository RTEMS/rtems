/*
 *  ISR Vectoring support for the Synova Mongoose-V.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <stdlib.h>

#define mips_get_cause( _cause ) \
  do { \
    asm volatile( "mfc0 %0, $13; nop" : "=r" (_cause) :  ); \
  } while (0)

#define CALL_ISR(_vector) \
  do { \
    if ( _ISR_Vector_table[_vector] ) \
      (_ISR_Vector_table[_vector])(_vector); \
    else \
      mips_default_exception_code_handler(_vector); \
  } while (0)

#include <bspIo.h>  /* for printk */

char *cause_strings[32] = {
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

void mips_default_exception_code_handler( int exc, CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_cause( cause );

  printk( "Unhandled exception cause %d\n", exc );
  printk( "sr: 0x%08x", sr );
  printk( "cause: 0x%08x --> %s", cause, cause_strings[(cause >> 2) &0x1f] );
  /* XXX dump frame once defined */
  rtems_fatal_error_occurred(1);
}

void mips_vector_exceptions( CPU_Interrupt_frame *frame )
{
  unsigned int cause;
  int          exc;

  mips_get_cause( cause );
  exc = (cause >> 2) &0x1f;

  printk( "Exception!\n" );
  mips_default_exception_code_handler( exc, frame );
}
