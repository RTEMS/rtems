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
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be found in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/context.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>

#include <rtems/powerpc/cache.h>
#include <rtems/powerpc/powerpc.h>

/*
 *  These are for testing purposes.
 */

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 */

static void ppc_spurious(int, CPU_Interrupt_frame *);

int _CPU_spurious_count = 0;
int _CPU_last_spurious = 0;

/* This variable is initialized to 'TRUE' by default;
 * BSPs which have their vectors in ROM should set it
 * to FALSE prior to initializing raw exceptions.
 *
 * I suspect the only candidate is the simulator.
 * After all, the value of this variable is used to
 * determine where to install the prologue code and
 * installing to ROM on anyting that's real ROM
 * will fail anyways.
 *
 * This should probably go away... (T.S. 2007/11/30)
 */
boolean bsp_exceptions_in_RAM = TRUE;

void _CPU_Initialize(
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
#if (PPC_USE_SPRG)
  int i;
#endif
  register uint32_t   r2 = 0;
  register uint32_t   r13 = 0;

  asm ("mr %0,13" : "=r" ((r13)) : "0" ((r13)));
  _CPU_IRQ_info.Default_r13 = r13;

  asm ("mr %0,2" : "=r" ((r2)) : "0" ((r2)));
  _CPU_IRQ_info.Default_r2 = r2;

  _CPU_IRQ_info.Nest_level = &_ISR_Nest_level;
  _CPU_IRQ_info.Disable_level = &_Thread_Dispatch_disable_level;
  /* fill in _CPU_IRQ_info.Vector_table later */
  _CPU_IRQ_info.Switch_necessary = &_Context_Switch_necessary;
  _CPU_IRQ_info.Signal = &_ISR_Signals_to_thread_executing;

#if (PPC_USE_SPRG)
  i = (int)&_CPU_IRQ_info;
  asm volatile("mtspr 0x113, %0" : "=r" (i) : "0" (i)); /* SPRG 3 */
#endif

  /*
   * Store Msr Value in the IRQ info structure.
   */
   _CPU_MSR_GET(_CPU_IRQ_info.msr_initial);

#if (PPC_USE_SPRG)
  i = _CPU_IRQ_info.msr_initial;
  asm volatile("mtspr 0x112, %0" : "=r" (i) : "0" (i)); /* SPRG 2 */
#endif
}

/*
 *  _CPU_Initialize_vectors()
 *
 *  Support routine to initialize the RTEMS vector table after it is allocated.
 *
 *  PowerPC Specific Information:
 *
 *  Complete initialization since the table is now allocated.
 */

void _CPU_Initialize_vectors(void)
{
  int i;
  proc_ptr handler = (proc_ptr)ppc_spurious;
  extern void (*bsp_spurious_handler)(uint32_t   vector, CPU_Interrupt_frame *);

  _CPU_IRQ_info.Vector_table = _ISR_Vector_table;

  if ( bsp_spurious_handler )
    handler = (proc_ptr)bsp_spurious_handler;

  for (i = 0; i < PPC_INTERRUPT_MAX;  i++)
    _ISR_Vector_table[i] = handler;

}

/*PAGE
 *
 *  _CPU_ISR_Calculate_level
 *
 *  The PowerPC puts its interrupt enable status in the MSR register
 *  which also contains things like endianness control.  To be more
 *  awkward, the layout varies from processor to processor.  This
 *  is why it was necessary to adopt a scheme which allowed the user
 *  to specify specifically which interrupt sources were enabled.
 */

uint32_t   _CPU_ISR_Calculate_level(
  uint32_t   new_level
)
{
  register uint32_t   new_msr = 0;

  /*
   *  Set the critical interrupt enable bit
   */

#if (PPC_HAS_RFCI)
  if ( !(new_level & PPC_INTERRUPT_LEVEL_CE) )
    new_msr |= PPC_MSR_CE;
#endif

  if ( !(new_level & PPC_INTERRUPT_LEVEL_ME) )
    new_msr |= PPC_MSR_ME;

  if ( !(new_level & PPC_INTERRUPT_LEVEL_EE) )
    new_msr |= PPC_MSR_EE;

  return new_msr;
}

