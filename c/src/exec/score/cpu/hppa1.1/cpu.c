/*
 *  HP PA-RISC Dependent Source
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
void hppa_cpu_halt(unsigned32 the_error);


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
   *  This is unsupported.  For HPPA this function is handled by BSP
   */

  _CPU_Fatal_halt( 0xdeaddead );
}



/*
 * This is the default handler which is called if
 * _CPU_ISR_install_vector() has not been called for the
 * specified vector.  It simply forwards onto the spurious
 * handler defined in the cpu-table.
 */

static ISR_Handler
hppa_interrupt_report_spurious(ISR_Vector_number vector,
                               void* rtems_isr_frame) /* HPPA extension */
{

    /*
     * If the CPU table defines a spurious_handler, then
     * call it.  If the handler returns halt.
     */
    if ( _CPU_Table.spurious_handler )
        _CPU_Table.spurious_handler(vector, rtems_isr_frame);
    
    hppa_cpu_halt(vector);
}


/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level(void)
{
    int level;
    HPPA_ASM_SSM(0, level);	/* change no bits; just get copy */
    if (level & HPPA_PSW_I)
        return 0;
    return 1;
}

/*PAGE
 *
 *  _CPU_ISR_install_vector
 *
 *  This kernel routine installs the RTEMS handler for the
 *  specified vector.  The handler is a C callable routine.
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
    *old_handler = _ISR_Vector_table[vector];

    _ISR_Vector_table[vector] = new_handler;
}

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 *
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
    register unsigned8  *fp_context;
    unsigned32 i;
    proc_ptr   old_handler;

    /*
     * This is the default fp context for all tasks
     * Set it up so that denormalized results go to zero.
     */

    fp_context = (unsigned8*) &_CPU_Null_fp_context;
    for (i=0 ; i<sizeof(Context_Control_fp); i++)
        *fp_context++ = 0;
    *((unsigned32 *) &_CPU_Null_fp_context) = HPPA_FPSTATUS_D;

    /*
     *  Save r27 into _CPU_Default_gr27 so it will hopefully be the correct
     *  global data pointer for the entire system.
     */

    asm volatile( "stw   %%r27,%0" : "=m" (_CPU_Default_gr27): );

    /*
     * Init the 2nd level interrupt handlers
     */

    for (i=0; i < CPU_INTERRUPT_NUMBER_OF_VECTORS; i++)
        _CPU_ISR_install_vector(i,
                                hppa_interrupt_report_spurious,
                                &old_handler);

    _CPU_Table = *cpu_table;
    
}


/*
 * Halt the system.
 * Called by the _CPU_Fatal_halt macro
 *
 * XXX
 * Later on, this will allow us to return to the prom.
 * For now, we just ignore 'type_of_halt'
 *
 * XXX
 * NOTE: for gcc, this function must be at the bottom
 * of the file, that is because if it is at the top
 * of the file, gcc will inline it's calls.  Since
 * the function uses the HPPA_ASM_LABEL() macro, when
 * gcc inlines it, you get two definitions of the same
 * label name, which is an assembly error.
 */


void
hppa_cpu_halt(unsigned32 the_error)
{
    unsigned32 isrlevel;

    _CPU_ISR_Disable(isrlevel);

    /*
     * XXXXX NOTE: This label is only needed that that when
     * the simulator stops, it shows the label name specified
     */
    /* HPPA_ASM_LABEL("_asm_hppa_cpu_halt");*/
    HPPA_ASM_BREAK(0, 0);
}

