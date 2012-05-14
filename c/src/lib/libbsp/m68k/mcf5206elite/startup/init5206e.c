/*
 *  MCF5206e hardware startup routines
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
#include "mcf5206/mcf5206e.h"

extern void CopyDataClearBSSAndStart(unsigned long ramsize);
extern void INTERRUPT_VECTOR(void);

#define m68k_set_srambar( _rambar0 ) \
  __asm__ volatile (  "movec %0,%%rambar0\n\t" \
                  "nop\n\t" \
                  : : "d" (_rambar0) )

#define m68k_set_mbar( _mbar ) \
  __asm__ volatile (  "movec %0,%%mbar\n\t" \
                  "nop\n\t" \
                  : : "d" (_mbar) )

#define mcf5206e_enable_cache() \
  m68k_set_cacr( MCF5206E_CACR_CENB )

#define mcf5206e_disable_cache() \
  __asm__ volatile (  "nop\n\t"    \
                  "movec %0,%%cacr\n\t" \
                  "nop\n\t" \
                  "movec %0,%%cacr\n\t" \
                  "nop\n\t" \
                  : : "d" (MCF5206E_CACR_CINV) )

/* Init5206e --
 *     Initialize MCF5206e on-chip modules
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
Init5206e(void)
{

    /* Set Module Base Address register */
    m68k_set_mbar((MBAR & MCF5206E_MBAR_BA) | MCF5206E_MBAR_V);

    /* Set System Protection Control Register (SYPCR):
     * Bus Monitor Enable, Bus Monitor Timing = 1024 clocks,
     * Software watchdog disabled
     */
    *MCF5206E_SYPCR(MBAR) = MCF5206E_SYPCR_BME |
                            MCF5206E_SYPCR_BMT_1024;

    /* Set Pin Assignment Register (PAR):
     *     Output Timer 0 (not DREQ) on *TOUT[0] / *DREQ[1]
     *     Input Timer 0 (not DREQ) on *TIN[0] / *DREQ[0]
     *     IRQ, not IPL
     *     UART2 RTS signal (not \RSTO)
     *     PST/DDATA (not PPIO)
     *     *WE (not CS/A)
     */
    *MCF5206E_PAR(MBAR) = MCF5206E_PAR_PAR9_TOUT |
                          MCF5206E_PAR_PAR8_TIN0 |
                          MCF5206E_PAR_PAR7_UART2 |
                          MCF5206E_PAR_PAR6_IRQ |
                          MCF5206E_PAR_PAR5_PST |
                          MCF5206E_PAR_PAR4_DDATA |
                          MCF5206E_PAR_WE0_WE1_WE2_WE3;

    /* Set SIM Configuration Register (SIMR):
     * Disable software watchdog timer and bus timeout monitor when
     * internal freeze signal is asserted.
     */
    *MCF5206E_SIMR(MBAR) = MCF5206E_SIMR_FRZ0 | MCF5206E_SIMR_FRZ1;

    /* Set Interrupt Mask Register: Disable all interrupts */
    *MCF5206E_IMR(MBAR) = 0xFFFF;

    /* Assign Interrupt Control Registers as it is defined in bsp.h */
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_EXT_IPL1) =
                            (BSP_INTLVL_AVEC1 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_AVEC1 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_EXT_IPL2) =
                            (BSP_INTLVL_AVEC2 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_AVEC2 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_EXT_IPL3) =
                            (BSP_INTLVL_AVEC3 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_AVEC3 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_EXT_IPL4) =
                            (BSP_INTLVL_AVEC4 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_AVEC4 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_EXT_IPL5) =
                            (BSP_INTLVL_AVEC5 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_AVEC5 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_EXT_IPL6) =
                            (BSP_INTLVL_AVEC6 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_AVEC6 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_EXT_IPL7) =
                            (BSP_INTLVL_AVEC7 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_AVEC7 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_TIMER_1) =
                            (BSP_INTLVL_TIMER1 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_TIMER1 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_TIMER_2) =
                            (BSP_INTLVL_TIMER2 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_TIMER2 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_MBUS) =
                            (BSP_INTLVL_MBUS << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_MBUS << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_UART_1) =
                            (BSP_INTLVL_UART1 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_UART1 << MCF5206E_ICR_IP_S);
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_UART_2) =
                            (BSP_INTLVL_UART2 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_UART2 << MCF5206E_ICR_IP_S);
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_DMA_0) =
                            (BSP_INTLVL_DMA0 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_DMA0 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;
    *MCF5206E_ICR(MBAR,MCF5206E_INTR_DMA_1) =
                            (BSP_INTLVL_DMA1 << MCF5206E_ICR_IL_S) |
                            (BSP_INTPRIO_DMA1 << MCF5206E_ICR_IP_S) |
                            MCF5206E_ICR_AVEC;

    /* Software Watchdog timer (not used now) */
    *MCF5206E_SWIVR(MBAR) = 0x0F; /* Uninitialized interrupt */
    *MCF5206E_SWSR(MBAR) = MCF5206E_SWSR_KEY1;
    *MCF5206E_SWSR(MBAR) = MCF5206E_SWSR_KEY2;

    /* Configuring Chip Selects */
    /* CS2: SRAM memory */
    *MCF5206E_CSAR(MBAR,2) = BSP_MEM_ADDR_ESRAM >> 16;
    *MCF5206E_CSMR(MBAR,2) = BSP_MEM_MASK_ESRAM;
    *MCF5206E_CSCR(MBAR,2) = MCF5206E_CSCR_WS1 |
                             MCF5206E_CSCR_PS_32 |
                             MCF5206E_CSCR_AA |
                             MCF5206E_CSCR_EMAA |
                             MCF5206E_CSCR_WR |
                             MCF5206E_CSCR_RD;

    /* CS3: GPIO on eLITE board */
    *MCF5206E_CSAR(MBAR,3) = BSP_MEM_ADDR_GPIO >> 16;
    *MCF5206E_CSMR(MBAR,3) = BSP_MEM_MASK_GPIO;
    *MCF5206E_CSCR(MBAR,3) = MCF5206E_CSCR_WS15 |
                             MCF5206E_CSCR_PS_16 |
                             MCF5206E_CSCR_AA |
                             MCF5206E_CSCR_EMAA |
                             MCF5206E_CSCR_WR |
                             MCF5206E_CSCR_RD;

    {
        uint32_t         *inttab = (uint32_t*)&INTERRUPT_VECTOR;
        uint32_t         *intvec = (uint32_t*)BSP_MEM_ADDR_ESRAM;
        register int i;
        for (i = 0; i < 256; i++)
        {
            *(intvec++) = *(inttab++);
        }
    }
    m68k_set_vbr(BSP_MEM_ADDR_ESRAM);

    /* CS0: Flash EEPROM */
    *MCF5206E_CSAR(MBAR,0) = BSP_MEM_ADDR_FLASH >> 16;
    *MCF5206E_CSCR(MBAR,0) = MCF5206E_CSCR_WS3 |
                             MCF5206E_CSCR_AA |
                             MCF5206E_CSCR_PS_16 |
                             MCF5206E_CSCR_EMAA |
                             MCF5206E_CSCR_WR |
                             MCF5206E_CSCR_RD;
    *MCF5206E_CSMR(MBAR,0) = BSP_MEM_MASK_FLASH;

    /*
     * Invalidate the cache and disable it
     */
    mcf5206e_disable_cache();

    /*
     * Setup ACRs so that if cache turned on, periphal accesses
     * are not messed up.  (Non-cacheable, serialized)
     */
    m68k_set_acr0 ( 0
        | MCF5206E_ACR_BASE(BSP_MEM_ADDR_ESRAM)
        | MCF5206E_ACR_MASK(BSP_MEM_MASK_ESRAM)
        | MCF5206E_ACR_EN
        | MCF5206E_ACR_SM_ANY
    );
    m68k_set_acr1 ( 0
        | MCF5206E_ACR_BASE(BSP_MEM_ADDR_FLASH)
        | MCF5206E_ACR_MASK(BSP_MEM_MASK_FLASH)
        | MCF5206E_ACR_EN
        | MCF5206E_ACR_SM_ANY
    );

    mcf5206e_enable_cache();

  /*
   * Copy data, clear BSS, switch stacks and call boot_card()
   */
  CopyDataClearBSSAndStart (BSP_MEM_SIZE_ESRAM - 0x400);
}
