/*
 *  PowerPC CPU Dependent Source
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/exec/cpu/no_cpu/cpu.c:
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/context.h>
#include <rtems/score/thread.h>

/*
 *  These are for testing purposes.
 */
#undef Testing

#ifdef Testing
static unsigned32 msr;
#ifdef ppc403
static unsigned32 evpr;
static unsigned32 exier;
#endif
#endif

/*
 *  ppc_interrupt_level_to_msr
 *
 *  This routine converts a two bit interrupt level to an MSR bit map.
 */

const unsigned32 _CPU_msrs[4] =
  { PPC_MSR_0, PPC_MSR_1, PPC_MSR_2, PPC_MSR_3 };

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 */

static void ppc_spurious(int, CPU_Interrupt_frame *);

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
  proc_ptr handler = (proc_ptr)ppc_spurious;
  int i;
#if (PPC_ABI != PPC_ABI_POWEROPEN)
  register unsigned32 r2;
#if (PPC_ABI != PPC_ABI_GCC27)
  register unsigned32 r13;

  asm ("mr %0,13" : "=r" ((r13)) : "0" ((r13)));
  _CPU_IRQ_info.Default_r13 = r13;
#endif

  asm ("mr %0,2" : "=r" ((r2)) : "0" ((r2)));
  _CPU_IRQ_info.Default_r2 = r2;
#endif

  _CPU_IRQ_info.Nest_level = &_ISR_Nest_level;
  _CPU_IRQ_info.Disable_level = &_Thread_Dispatch_disable_level;
  _CPU_IRQ_info.Vector_table = _ISR_Vector_table;
#if (PPC_ABI == PPC_ABI_POWEROPEN)
  _CPU_IRQ_info.Dispatch_r2 = ((unsigned32 *)_Thread_Dispatch)[1];
#endif
  _CPU_IRQ_info.Switch_necessary = &_Context_Switch_necessary;
  _CPU_IRQ_info.Signal = &_ISR_Signals_to_thread_executing;

  i = (int)&_CPU_IRQ_info;
  asm volatile("mtspr 0x113, %0" : "=r" (i) : "0" (i)); /* SPRG 3 */

  i = PPC_MSR_INITIAL & ~PPC_MSR_DISABLE_MASK;
  asm volatile("mtspr 0x112, %0" : "=r" (i) : "0" (i)); /* SPRG 2 */

#ifdef Testing
  {
    unsigned32 tmp;

    asm volatile ("mfmsr %0" : "=r" (tmp));
    msr = tmp;
#ifdef ppc403
    asm volatile ("mfspr %0, 0x3d6" : "=r" (tmp)); /* EVPR */
    evpr = tmp;
    asm volatile ("mfdcr %0, 0x42" : "=r" (tmp)); /* EXIER */
    exier = tmp;
    asm volatile ("mtspr 0x3d6, %0" :: "r" (0)); /* EVPR */
#endif
  }
#endif

  if ( cpu_table->spurious_handler )
    handler = (proc_ptr)cpu_table->spurious_handler;

  for (i = 0; i < PPC_INTERRUPT_MAX;  i++)
    _ISR_Vector_table[i] = handler;

  _CPU_Table = *cpu_table;
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 *
 *  COMMENTS FROM Andrew Bray <andy@i-cubed.co.uk>:
 *
 *  The PowerPC puts its interrupt enable status in the MSR register
 *  which also contains things like endianness control.  To be more
 *  awkward, the layout varies from processor to processor.  This
 *  is why I adopted a table approach in my interrupt handling.
 *  Thus the inverse process is slow, because it requires a table
 *  search.
 *
 *  This could fail, and return 4 (an invalid level) if the MSR has been
 *  set to a value not in the table.  This is also quite an expensive
 *  operation - I do hope its not too common.
 *
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level, msr;
 
  asm volatile("mfmsr %0" : "=r" ((msr)));
 
  msr &= PPC_MSR_DISABLE_MASK;
 
  for (level = 0; level < 4; level++)
    if ((_CPU_msrs[level] & PPC_MSR_DISABLE_MASK) == msr)
      break;
 
  return level;
}

/*  _CPU_ISR_install_vector
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

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
   *old_handler = _ISR_Vector_table[ vector ];

   /*
    *  If the interrupt vector table is a table of pointer to isr entry
    *  points, then we need to install the appropriate RTEMS interrupt
    *  handler for this vector number.
    */

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ vector ] = new_handler ? (ISR_Handler_entry)new_handler :
       _CPU_Table.spurious_handler ? 
          (ISR_Handler_entry)_CPU_Table.spurious_handler :
          (ISR_Handler_entry)ppc_spurious;
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
#if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
  _CPU_IRQ_info.Stack = _CPU_Interrupt_stack_high - 56;
#else
  _CPU_IRQ_info.Stack = _CPU_Interrupt_stack_high - 8;
#endif
}

/* Handle a spurious interrupt */
static void ppc_spurious(int v, CPU_Interrupt_frame *i)
{
#if 0
    printf("Spurious interrupt on vector %d from %08.8x\n",
	   v, i->pc);
#endif
#ifdef ppc403
    if (v == PPC_IRQ_EXTERNAL)
	{
	    register int r = 0;

	    asm volatile("mtdcr 0x42, %0" : "=r" ((r)) : "0" ((r))); /* EXIER */
	}
    else if (v == PPC_IRQ_PIT)
	{
	    register int r = 0x08000000;

	    asm volatile("mtspr 0x3d8, %0" : "=r" ((r)) : "0" ((r))); /* TSR */
	}
    else if (v == PPC_IRQ_FIT)
	{
	    register int r = 0x04000000;

	    asm volatile("mtspr 0x3d8, %0" : "=r" ((r)) : "0" ((r))); /* TSR */
	}
#endif
}

void _CPU_Fatal_error(unsigned32 _error)
{
#ifdef Testing
  unsigned32 tmp;

  tmp = msr;
  asm volatile ("mtmsr %0" :: "r" (tmp));
#ifdef ppc403
  tmp = evpr;
  asm volatile ("mtspr 0x3d6, %0" :: "r" (tmp)); /* EVPR */
  tmp = exier;
  asm volatile ("mtdcr 0x42, %0" :: "r" (tmp)); /* EXIER */
#endif
#endif
  asm volatile ("mr 3, %0" : : "r" ((_error)));
  asm volatile ("tweq 5,5");
  asm volatile ("li 0,0; mtmsr 0");
  while (1) ;
}
