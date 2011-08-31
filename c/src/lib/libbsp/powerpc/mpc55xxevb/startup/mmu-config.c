/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief MPC55XX MMU configuration.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <bsp/mpc55xx-config.h>
#include <bsp/start.h>
#include <bsp.h>

const BSP_START_TEXT_SECTION struct MMU_tag
mpc55xx_mmu_config [] = {
#if defined(MPC55XX_BOARD_GWLCFM)
  /* External Ethernet Controller 64k */
  MPC55XX_MMU_TAG_INITIALIZER(5, 0x3fff8000, 6, 0, 1, 1, 1)
#elif defined(MPC55XX_BOARD_PHYCORE_MPC5554)
    /* XXX I'm not using TLB1 entry 2 the same way as 
	 * in the BAM.
     */
    /*  Set up MMU TLB1 entry 2 for external ram. */
    /*  Effective Base address = 0x2100_0000 XXX NOT LIKE BAM */
    /*       Real Base address = 0x2100_0000 XXX NOT LIKE BAM */
    /*  Page Size            6 =  4MB XXX Not like BAM */
    /*  Not Guarded, Cache Enable, All Access (0, 3F) */
    {
        { .R = 0x10020000},     /* MAS0 */
        { .R = 0xC0000600},     /* MAS1 */
        { .R = 0x21000000},     /* MAS2 */
        { .R = 0x2100003F}      /* MAS3 */
    },

    /*  Set up MMU TLB1 entry 5 for second half of SRAM (debug RAM) */
    /*  Effective Base address = 0x2140_0000 */
    /*       Real Base address = 0x2140_0000 */
    /*  Page Size            6 = 4MB */
    /*  Not Guarded, Cache Enable, All Access (0, 3F) */
    {
        { .R =  0x10050000 },   /* MAS0 */
        { .R =  0xC0000600 },   /* MAS1 */
        { .R =  0x21400000 },   /* MAS2 */
        { .R =  0x2140003F }    /* MAS3 */
    },
    /*  Set up MMU TLB1 entry 6 for External LAN91C111 */
    /*  Effective Base address = 0x2200_0000 */
    /*       Real Base address = 0x2200_0000 */
    /*  Page Size            7 = 16MB */
    /*  Write-through, Guarded, Cache Inhibit, All Access (E, 3F) */
    {
        { .R = 0x10060000},     /* MAS0 */
        { .R = 0xC0000700},     /* MAS1 */
        { .R = 0x2200000E},     /* MAS2 */
        { .R = 0x2200003F}      /* MAS3 */
    },

    /*  Set up MMU TLB1 entry 7 for External FPGA */
    /*  Effective Base address = 0x2300_0000 */
    /*       Real Base address = 0x2300_0000 */
    /*  Page Size            7 = 16MB */
    /*  Write-through, Guarded, Cache Inhibit, All Access (E, 3F) */
    {
        { .R = 0x10070000},     /* MAS0 */
        { .R = 0xC0000700},     /* MAS1 */
        { .R = 0x2300000E},     /* MAS2 */
        { .R = 0x2300003F},     /* MAS3 */
    },

	/* Should also set up maps for the debug RAM and the
	 * external flash.
	 */
#elif defined(MPC55XX_BOARD_MPC5566EVB)
  /* Internal flash 3M */
  MPC55XX_MMU_TAG_INITIALIZER(1, 0x00000000, 6, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(5, 0x00010000, 6, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(6, 0x00020000, 6, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(7, 0x00030000, 6, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(8, 0x00040000, 8, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(9, 0x00080000, 8, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(10, 0x000c0000, 8, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(11, 0x00100000, 10, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(12, 0x00200000, 10, 1, 0, 1, 0),
  /* External SRAM 512k */
  MPC55XX_MMU_TAG_INITIALIZER(2, 0x20000000, 8, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(13, 0x20040000, 8, 0, 1, 1, 0),
  /* Internal SRAM 128k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40010000, 6, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(14, 0x40000000, 6, 0, 1, 1, 0),
  /* External Ethernet Controller 64k */
  MPC55XX_MMU_TAG_INITIALIZER(15, 0x3fff8000, 6, 0, 1, 1, 1)
#elif defined(MPC55XX_BOARD_MPC5674FEVB)
  /* Internal flash 4M */
  MPC55XX_MMU_TAG_INITIALIZER(1, 0x00000000, 6, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(5, 0x00010000, 6, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(6, 0x00020000, 7, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(7, 0x00040000, 8, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(8, 0x00080000, 9, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(9, 0x00100000, 10, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(10, 0x00200000, 11, 1, 0, 1, 0),
  /* External SRAM 512k */
  MPC55XX_MMU_TAG_INITIALIZER(2, 0x20000000, 9, 0, 1, 1, 0),
  /* Internal SRAM 256k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, 8, 0, 1, 1, 0)
#endif
};

const BSP_START_TEXT_SECTION size_t mpc55xx_mmu_config_count [] = {
  sizeof(mpc55xx_mmu_config) / sizeof(mpc55xx_mmu_config [0])
};
