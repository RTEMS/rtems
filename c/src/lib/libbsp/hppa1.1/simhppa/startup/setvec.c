/*  set_vector
 *
 *  This routine installs an interrupt vector on the HP PA-RISC simulator.
 *
 *  INPUT:
 *    handler - interrupt handler entry point
 *    vector  - vector number
 *    type    - 0 indicates raw hardware connect
 *              1 indicates RTEMS interrupt connect
 *
 *  NOTE 'type' is ignored on hppa; all interrupts are owned by RTEMS
 *
 *  RETURNS:
 *    address of previous interrupt handler
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

#include <rtems.h>

/*
 * Install an interrupt handler in the right place
 * given its vector number from cpu/hppa.h
 * There are 2 places an interrupt can be installed
 *      _ISR_Vector_table
 *      bsp interrupt      XXX: nyi
 *
 * We decide which based on the vector number
 */

rtems_isr_entry
set_vector(                                     /* returns old vector */
    rtems_isr_entry     handler,                /* isr routine        */
    rtems_vector_number vector,                 /* vector number      */
    int                 type                    /* RTEMS or RAW intr  */
)
{
    rtems_isr_entry previous_isr;

    /* is it an on chip interrupt? */
    /* XXX this should say CPU_INTERRUPT_NUMBER_OF_VECTORS */
    if (vector < HPPA_INTERRUPT_MAX)
    {
        rtems_interrupt_catch(handler, vector, &previous_isr);
    }
#if 0   /* XXX */
    else if ((vector >= HPPA_INTERRUPT_BSP_BASE) &&
             (vector < (HPPA_INTERRUPT_BSP_BASE + HPPA_BSP_INTERRUPTS)))
    {
        simhppa_interrupt_install(handler,
                                  vector - HPPA_INTERRUPT_BSP_BASE,
                                  (rtems_isr_entry *) &previous_isr);
    }
#endif

    return previous_isr;
}

