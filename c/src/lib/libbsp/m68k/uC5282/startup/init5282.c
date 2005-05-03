/*
 *  This is where the real hardware setup is done. A minimal stack
 *  has been provided by the start.S code. No normal C or RTEMS
 *  functions can be called from here.
 * 
 * This routine is pretty simple for the uC5282 because all the hard
 * work has been done by the bootstrap dBUG code.
 */

#include <rtems.h>
#include <bsp.h>

#define m68k_set_cacr(_cacr) asm volatile ("movec %0,%%cacr" : : "d" (_cacr))
#define m68k_set_acr0(_acr0) asm volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) asm volatile ("movec %0,%%acr1" : : "d" (_acr1))

void Init5282 (void)
{
    extern void CopyDataClearBSSAndStart (void);
    
    /*
     * Set the VBR to point to where the assembly code copied it
     */
    m68k_set_vbr(0);

    /*
     * Copy data, clear BSS and call boot_card()
     */
    CopyDataClearBSSAndStart ();
}
