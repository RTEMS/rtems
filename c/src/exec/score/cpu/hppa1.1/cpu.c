/*
 *  HP PA-RISC Dependent Source
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Division Incorporated not be
 *      used in advertising or publicity pertaining to distribution
 *      of the software without specific, written prior permission.
 *      Division Incorporated makes no representations about the
 *      suitability of this software for any purpose.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/fatal.h>
#include <rtems/isr.h>
#include <rtems/intr.h>
#include <rtems/wkspace.h>

rtems_status_code hppa_external_interrupt_initialize(void);
void hppa_external_interrupt_enable(unsigned32);
void hppa_external_interrupt_disable(unsigned32);
void hppa_external_interrupt(unsigned32, CPU_Interrupt_frame *);

/*
 * Our interrupt handlers take a 2nd argument:
 *   a pointer to a CPU_Interrupt_frame
 * So we use our own prototype instead of rtems_isr_entry
 */

typedef rtems_isr ( *hppa_rtems_isr_entry )(
    rtems_vector_number,
    CPU_Interrupt_frame *
 );


/*
 * who are we?  cpu number
 * Not used by executive proper, just kept (or not) as a convenience
 * for libcpu and libbsp stuff that wants it.
 *
 * Defaults to 0.  If the BSP doesn't like it, it can change it.
 */

int cpu_number;                 /* from 0; cpu number in a multi cpu system */


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
    unsigned32 iva;
    unsigned32 iva_table;
    int i;

    extern void IVA_Table(void);

    if ( cpu_table == NULL )
        rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED );

    /*
     * XXX; need to setup fpsr smarter perhaps
     */

    fp_context = (unsigned8*) &_CPU_Null_fp_context;
    for (i=0 ; i<sizeof(Context_Control_fp); i++)
        *fp_context++ = 0;

    /*
     *  Set _CPU_Default_gr27 here so it will hopefully be the correct
     *  global data pointer for the entire system.
     */

    asm volatile( "stw   %%r27,%0" : "=m" (_CPU_Default_gr27): );

    /*
     * Stabilize the interrupt stuff
     */

    (void) hppa_external_interrupt_initialize();

    /*
     * Set the IVA to point to physical address of the IVA_Table
     */

    iva_table = (unsigned32) IVA_Table;
    HPPA_ASM_LPA(0, iva_table, iva);
    set_iva(iva);

    _CPU_Table = *cpu_table;
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
   *  This is unsupported.
   */

  _CPU_Fatal_halt( 0xdeaddead );
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

/*
 * HPPA has 8w for each vector instead of an address to jump to.
 * We put the actual ISR address in '_ISR_vector_table'.  This will
 * be pulled by the code in the vector.
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
    *old_handler = _ISR_Vector_table[vector];

    _ISR_Vector_table[vector] = new_handler;

    if (vector >= HPPA_INTERRUPT_EXTERNAL_BASE)
    {
        unsigned32 external_vector;

        external_vector = vector - HPPA_INTERRUPT_EXTERNAL_BASE;
        if (new_handler)
            hppa_external_interrupt_enable(external_vector);
        else
            /* XXX this can never happen due to _ISR_Is_valid_user_handler */
            hppa_external_interrupt_disable(external_vector);
    }
}


/*
 * Support for external and spurious interrupts on HPPA
 *
 *  TODO:
 *    delete interrupt.c etc.
 *    Count interrupts
 *    make sure interrupts disabled properly
 *    should handler check again for more interrupts before exit?
 *    How to enable interrupts from an interrupt handler?
 *    Make sure there is an entry for everything in ISR_Vector_Table
 */

#define DISMISS(mask)           set_eirr(mask)
#define DISABLE(mask)           set_eiem(get_eiem() & ~(mask))
#define ENABLE(mask)            set_eiem(get_eiem() | (mask))
#define VECTOR_TO_MASK(v)       (1 << (31 - (v)))

/*
 * Init the external interrupt scheme
 * called by bsp_start()
 */