/*PAGE
 *
 *  _CPU_ISR_Set_level
 *
 *  This routine sets the requested level in the MSR.
 */

void _CPU_ISR_Set_level(
  uint32_t   new_level
)
{
  register uint32_t   tmp = 0;
  register uint32_t   new_msr;

  new_msr = _CPU_ISR_Calculate_level( new_level );

  asm volatile (
    "mfmsr %0; andc %0,%0,%1; and %2, %2, %1; or %0, %0, %2; mtmsr %0" :
    "=&r" ((tmp)) :
    "r" ((PPC_MSR_DISABLE_MASK)), "r" ((new_msr)), "0" ((tmp))
  );
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 *
 *  This routine gets the current interrupt level from the MSR and
 *  converts it to an RTEMS interrupt level.
 */

uint32_t   _CPU_ISR_Get_level( void )
{
  uint32_t   level = 0;
  uint32_t   msr;

  asm volatile("mfmsr %0" : "=r" ((msr)));

  msr &= PPC_MSR_DISABLE_MASK;

  /*
   *  Set the critical interrupt enable bit
   */

#if (PPC_HAS_RFCI)
  if ( !(msr & PPC_MSR_CE) )
    level |= PPC_INTERRUPT_LEVEL_CE;
#endif

  if ( !(msr & PPC_MSR_ME) )
    level |= PPC_INTERRUPT_LEVEL_ME;

  if ( !(msr & PPC_MSR_EE) )
    level |= PPC_INTERRUPT_LEVEL_EE;

  return level;
}

/*PAGE
 *
 *  _CPU_Context_Initialize
 */

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  boolean           is_fp
)
{
  uint32_t   msr_value;
  uint32_t   sp;

  sp = (uint32_t)stack_base + size - PPC_MINIMUM_STACK_FRAME_SIZE;
  *((uint32_t*)sp) = 0;
  the_context->gpr1 = sp;

  the_context->msr = _CPU_ISR_Calculate_level( new_level );

  /*
   *  The FP bit of the MSR should only be enabled if this is a floating
   *  point task.  Unfortunately, the vfprintf_r routine in newlib
   *  ends up pushing a floating point register regardless of whether or
   *  not a floating point number is being printed.  Serious restructuring
   *  of vfprintf.c will be required to avoid this behavior.  At this
   *  time (7 July 1997), this restructuring is not being done.
   */

  /*if ( is_fp ) */
    the_context->msr |= PPC_MSR_FP;

  /*
   *  Calculate the task's MSR value:
   *
   *     + Set the exception prefix bit to point to the exception table
   *     + Force the RI bit
   *     + Use the DR and IR bits
   */
  _CPU_MSR_GET( msr_value );
  the_context->msr |= (msr_value & PPC_MSR_EP);
#if defined(PPC_MSR_RI)
  the_context->msr |= PPC_MSR_RI;
#endif
  the_context->msr |= msr_value & (PPC_MSR_DR|PPC_MSR_IR);

#if (PPC_ABI == PPC_ABI_SVR4)
  { unsigned    r13 = 0;
    asm volatile ("mr %0, 13" : "=r" ((r13)));

    the_context->pc = (uint32_t)entry_point;
    the_context->gpr13 = r13;
  }
#endif

#if (PPC_ABI == PPC_ABI_EABI)
  { uint32_t    r2 = 0;
    unsigned    r13 = 0;
    asm volatile ("mr %0,2; mr %1,13" : "=r" ((r2)), "=r" ((r13)));

    the_context->pc = (uint32_t)entry_point;
    the_context->gpr2 = r2;
    the_context->gpr13 = r13;
  }
#endif
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
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr   ignored;
  extern void (*bsp_spurious_handler)(uint32_t   vector, CPU_Interrupt_frame *);

  *old_handler = _ISR_Vector_table[ vector ];

  /*
   *  If the interrupt vector table is a table of pointer to isr entry
   *  points, then we need to install the appropriate RTEMS interrupt
   *  handler for this vector number.
   */

  /*
   * Install the wrapper so this ISR can be invoked properly.
   */
  if (bsp_exceptions_in_RAM)
     _CPU_ISR_install_raw_handler( vector, _ISR_Handler, &ignored );

  /*
   *  We put the actual user ISR address in '_ISR_vector_table'.  This will
   *  be used by the _ISR_Handler so the user gets control.
   */

   _ISR_Vector_table[ vector ] = new_handler ? (ISR_Handler_entry)new_handler :
     bsp_spurious_handler ?
         (ISR_Handler_entry)bsp_spurious_handler :
         (ISR_Handler_entry)ppc_spurious;
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
/* PPC_ABI_EABI */
  _CPU_IRQ_info.Stack = _CPU_Interrupt_stack_high - 8;
}

