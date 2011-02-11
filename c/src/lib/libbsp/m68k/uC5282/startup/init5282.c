/*
 * This is where the real hardware setup is done. A minimal stack
 * has been provided by the start.S code. No normal C or RTEMS
 * functions can be called from here.
 *
 * This routine is pretty simple for the uC5282 because all the hard
 * work has been done by the bootstrap dBUG code.
 *
 *  Author: W. Eric Norum <norume@aps.anl.gov>
 *
 *  COPYRIGHT (c) 2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>

#define m68k_set_cacr(_cacr) __asm__ volatile ("movec %0,%%cacr" : : "d" (_cacr))
#define m68k_set_acr0(_acr0) __asm__ volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) __asm__ volatile ("movec %0,%%acr1" : : "d" (_acr1))

 extern void CopyDataClearBSSAndStart (void);

void Init5282 (void)
{
    /*
     * Copy data, clear BSS and call boot_card()
     */
    CopyDataClearBSSAndStart ();
}
