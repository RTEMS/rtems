/*
 * SMFD board hardware initialization.
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *	   Alexandra Kossovsky <sasha@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include "rtems/score/sh7750_regs.h"
#include "rtems/score/sh_io.h"
#include "sdram.h"
#include "bsp.h"

/* early_hw_init --
 *     Perform initial hardware initialization:
 *         - setup clock generator
 *         - initialize bus state controller, memory settings, SDRAM
 *         - disable DMA
 *         - setup external ports, etc.
 *         - initialize interrupt controller
 *
 *     This function should not access the memory! It should be compiled
 *     with -fomit-frame-pointer to avoid stack access.
 *
 * PARAMETERS:
 *     none
 *
 * RETURNS:
 *     none
 */
void
early_hw_init(void)
{
    /* Explicitly turn off the MMU */
    write32(0, SH7750_MMUCR);

    /* Disable instruction and operand caches */
    write32(0, SH7750_CCR);

    /* Setup Clock Generator */
    /*
     * Input clock frequency is 16 MHz, MD0=1,
     * CPU clock frequency already selected to 96MHz.
     * Bus clock frequency should be set to 48 MHz, therefore divider 2
     * should be applied (bus frequency is 48 MHz, clock period is 20.84ns).
     * Peripheral frequency should be set to 24 MHz, therefore divider 4
     * should be used.
     */
    /* Prepare watchdog timer for frequency changing */
    write16((read8(SH7750_WTCSR) & ~SH7750_WTCSR_TME) |
            SH7750_WTCSR_KEY, SH7750_WTCSR);
    write16(SH7750_WTCSR_MODE_IT | SH7750_WTCSR_CKS_DIV4096 |
            SH7750_WTCSR_KEY, SH7750_WTCSR);

    /* Turn PLL1 on */
    write16(0x40 | SH7750_WTCNT_KEY, SH7750_WTCNT);
    write16(read16(SH7750_FRQCR) | SH7750_FRQCR_PLL1EN, SH7750_FRQCR);

    /* Perform Frequency Selection */
    write16(0x40 | SH7750_WTCNT_KEY, SH7750_WTCNT);
    write16(SH7750_FRQCR_CKOEN | SH7750_FRQCR_PLL1EN |
            SH7750_FRQCR_IFCDIV1 | SH7750_FRQCR_BFCDIV2 | SH7750_FRQCR_PFCDIV4,
            SH7750_FRQCR);

    /* Turn PLL2 on */
    write16(0x40 | SH7750_WTCNT_KEY, SH7750_WTCNT);
    write16(read16(SH7750_FRQCR) | SH7750_FRQCR_PLL2EN, SH7750_FRQCR);

    /* Bus State Controller Initialization */
    /*
     * Area assignments:
     *     Area 0: Flash memory, SRAM interface
     *     Area 1: GDC
     *     Area 2: SDRAM
     *     Area 3-6: unused
     */
    write32(
        /* Pull-ups (IPUP, OPUP) enabled */
        /* No Byte-Control SRAM mode for Area 1 and Area 3 */
        SH7750_BCR1_BREQEN | /* Enable external bus requests */
        /* No Partial Sharing Mode */
        /* No MPX interface */
        /* Memory and Control Signals are in HiZ */
        SH7750_BCR1_A0BST_SRAM | /* No burst ROM in flash */
        SH7750_BCR1_A5BST_SRAM | /* Area 5 is not in use */
        SH7750_BCR1_A6BST_SRAM | /* Area 6 is not in use */
        SH7750_BCR1_DRAMTP_2SDRAM_3SDRAM  /* Select Area 2 SDRAM type */
        /* Area 5,6 programmed as a SRAM interface (not PCMCIA) */,
        SH7750_BCR1);

    write16(
        (SH7750_BCR2_SZ_8 << SH7750_BCR2_A0SZ_S) |  /* These bits is read-only
                                                       and set during reset */
        (SH7750_BCR2_SZ_32 << SH7750_BCR2_A6SZ_S) | /* Area 6 not used */
        (SH7750_BCR2_SZ_32 << SH7750_BCR2_A5SZ_S) | /* Area 5 not used */
        (SH7750_BCR2_SZ_32 << SH7750_BCR2_A4SZ_S) | /* Area 4 not used */
        (SH7750_BCR2_SZ_32 << SH7750_BCR2_A3SZ_S) | /* Area 3 not used */
        (SH7750_BCR2_SZ_32 << SH7750_BCR2_A2SZ_S) | /* SDRAM is 32-bit width */
        (SH7750_BCR2_SZ_32 << SH7750_BCR2_A1SZ_S) | /* GDC is 32-bit width */
        SH7750_BCR2_PORTEN,                         /* Use D32-D51 as a port */
        SH7750_BCR2);

    write32(
        (0 << SH7750_WCR1_DMAIW_S) |  /* 0 required for SDRAM RAS down mode */
        (7 << SH7750_WCR1_A6IW_S) |   /* Area 6 not used */
        (7 << SH7750_WCR1_A5IW_S) |   /* Area 5 not used */
        (7 << SH7750_WCR1_A4IW_S) |   /* Area 4 not used */
        (7 << SH7750_WCR1_A3IW_S) |   /* Area 3 not used */
        (1 << SH7750_WCR1_A2IW_S) |   /* 1 idle cycles inserted between acc */
        (7 << SH7750_WCR1_A1IW_S) |   /* Don't have GDC specs... Set safer. */
        (1 << SH7750_WCR1_A0IW_S),    /* 1 idle cycles inserted between acc */
        SH7750_WCR1);

    write32(
        (SH7750_WCR2_WS15   << SH7750_WCR2_A6W_S) | /* Area 6 not used */
        (SH7750_WCR2_BPWS7  << SH7750_WCR2_A6B_S) |
        (SH7750_WCR2_WS15   << SH7750_WCR2_A5W_S) | /* Area 5 not used */
        (SH7750_WCR2_BPWS7  << SH7750_WCR2_A5B_S) |
        (SH7750_WCR2_WS15   << SH7750_WCR2_A4W_S) | /* Area 4 not used */
        (SH7750_WCR2_WS15   << SH7750_WCR2_A3W_S) | /*Area 3 not used*/
        (SH7750_WCR2_SDRAM_CAS_LAT2 << SH7750_WCR2_A2W_S) | /* SDRAM CL = 2 */
        (SH7750_WCR2_WS15   << SH7750_WCR2_A1W_S) | /* Area 1 (GDC)
                                                       requirements not known*/
        (SH7750_WCR2_WS6    << SH7750_WCR2_A0W_S) | /* 4 wait states required
                                                       at 48MHz for 70ns mem.,
                                                       set closest greater */
        (SH7750_WCR2_BPWS7  << SH7750_WCR2_A0B_S),  /* burst mode disabled for
                                                       Area 0 flash ROM */
        SH7750_WCR2);
    write32(
        SH7750_WCR3_A6S |                           /* Area 6 not used */
        (SH7750_WCR3_DHWS_3 << SH7750_WCR3_A6H_S) |
        SH7750_WCR3_A5S |                           /* Area 5 not used */
        (SH7750_WCR3_DHWS_3 << SH7750_WCR3_A5H_S) |
        SH7750_WCR3_A4S |                           /* Area 4 not used */
        (SH7750_WCR3_DHWS_3 << SH7750_WCR3_A4H_S) |
        SH7750_WCR3_A3S |                           /* Area 3 not used */
        (SH7750_WCR3_DHWS_3 << SH7750_WCR3_A3H_S) |
        SH7750_WCR3_A2S |                           /* SDRAM - ignored */
        (SH7750_WCR3_DHWS_3 << SH7750_WCR3_A2H_S) |
        SH7750_WCR3_A1S |                           /* GDC - unknown, set max*/
        (SH7750_WCR3_DHWS_3 << SH7750_WCR3_A1H_S) |
        0 |               /* flash ROM - no write strobe setup time required */
        (SH7750_WCR3_DHWS_0 << SH7750_WCR3_A0H_S),
        SH7750_WCR3);

    #define MCRDEF \
        /* SH7750_MCR_RASD | */  /* Set RAS Down mode */                      \
        (SH7750_MCR_TRC_0 | SH7750_MCR_TRAS_SDRAM_TRC_4 |                     \
                 /* RAS precharge time is 63ns; it corresponds to 4 clocks */ \
        /* TCAS valid only for DRAM interface */                              \
        SH7750_MCR_TPC_SDRAM_1 | /* TPC = 20ns = 1 clock */                   \
        SH7750_MCR_RCD_SDRAM_2 | /* RCD = 21ns = 2 clock */                   \
        /* After write, next active command is not issued for a period of     \
           TPC + TRWL. SDRAM specifies that it should be BL+Trp clocks when   \
           CL=2. Trp = 20ns = 1clock; BL=8. Therefore we should wait 9        \
           clocks. Don't know why, but 6 clocks (TRWL=5 and TPC=1) seems      \
           working. May be, something wrong in documentation? */              \
        SH7750_MCR_TRWL_5 |      /* TRWL = 5 clock        */                  \
        SH7750_MCR_BE |          /* Always enabled for SDRAM */               \
        SH7750_MCR_SZ_32 |       /* Memory data size is 32 bit */             \
        (4 << SH7750_MCR_AMX_S) | /* Select memory device type */             \
        SH7750_MCR_RFSH |        /* Refresh is performed */                   \
        SH7750_MCR_RMODE_NORMAL) /* Auto-Refresh mode */

    /* Clear refresh timer counter */
    write16(SH7750_RTCNT_KEY | 0, SH7750_RTCNT);

    /* Time between auto-refresh commands is 15.6 microseconds; refresh
       timer counter frequency is 12 MHz; 1.56e-5*1.2e7= 187.2, therefore
       program the refresh timer divider to 187 */
    write16(SH7750_RTCOR_KEY | 187, SH7750_RTCOR);

    /* Clear refresh counter */
    write16(SH7750_RFCR_KEY | 0, SH7750_RFCR);

    /* Select refresh counter base frequency as bus frequency/4 = 12 MHz */
    write16(SH7750_RTCSR_CKS_CKIO_DIV4 | SH7750_RTCSR_KEY, SH7750_RTCSR);

    /* Initialize Memory Control Register; disable refresh */
    write32((MCRDEF & ~SH7750_MCR_RFSH) | SH7750_MCR_PALL, SH7750_MCR);

    /* SDRAM power-up initialization require 100 microseconds delay after
       stable power and clock fed; 100 microseconds corresponds to 7 refresh
       intervals */
    while (read16(SH7750_RFCR) <= 7);

    /* Clear refresh timer counter */
    write16(SH7750_RTCNT_KEY | 0, SH7750_RTCNT);

    /* Clear refresh counter */
    write16(SH7750_RFCR_KEY | 0, SH7750_RFCR);

    /* Execute Precharge All command */
    write32(0, SH7750_SDRAM_MODE_A2_32BIT(0));

    /* Initialize Memory Control Register; enable refresh, prepare to
       SDRAM mode register setting */
    write32(MCRDEF | SH7750_MCR_MRSET, SH7750_MCR);

    /* Wait until at least 2 auto-refresh commands to be executed */
    while (read16(SH7750_RFCR) <= 10);

    /* SDRAM data width is 32 bit (4 bytes), cache line size is 32 bytes,
       therefore burst length is 8 (32 / 4) */
    write8(0,SH7750_SDRAM_MODE_A2_32BIT(
        SDRAM_MODE_BL_8 |
        SDRAM_MODE_BT_SEQ |    /* Only sequential burst mode supported
                                  in SH7750 */
        SDRAM_MODE_CL_2 |      /* CAS latency is 2 */
        SDRAM_MODE_OPC_BRBW)   /* Burst read/burst write */
    );
    /* Bus State Controller initialized now */

    /* Disable DMA controller */
    write32(0, SH7750_DMAOR);

    /* I/O port setup */
    /* Configure all port bits as output - to fasciliate debugging */
    write32(
        SH7750_PCTRA_PBOUT(0)  | SH7750_PCTRA_PBOUT(1) |
        SH7750_PCTRA_PBOUT(2)  | SH7750_PCTRA_PBOUT(3) |
        SH7750_PCTRA_PBOUT(4)  | SH7750_PCTRA_PBOUT(5) |
        SH7750_PCTRA_PBOUT(6)  | SH7750_PCTRA_PBOUT(7) |
        SH7750_PCTRA_PBOUT(8)  | SH7750_PCTRA_PBOUT(9) |
        SH7750_PCTRA_PBOUT(10) | SH7750_PCTRA_PBOUT(11) |
        SH7750_PCTRA_PBOUT(12) | SH7750_PCTRA_PBOUT(13) |
        SH7750_PCTRA_PBOUT(14) | SH7750_PCTRA_PBOUT(15),
        SH7750_PCTRA);
    write32(
        SH7750_PCTRB_PBOUT(16) | SH7750_PCTRB_PBOUT(17) |
        SH7750_PCTRB_PBOUT(18) | SH7750_PCTRB_PBOUT(19),
        SH7750_PCTRB);
    /* Clear data in port */
    write32(0, SH7750_PDTRA);
    write32(0, SH7750_PDTRB);

    /* Interrupt Controller Initialization */
    write16(SH7750_ICR_IRLM, SH7750_ICR); /* IRLs serves as an independent
                                             interrupt request lines */
    /* Mask all requests at this time */
    write16(
        (0 << SH7750_IPRA_TMU0_S) |
        (0 << SH7750_IPRA_TMU1_S) |
        (0 << SH7750_IPRA_TMU2_S) |
        (0 << SH7750_IPRA_RTC_S),
        SH7750_IPRA);
    write16(
        (0 << SH7750_IPRB_WDT_S) |
        (0 << SH7750_IPRB_REF_S) |
        (0 << SH7750_IPRB_SCI1_S),
        SH7750_IPRB);
    write16(
        (0 << SH7750_IPRC_GPIO_S) |
        (0 << SH7750_IPRC_DMAC_S) |
        (0 << SH7750_IPRC_SCIF_S) |
        (0 << SH7750_IPRC_HUDI_S),
        SH7750_IPRC);

}

/*
 * cache_on --
 *      Enable instruction and operand caches
 */
void bsp_cache_on(void)
{
    switch (boot_mode)
    {
        case SH4_BOOT_MODE_FLASH:
            write32(SH7750_CCR_ICI | SH7750_CCR_ICE |
                    SH7750_CCR_OCI | SH7750_CCR_CB | SH7750_CCR_OCE,
                    SH7750_CCR);
            break;
        case SH4_BOOT_MODE_IPL:
            __asm__ volatile (
                "mov     #6, r0\n"
                "xor     r4, r4\n"
                "trapa   #0x3f\n"
                : : : "r0", "r4");
            break;
        default: /* unreachable */
            break;
    }
}
