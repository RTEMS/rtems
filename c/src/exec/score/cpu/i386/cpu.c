/*
 *  Intel i386 Dependent Source
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/fatal.h>
#include <rtems/core/isr.h>
#include <rtems/core/wkspace.h>

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
  register unsigned16  fp_status asm ("ax");
  register unsigned8  *fp_context;

  _CPU_Table = *cpu_table;

  /*
   *  The following code saves a NULL i387 context which is given
   *  to each task at start and restart time.  The following code
   *  is based upon that provided in the i386 Programmer's
   *  Manual and should work on any coprocessor greater than
   *  the i80287.
   *
   *  NOTE: The NO RTEMS_WAIT form of the coprocessor instructions
   *        MUST be used in case there is not a coprocessor
   *        to wait for.
   */

  fp_status = 0xa5a5;
  asm volatile( "fninit" );
  asm volatile( "fnstsw %0" : "=a" (fp_status) : "0" (fp_status) );

  if ( fp_status ==  0 ) {

    fp_context = _CPU_Null_fp_context;

    asm volatile( "fsave (%0)" : "=r" (fp_context)
                               : "0"  (fp_context)
                );
  }
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level;
 
  i386_get_interrupt_level( level );
 
  return level;
}
 
/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 */
 
#if __GO32__
#include <go32.h>
#include <dpmi.h>
#endif /* __GO32__ */

void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
#if __GO32__
    _go32_dpmi_seginfo handler_info;
 
    /* get the address of the old handler */
    _go32_dpmi_get_protected_mode_interrupt_vector( vector, &handler_info);
 
    /* Notice how we're failing to save the pm_segment portion of the */
    /* structure here?  That means we might crash the system if we  */
    /* try to restore the ISR.  Can't fix this until i386_isr is  */
    /* redefined.  XXX [BHC].           */
    *old_handler = (proc_ptr *) handler_info.pm_offset;

    handler_info.pm_offset = (u_long) new_handler;
    handler_info.pm_selector = _go32_my_cs();

    /* install the IDT entry */
    _go32_dpmi_set_protected_mode_interrupt_vector( vector, &handler_info );
#else
  i386_IDT_slot idt;
  unsigned32    handler;

  *old_handler =  0;    /* XXX not supported */

  handler = (unsigned32) new_handler;

  /* build the IDT entry */
  idt.offset_0_15      = handler & 0xffff;
  idt.segment_selector = i386_get_cs();
  idt.reserved         = 0x00;
  idt.p_dpl            = 0x8e;         /* present, ISR */
  idt.offset_16_31     = handler >> 16;

  /* install the IDT entry */
  i386_Install_idt(
    (unsigned32) &idt,
    _CPU_Table.interrupt_table_segment,
    (unsigned32) _CPU_Table.interrupt_table_offset + (8 * vector)
  );
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

void _ISR_Handler_0(), _ISR_Handler_1();

#define PER_ISR_ENTRY \
    (((unsigned32) _ISR_Handler_1 - (unsigned32) _ISR_Handler_0))

#define _Interrupt_Handler_entry( _vector ) \
   (((unsigned32)_ISR_Handler_0) + ((_vector) * PER_ISR_ENTRY))

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  proc_ptr      ignored;
  unsigned32    unique_handler;

  *old_handler = _ISR_Vector_table[ vector ];

  /* calculate the unique entry point for this vector */
  unique_handler = _Interrupt_Handler_entry( vector );

  _CPU_ISR_install_raw_handler( vector, (void *)unique_handler, &ignored );

  _ISR_Vector_table[ vector ] = new_handler;
}
