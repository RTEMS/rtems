/*
 *  Mips CPU Dependent Source
 *
 *  2002:       Greg Menke (gregory.menke@gsfc.nasa.gov)
 *      Overhauled interrupt level and interrupt enable/disable code
 *      to more exactly support MIPS.  Our mods were for MIPS1 processors
 *      MIPS3 ports are affected, though apps written to the old behavior
 *      should still work OK.
 *
 *  Conversion to MIPS port by Alan Cudmore <alanc@linuxstart.com> and
 *           Joel Sherrill <joel@OARcorp.com>. 
 *
 *    These changes made the code conditional on standard cpp predefines,
 *    merged the mips1 and mips3 code sequences as much as possible,
 *    and moved some of the assembly code to C.  Alan did much of the
 *    initial analysis and rework.  Joel took over from there and
 *    wrote the JMR3904 BSP so this could be tested.  Joel also
 *    added the new interrupt vectoring support in libcpu and
 *    tried to better support the various interrupt controllers.
 *
 *  Original MIP64ORION port by Craig Lebakken <craigl@transition.com>
 *           COPYRIGHT (c) 1996 by Transition Networks Inc.
 *
 *         To anyone who acknowledges that this file is provided "AS IS"
 *         without any express or implied warranty:
 *             permission to use, copy, modify, and distribute this file
 *             for any purpose is hereby granted without fee, provided that
 *             the above copyright notice and this notice appears in all
 *             copies, and that the name of Transition Networks not be used in
 *             advertising or publicity pertaining to distribution of the
 *             software without specific, written prior permission.
 *             Transition Networks makes no representations about the 
 *             suitability of this software for any purpose.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>




/* 
** Exception stack frame pointer used in cpu_asm to pass the exception stack frame 
** address to the context switch code.
*/
#if (__mips == 1) || (__mips == 32)
typedef uint32_t ESF_PTR_TYPE;
#elif (__mips == 3) 
typedef uint64_t ESF_PTR_TYPE;
#else
#error "unknown MIPS ISA"
#endif

ESF_PTR_TYPE __exceptionStackFrame = 0; 




/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    thread_dispatch - address of dispatching routine
 */
void _CPU_Initialize(
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

#if CPU_HARDWARE_FP
  /* FP context initialization support goes here */
  _CPU_Null_fp_context.fpcs = 0x1000000; 	/* Set FS flag in floating point coprocessor
  						   control register to prevent underflow and
  						   inexact exceptions */
#endif
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 *
 *  This routine returns the current interrupt level.
 */

uint32_t   _CPU_ISR_Get_level( void )
{
  unsigned int sr;

  mips_get_sr(sr);

  /* printf("current sr=%08X, ",sr); */

#if (__mips == 3) || (__mips == 32)
/* IE bit and shift down hardware ints into bits 1 thru 6 */
  sr = (sr & SR_IE) | ((sr & mips_interrupt_mask()) >> 9);

#elif __mips == 1
/* IEC bit and shift down hardware ints into bits 1 thru 6 */
  sr = (sr & SR_IEC) | ((sr & mips_interrupt_mask()) >> 9);

#else
#error "CPU ISR level: unknown MIPS level for SR handling"
#endif
  return sr;
}


void _CPU_ISR_Set_level( uint32_t   new_level )
{
  unsigned int sr, srbits;

  /* 
  ** mask off the int level bits only so we can 
  ** preserve software int settings and FP enable
  ** for this thread.  Note we don't force software ints
  ** enabled when changing level, they were turned on
  ** when this task was created, but may have been turned 
  ** off since, so we'll just leave them alone.
  */

  new_level &= 0xff;

  mips_get_sr(sr);

#if (__mips == 3) || (__mips == 32)
  mips_set_sr( (sr & ~SR_IE) );                 /* first disable ie bit (recommended) */

   srbits = sr & ~(0xfc00 | SR_IE);

   sr = srbits | ((new_level==0)? (mips_interrupt_mask() | SR_IE): \
		 (((new_level<<9) & mips_interrupt_mask()) | \
                   ((new_level & 1)?SR_IE:0)));
/*
  if ( (new_level & SR_EXL) == (sr & SR_EXL) )
    return;

  if ( (new_level & SR_EXL) == 0 ) {
    sr &= ~SR_EXL;                    * clear the EXL bit *
    mips_set_sr(sr);
  } else {

    sr |= SR_EXL|SR_IE;              * enable exception level *
    mips_set_sr(sr);                 * first disable ie bit (recommended) *
  }
*/
 
#elif __mips == 1
  mips_set_sr( (sr & ~SR_IEC) );
  srbits = sr & ~(0xfc00 | SR_IEC);
  sr = srbits | ((new_level==0)?0xfc01:( ((new_level<<9) & 0xfc00) | \
                                         (new_level & SR_IEC)));
#else
#error "CPU ISR level: unknown MIPS level for SR handling"
#endif
  mips_set_sr( sr );
}



/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    old_handler - former ISR for this vector number
 *    new_handler - replacement ISR for this vector number
 *
 *  Output parameters:  NONE
 *
 */
 
void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  /*
   *  This is where we install the interrupt handler into the "raw" interrupt
   *  table used by the CPU to dispatch interrupt handlers.
   *
   *  Because all interrupts are vectored through the same exception handler
   *  this is not necessary on thi sport.
   */
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

void _CPU_ISR_install_vector(
  uint32_t    vector,
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

   _CPU_ISR_install_raw_handler( vector, _ISR_Handler, old_handler );

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ vector ] = new_handler;
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
/* we don't support this yet */
}

/*PAGE
 *
 *  _CPU_Internal_threads_Idle_thread_body
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

void *_CPU_Thread_Idle_body( uint32_t ignored )
{
#if (__mips == 3) || (__mips == 32)
   for( ; ; )
     asm volatile("wait"); /* use wait to enter low power mode */
#elif __mips == 1
   for( ; ; )
     ;
#else
#error "IDLE: __mips not set to 1 or 3"
#endif
}