rtems_status_code
hppa_external_interrupt_initialize(void)
{
    rtems_isr_entry ignore;

    /* mark them all unused */

    DISABLE(~0);
    DISMISS(~0);

    /* install the external interrupt handler */
    rtems_interrupt_catch((rtems_isr_entry) hppa_external_interrupt,
                          HPPA_INTERRUPT_EXTERNAL_INTERRUPT, &ignore) ;

    return RTEMS_SUCCESSFUL;
}

/*
 * Enable a specific external interrupt
 */

void
hppa_external_interrupt_enable(unsigned32 v)
{
    unsigned32 isrlevel;

    _CPU_ISR_Disable(isrlevel);
    ENABLE(VECTOR_TO_MASK(v));
    _CPU_ISR_Enable(isrlevel);
}

/*
 * Does not clear or otherwise affect any pending requests
 */

void
hppa_external_interrupt_disable(unsigned32 v)
{
    unsigned32 isrlevel;

    _CPU_ISR_Disable(isrlevel);
    DISABLE(VECTOR_TO_MASK(v));
    _CPU_ISR_Enable(isrlevel);
}

void
hppa_external_interrupt_spurious_handler(unsigned32           vector,
                                         CPU_Interrupt_frame *iframe)
{
/* XXX should not be printing :)
    printf("spurious external interrupt: %d at pc 0x%x; disabling\n",
       vector, iframe->Interrupt.pcoqfront);
*/
    DISMISS(VECTOR_TO_MASK(vector));
    DISABLE(VECTOR_TO_MASK(vector));
}

void
hppa_external_interrupt_report_spurious(unsigned32           spurious,
                                        CPU_Interrupt_frame *iframe)
{
    int v;
    for (v=0; v < HPPA_EXTERNAL_INTERRUPTS; v++)
        if (VECTOR_TO_MASK(v) & spurious)
            hppa_external_interrupt_spurious_handler(v, iframe);
    DISMISS(spurious);
}


/*
 * External interrupt handler.
 * This is installed as cpu interrupt handler for
 * HPPA_INTERRUPT_EXTERNAL_INTERRUPT. It vectors out to
 * specific external interrupt handlers.
 */

void
hppa_external_interrupt(unsigned32           vector,
                        CPU_Interrupt_frame *iframe)
{
    unsigned32   mask;
    unsigned32  *vp, *max_vp;
    unsigned32   external_vector;
    unsigned32   global_vector;
    hppa_rtems_isr_entry handler;

    max_vp = &_CPU_Table.external_interrupt[_CPU_Table.external_interrupts];
    while ( (mask = (get_eirr() & get_eiem())) )
    {
        for (vp = _CPU_Table.external_interrupt; (vp < max_vp) && mask; vp++)
        {
            unsigned32 m;

            external_vector = *vp;
            global_vector = external_vector + HPPA_INTERRUPT_EXTERNAL_BASE;
            m = VECTOR_TO_MASK(external_vector);
            handler = (hppa_rtems_isr_entry) _ISR_Vector_table[global_vector];
            if ((m & mask) && handler)
            {
                DISMISS(m);
                mask &= ~m;
                (*handler)(global_vector, iframe);
            }
        }

        if (mask != 0) {
            if ( _CPU_Table.spurious_handler )
              (*((hppa_rtems_isr_entry) _CPU_Table.spurious_handler))(
                  mask,
                  iframe
                );
            else
              hppa_external_interrupt_report_spurious(mask, iframe);
        }
    }
}

/*
 * Halt the system.
 * Called by the _CPU_Fatal_halt macro
 *
 * XXX
 * Later on, this will allow us to return to the prom.
 * For now, we just ignore 'type_of_halt'
 */

void
hppa_cpu_halt(unsigned32 type_of_halt,
              unsigned32 the_error)
{
    unsigned32 isrlevel;

    _CPU_ISR_Disable(isrlevel);

    asm volatile( "copy %0,%%r1" : : "r" (the_error) );
    HPPA_ASM_BREAK(1, 0);
}
