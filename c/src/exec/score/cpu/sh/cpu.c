/*
 *  This file contains information pertaining to the Hitachi SH
 *  processor.
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *
 *  COPYRIGHT (c) 1998-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
 
#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/cpu.h>
#include <rtems/score/sh.h>

/* referenced in start.S */
extern proc_ptr vectab[] ;

proc_ptr vectab[256] ;

extern proc_ptr _Hardware_isr_Table[];

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 */


void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
  register unsigned32 level = 0;

  /*
   *  The thread_dispatch argument is the address of the entry point
   *  for the routine called at the end of an ISR once it has been
   *  decided a context switch is necessary.  On some compilation
   *  systems it is difficult to call a high-level language routine
   *  from assembly.  This allows us to trick these systems.
   *
   *  If you encounter this problem save the entry point in a CPU
   *  dependent variable.
   */

  _CPU_Thread_dispatch_pointer = thread_dispatch;

  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

  /* FP context initialization support goes here */
  /* FIXME: When not to use SH4_FPSCR_PR ? */
#ifdef __SH4__
  _CPU_Null_fp_context.fpscr = SH4_FPSCR_DN | SH4_FPSCR_RM | SH4_FPSCR_PR;
#endif
#ifdef __SH3E__
  /* FIXME: Wild guess :) */
  _CPU_Null_fp_context.fpscr = SH4_FPSCR_DN | SH4_FPSCR_RM;
#endif

  _CPU_Table = *cpu_table;

  /* enable interrupts */
  _CPU_ISR_Set_level( level);
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  /*
   *  This routine returns the current interrupt level.
   */

  register unsigned32 _mask ;
  
  sh_get_interrupt_level( _mask );
  
  return ( _mask);
}

/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 */
 
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  /*
   *  This is where we install the interrupt handler into the "raw" interrupt
   *  table used by the CPU to dispatch interrupt handlers.
   */
  volatile proc_ptr	*vbr ;

#if SH_PARANOID_ISR  
  unsigned32		level ;

  sh_disable_interrupts( level );
#endif    

  /* get vbr */
  asm ( "stc vbr,%0" : "=r" (vbr) );

  *old_handler = vbr[vector] ;
  vbr[vector]  = new_handler ;

#if SH_PARANOID_ISR
  sh_enable_interrupts( level );
#endif
}


/*PAGE
 *
 *  _CPU_ISR_install_vector
 *
 *  This kernel routine installs the RTEMS handler for the
 *  specified vector.
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    old_handler - former ISR for this vector number
 *    new_handler - replacement ISR for this vector number
 *
 *  Output parameters:  NONE
 *
 */

#if defined(sh1) || defined(sh2)
void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
   proc_ptr ignored ;
#if 0 
   if(( vector <= 113) && ( vector >= 11))
     {
#endif
       *old_handler = _ISR_Vector_table[ vector ];

       /*
	*  If the interrupt vector table is a table of pointer to isr entry
	*  points, then we need to install the appropriate RTEMS interrupt
	*  handler for this vector number.
	*/
       _CPU_ISR_install_raw_handler(vector, 
				    _Hardware_isr_Table[vector],
				    &ignored );

       /*
	*  We put the actual user ISR address in '_ISR_Vector_table'.  
	*  This will be used by __ISR_Handler so the user gets control.
	*/

       _ISR_Vector_table[ vector ] = new_handler;
#if 0
     }
#endif
}

/*PAGE
 *
 *  _CPU_Thread_Idle_body
 *
 *  NOTES:
 *
 *  1. This is the same as the regular CPU independent algorithm.
 *
 *  2. If you implement this using a "halt", "idle", or "shutdown"
 *     instruction, then don't forget to put it in an infinite loop.
 *
 *  3. Be warned. Some processors with onboard DMA have been known
 *     to stop the DMA if the CPU were put in IDLE mode.  This might
 *     also be a problem with other on-chip peripherals.  So use this
 *     hook with caution.
 */

#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)
void _CPU_Thread_Idle_body( void )
{

  for( ; ; )
    {
      asm volatile("nop");
    }
    /* insert your "halt" instruction here */ ;
}
#endif

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

unsigned8 _bit_set_table[16] = 
  { 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 1,0};


#endif

void _CPU_Context_Initialize(
  Context_Control	*_the_context,
  void			*_stack_base,
  unsigned32		_size,
  unsigned32		_isr,
  void 	(*_entry_point)(void),
  int			_is_fp )
{
  _the_context->r15 = (unsigned32*) ((unsigned32) (_stack_base) + (_size) );
#if defined(__sh1__) || defined(__sh2__)
  _the_context->sr  = (_isr << 4) & 0x00f0 ;
#else
  _the_context->sr  = SH4_SR_MD | ((_isr << 4) & 0x00f0);
#endif
  _the_context->pr  = (unsigned32*) _entry_point ;


#if 0 && SH_HAS_FPU
   /* Disable FPU if it is non-fp task */
  if(!_is_fp)
    _the_context->sr |= SH4_SR_FD;
#endif
}