/* Handle a spurious interrupt */
static void ppc_spurious(int v, CPU_Interrupt_frame *i)
{
#if 0
    printf("Spurious interrupt on vector %d from %08.8x\n",
	   v, i->pc);
#endif
#if defined(ppc403) || defined(ppc405)
    if (v == PPC_IRQ_EXTERNAL)
	{
	    register int r = 0;

	    asm volatile("mtdcr 0x42, %0" :
                "=&r" ((r)) : "0" ((r))); /* EXIER */
	}
    else if (v == PPC_IRQ_PIT)
	{
	    register int r = 0x08000000;

	    asm volatile("mtspr 0x3d8, %0" :
                "=&r" ((r)) : "0" ((r))); /* TSR */
	}
    else if (v == PPC_IRQ_FIT)
	{
	    register int r = 0x04000000;

	    asm volatile("mtspr 0x3d8, %0" :
                "=&r" ((r)) : "0" ((r))); /* TSR */
	}
#endif
    ++_CPU_spurious_count;
    _CPU_last_spurious = v;
}

void _CPU_Fatal_error(uint32_t   _error)
{
  asm volatile ("mr 3, %0" : : "r" ((_error)));
  asm volatile ("tweq 5,5");
  asm volatile ("li 0,0; mtmsr 0");
  while (1) ;
}

#define PPC_SYNCHRONOUS_TRAP_BIT_MASK    0x100
#define PPC_ASYNCHRONOUS_TRAP( _trap ) (_trap)
#define PPC_SYNCHRONOUS_TRAP ( _trap ) ((_trap)+PPC_SYNCHRONOUS_TRAP_BIT_MASK)
#define PPC_REAL_TRAP_NUMBER ( _trap ) ((_trap)%PPC_SYNCHRONOUS_TRAP_BIT_MASK)

const CPU_Trap_table_entry _CPU_Trap_slot_template = {

  0x9421ff90,           /* stwu    r1, -(IP_END)(r1)            */

  0x90010008,           /* stw   %r0, IP_0(%r1)                 */
  0x38000000,           /* li    %r0, PPC_IRQ                   */
  0x48000002            /* ba    PROC (_ISR_Handler)            */
};

#if defined(mpc860) || defined(mpc821)
const CPU_Trap_table_entry _CPU_Trap_slot_template_m860 = {
  0x7c0803ac,           /* mtlr  %r0                            */
  0x81210028,           /* lwz   %r9, IP_9(%r1)                 */
  0x38000000,           /* li    %r0, PPC_IRQ                   */
  0x48000002            /* b     PROC (_ISR_Handler)            */
};
#endif /* mpc860 */

uint32_t    ppc_exception_vector_addr(
  uint32_t   vector
);

/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs the specified handler as a "raw" non-executive
 *  supported trap handler (a.k.a. interrupt service routine).
 *
 *  Input Parameters:
 *    vector      - trap table entry number plus synchronous
 *                    vs. asynchronous information
 *    new_handler - address of the handler to be installed
 *    old_handler - pointer to an address of the handler previously installed
 *
 *  Output Parameters: NONE
 *    *new_handler - address of the handler previously installed
 *
 *  NOTE:
 *
 *  This routine is based on the SPARC routine _CPU_ISR_install_raw_handler.
 *  Install a software trap handler as an executive interrupt handler
 *  (which is desirable since RTEMS takes care of window and register issues),
 *  then the executive needs to know that the return address is to the trap
 *  rather than the instruction following the trap.
 *
 */

