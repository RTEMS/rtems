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
#include <libcpu/mongoose-v.h>

#include "iregdef.h"
#include "idtcpu.h"


#define CALL_ISR(_vector,_frame) \
  do { \
    if ( _ISR_Vector_table[_vector] ) \
      (_ISR_Vector_table[_vector])(_vector,_frame); \
    else \
      mips_default_isr(_vector); \
  } while (0)

#include <bspIo.h>  /* for printk */






void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned32  sr, srmaskoff;
  unsigned32  cause, cshifted;
  unsigned32  bit;
  unsigned32  pf_icr;

  /* mips_get_sr( sr ); */
  sr = frame->regs[ R_SR ];

  mips_get_cause( cause );

  /* mask off everything other than the interrupt bits */
  cause &= SR_IMASK;

  /* mask off the pending interrupts in the status register */
  srmaskoff = sr & ~cause;
  mips_set_sr( srmaskoff );

  /* allow nesting for all non-pending interrupts */
  asm volatile( "rfe" );

  cshifted = (cause & (sr & SR_IMASK)) >> CAUSE_IPSHIFT;

  if ( cshifted & 0x04 )       /* IP[0] ==> INT0 == TIMER1 */
    CALL_ISR( MONGOOSEV_IRQ_TIMER1, frame );
    
  if ( cshifted & 0x08 )       /* IP[1] ==> INT1 == TIMER2*/
    CALL_ISR( MONGOOSEV_IRQ_TIMER2, frame );
    
  if ( cshifted & 0x10 )       /* IP[2] ==> INT2 */
    CALL_ISR( MONGOOSEV_IRQ_INT2, frame );
    
  if ( cshifted & 0x20 )       /* IP[3] ==> INT3 == FPU interrupt */
    CALL_ISR( MONGOOSEV_IRQ_INT3, frame );
    
  if ( cshifted & 0x40 )       /* IP[4] ==> INT4, external interrupt */
    CALL_ISR( MONGOOSEV_IRQ_INT4, frame );

  if ( cshifted & 0x80 )       /* IP[5] ==> INT5, peripheral interrupt */
  {
     pf_icr = MONGOOSEV_READ( MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_CAUSE_REGISTER );

     /* if !pf_icr */
     for ( bit=0 ; bit <= 31 ; bit++, pf_icr >>= 1 ) 
     {
	if ( pf_icr & 1 )
	{
	   CALL_ISR( MONGOOSEV_IRQ_PERIPHERAL_BASE + bit, frame );
	}
     } 
  }


  /* all the pending interrupts were serviced, now re-enable them */
  mips_get_sr( sr );

  /* we allow the 2 software interrupts to nest freely, under the
   * assumption that the program knows what its doing...  
   */

  if( cshifted & 0x3 )
  {
     sr |= (SR_IBIT1 | SR_IBIT1);
     cause &= ~(SR_IBIT1 | SR_IBIT1);

     mips_set_cause(cause);
     mips_set_sr(sr);

     if ( cshifted & 0x01 )       /* SW[0] */
     {
	CALL_ISR( MONGOOSEV_IRQ_SOFTWARE_1, frame );
     } 
     if ( cshifted & 0x02 )       /* SW[1] */
     {
	CALL_ISR( MONGOOSEV_IRQ_SOFTWARE_2, frame );
     }
  }

  sr |= cause;
  mips_set_sr( sr );
}







void mips_default_isr( int vector )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  printk( "Unhandled isr exception: vector 0x%02x, cause 0x%08X, sr 0x%08X\n", vector, cause, sr );
  rtems_fatal_error_occurred(1);
}








/* userspace routine to assert either software interrupt */

int assertSoftwareInterrupt( unsigned32 n )
{
   if( n >= 0 && n<2 )
   {
      unsigned32 c;

      mips_get_cause(c);
      c = ((n+1) << 8);
      mips_set_cause(c);

      return n;
   }
   else return -1;
}











/* exception vectoring, from vectorexceptions.c  */

/*#include <rtems.h>
#include <stdlib.h>
#include "iregdef.h"
#include <bsp.h>
#include <bspIo.h>*/




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


/* 
 *  this struct holds the set of registers we're going to dump on an
 *  exception, the symbols are defined by iregdef.h, and they are set
 *  by cpu_asm.S into the CPU_Interrupt_frame passed here by
 *  ISR_Handler.  Note not all registers are stored, only those used
 *  by the cpu_asm.S code.  Refer to cpu_asm.S 
 */


struct regdef dumpregs[]= { { R_RA, "R_RA" }, { R_V0, "R_V0" }, { R_V1, "R_V1" }, { R_A0, "R_A0" }, { R_A1,   "R_A1" }, { R_A2,   "R_A2" }, \
			    { R_A3, "R_A3" }, { R_T0, "R_T0" }, { R_T1, "R_T1" }, { R_T2, "R_T2" }, { R_T3,   "R_T3" }, { R_T4,   "R_T4" }, \
			    { R_T5, "R_T5" }, { R_T6, "R_T6" }, { R_T7, "R_T7" }, { R_T8, "R_T8" }, { R_MDLO, "R_MDLO" }, { R_MDHI, "R_MDHI" }, \
			    { R_GP, "R_GP" }, { R_FP, "R_FP" }, { R_AT, "R_AT" }, { R_EPC,"R_EPC"}, { -1, NULL } };



void mips_default_exception_code_handler( int exc, CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;
  int	i, j;

  mips_get_sr( sr );
  mips_get_cause( cause );

  printk( "Unhandled exception %d\n", exc );
  printk( "sr: 0x%08x  cause: 0x%08x --> %s\n", sr, cause, cause_strings[(cause >> 2) &0x1f] );

  for(i=0; dumpregs[i].offset > -1; i++)
  {
     printk("   %s", dumpregs[i].name);
     for(j=0; j< 7-strlen(dumpregs[i].name); j++) printk(" ");
     printk("  %08X\n", frame->regs[dumpregs[i].offset] );
  }
  
  rtems_fatal_error_occurred(1);
}







#define CALL_EXC(_vector,_frame) \
   do { \
	if( _ISR_Vector_table[_vector] ) \
	     (_ISR_Vector_table[_vector])(_vector,_frame); \
	  else \
	     mips_default_exception_code_handler( _vector, _frame ); \
   } while(0)





void mips_vector_exceptions( CPU_Interrupt_frame *frame )
{
  unsigned32 cause;
  unsigned32 exc;

  mips_get_cause( cause );
  exc = (cause >> 2) & 0x1f;

  if( exc == 4 )
     CALL_EXC( MONGOOSEV_EXCEPTION_ADEL, frame );

  else if( exc == 5 )
     CALL_EXC( MONGOOSEV_EXCEPTION_ADES, frame );

  else if( exc == 6 )
     CALL_EXC( MONGOOSEV_EXCEPTION_IBE, frame );

  else if( exc == 7 )
     CALL_EXC( MONGOOSEV_EXCEPTION_DBE, frame );

  else if( exc == 8 )
     CALL_EXC( MONGOOSEV_EXCEPTION_SYSCALL, frame );

  else if( exc == 9 )
     CALL_EXC( MONGOOSEV_EXCEPTION_BREAK, frame );

  else if( exc == 10 )
     CALL_EXC( MONGOOSEV_EXCEPTION_RI, frame );

  else if( exc == 11 )
     CALL_EXC( MONGOOSEV_EXCEPTION_CPU, frame );

  else if( exc == 12 )
     CALL_EXC( MONGOOSEV_EXCEPTION_OVERFLOW, frame );

  else
     mips_default_exception_code_handler( exc, frame );
}


// eof

