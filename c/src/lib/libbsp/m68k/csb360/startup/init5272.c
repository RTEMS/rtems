/*
 *  CSB360 hardware startup routines
 *
 *  This is where the real hardware setup is done. A minimal stack
 *  has been provided by the start.S code. No normal C or RTEMS
 *  functions can be called from here.
 *
 *  This initialization code based on hardware settings of dBUG
 *  monitor. This must be changed if you like to run it immediately
 *  after reset.
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  Based on work:
 *  Author:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <mcf5272/mcf5272.h>

/* externs */
extern void clear_bss(void);
extern void start_csb360(void);
extern void INTERRUPT_VECTOR(void);

/* Set the pointers to the modules */
sim_regs_t *g_sim_regs            = (void *) MCF5272_SIM_BASE(BSP_MBAR);
intctrl_regs_t *g_intctrl_regs    = (void *) MCF5272_INT_BASE(BSP_MBAR);
chipsel_regs_t *g_chipsel_regs    = (void *) MCF5272_CS_BASE(BSP_MBAR);
gpio_regs_t *g_gpio_regs          = (void *) MCF5272_GPIO_BASE(BSP_MBAR);
qspi_regs_t *g_qspi_regs          = (void *) MCF5272_QSPI_BASE(BSP_MBAR);
pwm_regs_t *g_pwm_regs            = (void *) MCF5272_PWM_BASE(BSP_MBAR);
dma_regs_t *g_dma_regs            = (void *) MCF5272_DMAC_BASE(BSP_MBAR);
uart_regs_t *g_uart0_regs         = (void *) MCF5272_UART0_BASE(BSP_MBAR);
uart_regs_t *g_uart1_regs         = (void *) MCF5272_UART1_BASE(BSP_MBAR);
timer_regs_t *g_timer_regs        = (void *) MCF5272_TIMER_BASE(BSP_MBAR);
plic_regs_t *g_plic_regs          = (void *) MCF5272_PLIC_BASE(BSP_MBAR);
enet_regs_t *g_enet_regs          = (void *) MCF5272_ENET_BASE(BSP_MBAR);
usb_regs_t *g_usb_regs            = (void *) MCF5272_USB_BASE(BSP_MBAR);

#define m68k_set_srambar( _rambar0 ) \
  __asm__ volatile (  "movec %0,%%rambar0\n\t" \
                  "nop\n\t" \
                  : : "d" (_rambar0) )

#define m68k_set_mbar( _mbar ) \
  __asm__ volatile (  "movec %0,%%mbar\n\t" \
                  "nop\n\t" \
                  : : "d" (_mbar) )

#define mcf5272_enable_cache() \
  m68k_set_cacr( MCF5272_CACR_CENB )


#define mcf5272_disable_cache() \
  __asm__ volatile (  "nop\n\t"    \
                  "movec %0,%%cacr\n\t" \
                  "nop\n\t" \
                  "movec %0,%%cacr\n\t" \
                  "nop\n\t" \
                  : : "d" (MCF5272_CACR_CINV) )

/* init5272 --
 *     Initialize MCF5272 on-chip modules
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
init5272(void)
{
    /* Invalidate the cache - WARNING: It won't complete for 64 clocks */
    m68k_set_cacr(MCF5272_CACR_CINV);

    /* Set Module Base Address register */
    m68k_set_mbar((BSP_MBAR & MCF5272_MBAR_BA) | MCF5272_MBAR_V);

    /* Set RAM Base Address register */
    m68k_set_srambar((BSP_RAMBAR & MCF5272_RAMBAR_BA) | MCF5272_RAMBAR_V);

    /* Set System Control Register:
     * Enet has highest priority, 16384 bus cycles before timeout
     */
    g_sim_regs->scr = (MCF5272_SCR_HWR_16384);

    /* System Protection Register:
     * Enable Hardware watchdog timer.
     */
    g_sim_regs->spr = MCF5272_SPR_HWTEN;

    /* Clear and mask all interrupts */
    g_intctrl_regs->icr1 = 0x88888888;
    g_intctrl_regs->icr2 = 0x88888888;
    g_intctrl_regs->icr3 = 0x88888888;
    g_intctrl_regs->icr4 = 0x88880000;

    /* Copy the interrupt vector table to SRAM */
    {
        uint32_t *inttab = (uint32_t *)&INTERRUPT_VECTOR;
        uint32_t *intvec = (uint32_t *)BSP_RAMBAR;
        register int i;
        for (i = 0; i < 256; i++)
        {
            *(intvec++) = *(inttab++);
        }
    }
    m68k_set_vbr(BSP_RAMBAR);


    /*
     * Setup ACRs so that if cache turned on, periphal accesses
     * are not messed up.  (Non-cacheable, serialized)
     */

    m68k_set_acr0(MCF5272_ACR_BASE(BSP_MEM_ADDR_SDRAM)    |
                  MCF5272_ACR_MASK(BSP_MEM_MASK_SDRAM)    |
                  MCF5272_ACR_EN                          |
                  MCF5272_ACR_SM_ANY);

/*
    m68k_set_acr1 (MCF5206E_ACR_BASE(BSP_MEM_ADDR_FLASH) |
                   MCF5206E_ACR_MASK(BSP_MEM_MASK_FLASH) |
                   MCF5206E_ACR_EN                       |
                   MCF5206E_ACR_SM_ANY);
*/

    /* Enable the caches */
    m68k_set_cacr(MCF5272_CACR_CENB |
                  MCF5272_CACR_DCM);       /* Default is not cached */

  /*
   * Copy data, clear BSS, switch stacks and call boot_card()
   */
/*
  CopyDataClearBSSAndStart(BSP_MEM_SIZE_ESRAM - 0x400);
*/
    clear_bss();
    start_csb360();

}