void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  uint32_t               real_vector;
  CPU_Trap_table_entry  *slot;
  uint32_t               u32_handler=0;

  /*
   *  Get the "real" trap number for this vector ignoring the synchronous
   *  versus asynchronous indicator included with our vector numbers.
   */

  real_vector = vector;

  /*
   *  Get the current base address of the trap table and calculate a pointer
   *  to the slot we are interested in.
   */
  slot = (CPU_Trap_table_entry  *)ppc_exception_vector_addr( real_vector );

  /*
   *  Get the address of the old_handler from the trap table.
   *
   *  NOTE: The old_handler returned will be bogus if it does not follow
   *        the RTEMS model.
   */

#define HIGH_BITS_MASK   0xFFFFFC00
#define HIGH_BITS_SHIFT  10
#define LOW_BITS_MASK    0x000003FF

  if (slot->stwu_r1 == _CPU_Trap_slot_template.stwu_r1) {
    /*
     * Set u32_handler = to target address
     */
    u32_handler = slot->b_Handler & 0x03fffffc;

    /* IMD FIX: sign extend address fragment... */
    if (u32_handler & 0x02000000) {
      u32_handler  |= 0xfc000000;
    }

    *old_handler =  (proc_ptr) u32_handler;
  } else
/* There are two kinds of handlers for the MPC860. One is the 'standard'
 *  one like above. The other is for the cascaded interrupts from the SIU
 *  and CPM. Therefore we must check for the alternate one if the standard
 *  one is not present
 */
#if defined(mpc860) || defined(mpc821)
  if (slot->stwu_r1 == _CPU_Trap_slot_template_m860.stwu_r1) {
    /*
     * Set u32_handler = to target address
     */
    u32_handler = slot->b_Handler & 0x03fffffc;
    *old_handler =  (proc_ptr) u32_handler;
  } else
#endif /* mpc860 */

    *old_handler = 0;

  /*
   *  Copy the template to the slot and then fix it.
   */
#if defined(mpc860) || defined(mpc821)
  if (vector >= PPC_IRQ_IRQ0)
    *slot = _CPU_Trap_slot_template_m860;
  else
#endif /* mpc860 */
  *slot = _CPU_Trap_slot_template;

  u32_handler = (uint32_t) new_handler;

  /*
   * IMD FIX: insert address fragment only (bits 6..29)
   *          therefore check for proper address range
   *          and remove unwanted bits
   */
  if ((u32_handler & 0xfc000000) == 0xfc000000) {
    u32_handler  &= ~0xfc000000;
  }
  else if ((u32_handler & 0xfc000000) != 0x00000000) {
    _Internal_error_Occurred(INTERNAL_ERROR_CORE,
			     TRUE,
			     u32_handler);
  }

  slot->b_Handler |= u32_handler;

  slot->li_r0_IRQ  |= vector;

  _CPU_Data_Cache_Block_Flush( slot );
}

uint32_t    ppc_exception_vector_addr(
  uint32_t   vector
)
{
#if (!PPC_HAS_EVPR)
  uint32_t   Msr;
#endif
  uint32_t   Top = 0;
  uint32_t   Offset = 0x000;

#if (PPC_HAS_EXCEPTION_PREFIX)
  _CPU_MSR_GET ( Msr );
  if ( ( Msr & PPC_MSR_EP) != 0 ) /* Vectors at FFFx_xxxx */
    Top = 0xfff00000;
#elif (PPC_HAS_EVPR)
  asm volatile( "mfspr %0,0x3d6" : "=r" (Top)); /* EVPR */
  Top = Top & 0xffff0000;
#endif

  switch ( vector ) {
    case PPC_IRQ_SYSTEM_RESET:   /* on 40x aka PPC_IRQ_CRIT */
      Offset = 0x00100;
      break;
    case PPC_IRQ_MCHECK:
      Offset = 0x00200;
      break;
    case PPC_IRQ_PROTECT:
      Offset = 0x00300;
      break;
    case PPC_IRQ_ISI:
      Offset = 0x00400;
      break;
    case PPC_IRQ_EXTERNAL:
      Offset = 0x00500;
      break;
    case PPC_IRQ_ALIGNMENT:
      Offset = 0x00600;
      break;
    case PPC_IRQ_PROGRAM:
      Offset = 0x00700;
      break;
    case PPC_IRQ_NOFP:
      Offset = 0x00800;
      break;
    case PPC_IRQ_DECREMENTER:
      Offset = 0x00900;
      break;
    case PPC_IRQ_RESERVED_A:
      Offset = 0x00a00;
      break;
    case PPC_IRQ_RESERVED_B:
      Offset = 0x00b00;
      break;
    case PPC_IRQ_SCALL:
      Offset = 0x00c00;
      break;
    case PPC_IRQ_TRACE:
      Offset = 0x00d00;
      break;
    case PPC_IRQ_FP_ASST:
      Offset = 0x00e00;
      break;

#if defined(ppc403) || defined(ppc405)

/*  PPC_IRQ_CRIT is the same vector as PPC_IRQ_RESET
    case PPC_IRQ_CRIT:
      Offset = 0x00100;
      break;
*/
    case PPC_IRQ_PIT:
      Offset = 0x01000;
      break;
    case PPC_IRQ_FIT:
      Offset = 0x01010;
      break;
    case PPC_IRQ_WATCHDOG:
      Offset = 0x01020;
      break;
    case PPC_IRQ_DEBUG:
      Offset = 0x02000;
      break;

#elif defined(ppc601)
    case PPC_IRQ_TRACE:
      Offset = 0x02000;
      break;

#elif defined(ppc603) || defined(ppc603e)
    case PPC_IRQ_TRANS_MISS:
      Offset = 0x1000;
      break;
    case PPC_IRQ_DATA_LOAD:
      Offset = 0x1100;
      break;
    case PPC_IRQ_DATA_STORE:
      Offset = 0x1200;
      break;
    case PPC_IRQ_ADDR_BRK:
      Offset = 0x1300;
      break;
    case PPC_IRQ_SYS_MGT:
      Offset = 0x1400;
      break;

#elif defined(mpc604)
    case PPC_IRQ_ADDR_BRK:
      Offset = 0x1300;
      break;
    case PPC_IRQ_SYS_MGT:
      Offset = 0x1400;
      break;

#elif defined(mpc860) || defined(mpc821)
    case PPC_IRQ_EMULATE:
      Offset = 0x1000;
      break;
    case PPC_IRQ_INST_MISS:
      Offset = 0x1100;
      break;
    case PPC_IRQ_DATA_MISS:
      Offset = 0x1200;
      break;
    case PPC_IRQ_INST_ERR:
      Offset = 0x1300;
      break;
    case PPC_IRQ_DATA_ERR:
      Offset = 0x1400;
      break;
    case PPC_IRQ_DATA_BPNT:
      Offset = 0x1c00;
      break;
    case PPC_IRQ_INST_BPNT:
      Offset = 0x1d00;
      break;
    case PPC_IRQ_IO_BPNT:
      Offset = 0x1e00;
      break;
    case PPC_IRQ_DEV_PORT:
      Offset = 0x1f00;
      break;
    case PPC_IRQ_IRQ0:
      Offset = 0x2000;
      break;
    case PPC_IRQ_LVL0:
      Offset = 0x2040;
      break;
    case PPC_IRQ_IRQ1:
      Offset = 0x2080;
      break;
    case PPC_IRQ_LVL1:
      Offset = 0x20c0;
      break;
    case PPC_IRQ_IRQ2:
      Offset = 0x2100;
      break;
    case PPC_IRQ_LVL2:
      Offset = 0x2140;
      break;
    case PPC_IRQ_IRQ3:
      Offset = 0x2180;
      break;
    case PPC_IRQ_LVL3:
      Offset = 0x21c0;
      break;
    case PPC_IRQ_IRQ4:
      Offset = 0x2200;
      break;
    case PPC_IRQ_LVL4:
      Offset = 0x2240;
      break;
    case PPC_IRQ_IRQ5:
      Offset = 0x2280;
      break;
    case PPC_IRQ_LVL5:
      Offset = 0x22c0;
      break;
    case PPC_IRQ_IRQ6:
      Offset = 0x2300;
      break;
    case PPC_IRQ_LVL6:
      Offset = 0x2340;
      break;
    case PPC_IRQ_IRQ7:
      Offset = 0x2380;
      break;
    case PPC_IRQ_LVL7:
      Offset = 0x23c0;
      break;
    case PPC_IRQ_CPM_ERROR:
      Offset = 0x2400;
      break;
    case PPC_IRQ_CPM_PC4:
      Offset = 0x2410;
      break;
    case PPC_IRQ_CPM_PC5:
      Offset = 0x2420;
      break;
    case PPC_IRQ_CPM_SMC2:
      Offset = 0x2430;
      break;
    case PPC_IRQ_CPM_SMC1:
      Offset = 0x2440;
      break;
    case PPC_IRQ_CPM_SPI:
      Offset = 0x2450;
      break;
    case PPC_IRQ_CPM_PC6:
      Offset = 0x2460;
      break;
    case PPC_IRQ_CPM_TIMER4:
      Offset = 0x2470;
      break;
    case PPC_IRQ_CPM_RESERVED_8:
      Offset = 0x2480;
      break;
    case PPC_IRQ_CPM_PC7:
      Offset = 0x2490;
      break;
    case PPC_IRQ_CPM_PC8:
      Offset = 0x24a0;
      break;
    case PPC_IRQ_CPM_PC9:
      Offset = 0x24b0;
      break;
    case PPC_IRQ_CPM_TIMER3:
      Offset = 0x24c0;
      break;
    case PPC_IRQ_CPM_RESERVED_D:
      Offset = 0x24d0;
      break;
    case PPC_IRQ_CPM_PC10:
      Offset = 0x24e0;
      break;
    case PPC_IRQ_CPM_PC11:
      Offset = 0x24f0;
      break;
    case PPC_IRQ_CPM_I2C:
      Offset = 0x2500;
      break;
    case PPC_IRQ_CPM_RISC_TIMER:
      Offset = 0x2510;
      break;
    case PPC_IRQ_CPM_TIMER2:
      Offset = 0x2520;
      break;
    case PPC_IRQ_CPM_RESERVED_13:
      Offset = 0x2530;
      break;
    case PPC_IRQ_CPM_IDMA2:
      Offset = 0x2540;
      break;
    case PPC_IRQ_CPM_IDMA1:
      Offset = 0x2550;
      break;
    case PPC_IRQ_CPM_SDMA_ERROR:
      Offset = 0x2560;
      break;
    case PPC_IRQ_CPM_PC12:
      Offset = 0x2570;
      break;
    case PPC_IRQ_CPM_PC13:
      Offset = 0x2580;
      break;
    case PPC_IRQ_CPM_TIMER1:
      Offset = 0x2590;
      break;
    case PPC_IRQ_CPM_PC14:
      Offset = 0x25a0;
      break;
    case PPC_IRQ_CPM_SCC4:
      Offset = 0x25b0;
      break;
    case PPC_IRQ_CPM_SCC3:
      Offset = 0x25c0;
      break;
    case PPC_IRQ_CPM_SCC2:
      Offset = 0x25d0;
      break;
    case PPC_IRQ_CPM_SCC1:
      Offset = 0x25e0;
      break;
    case PPC_IRQ_CPM_PC15:
      Offset = 0x25f0;
      break;
#endif

  }
  Top += Offset;
  return Top;
}

/*PAGE
 *
 *  This is the PowerPC specific implementation of the routine which
 *  returns TRUE if an interrupt is in progress.
 *
 *  NOTE: This is the same as the generic version. But since the
 *        PowerPC is still supporting old and new exception processing
 *        models and the new exception processing model has a hardware
 *        way of doing this, we have to provide this capability here
 *        for symmetry.
 */

boolean _ISR_Is_in_progress( void )
{
  return (_ISR_Nest_level != 0);
}

const unsigned int _PPC_MSR_DISABLE_MASK = PPC_MSR_DISABLE_MASK;
