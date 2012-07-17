/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief MMU configuration.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
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
 */

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/mpc55xx-config.h>

BSP_START_TEXT_SECTION const struct MMU_tag
  mpc55xx_start_config_mmu [] = {
#if defined(MPC55XX_BOARD_GWLCFM)
  /* External Ethernet Controller 64k */
  MPC55XX_MMU_TAG_INITIALIZER(5, 0x3fff8000, MPC55XX_MMU_64K, 0, 1, 1, 1)
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
  MPC55XX_MMU_TAG_INITIALIZER(1, 0x00000000, MPC55XX_MMU_64K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(6, 0x00010000, MPC55XX_MMU_64K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(7, 0x00020000, MPC55XX_MMU_64K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(8, 0x00030000, MPC55XX_MMU_64K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(9, 0x00040000, MPC55XX_MMU_256K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(10, 0x00080000, MPC55XX_MMU_256K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(11, 0x000c0000, MPC55XX_MMU_256K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(12, 0x00100000, MPC55XX_MMU_1M, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(13, 0x00200000, MPC55XX_MMU_1M, 1, 0, 1, 0),
  /* External SRAM 512k */
  MPC55XX_MMU_TAG_INITIALIZER(2, 0x20000000, MPC55XX_MMU_256K, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(14, 0x20040000, MPC55XX_MMU_256K, 0, 1, 1, 0),
  /* Internal SRAM 128k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_64K, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(5, 0x40010000, MPC55XX_MMU_64K, 0, 1, 1, 0),
  /* External Ethernet Controller 64k */
  MPC55XX_MMU_TAG_INITIALIZER(15, 0x3fff8000, MPC55XX_MMU_64K, 0, 1, 1, 1)
#elif defined(MPC55XX_BOARD_MPC5674FEVB)
  /* Internal flash 4M */
  MPC55XX_MMU_TAG_INITIALIZER(1, 0x00000000, MPC55XX_MMU_64K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(5, 0x00010000, MPC55XX_MMU_64K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(6, 0x00020000, MPC55XX_MMU_128K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(7, 0x00040000, MPC55XX_MMU_256K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(8, 0x00080000, MPC55XX_MMU_512K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(9, 0x00100000, MPC55XX_MMU_1M, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(10, 0x00200000, MPC55XX_MMU_2M, 1, 0, 1, 0),
  /* External SRAM 512k */
  MPC55XX_MMU_TAG_INITIALIZER(2, 0x20000000, MPC55XX_MMU_512K, 0, 1, 1, 0),
  /* Internal SRAM 256k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_128K, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(11, 0x40020000, MPC55XX_MMU_64K, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(12, 0x40030000, MPC55XX_MMU_32K, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(13, 0x40038000, MPC55XX_MMU_16K, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(14, 0x4003c000, MPC55XX_MMU_16K, 0, 1, 1, 1),
  /* External Ethernet controller */
  MPC55XX_MMU_TAG_INITIALIZER(15, 0x3fff8000, MPC55XX_MMU_64K, 0, 1, 1, 1)
#elif defined(MPC55XX_BOARD_MPC5674F_ECU508)
  /* Arguments macro:       idx,  addr,                     size,             x, w, r, io */
  /* Internal flash 4M */
  MPC55XX_MMU_TAG_INITIALIZER(1,  0x00000000,               MPC55XX_MMU_64K,  1, 0, 1, 0),  /* first 64k unused, to detect null-pointer access */
  MPC55XX_MMU_TAG_INITIALIZER(5,  0x00010000,               MPC55XX_MMU_64K,  1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(6,  0x00020000,               MPC55XX_MMU_128K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(7,  0x00040000,               MPC55XX_MMU_256K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(8,  0x00080000,               MPC55XX_MMU_512K, 1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(9,  0x00100000,               MPC55XX_MMU_1M,   1, 0, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(10, 0x00200000,               MPC55XX_MMU_2M,   1, 0, 1, 0),
  /* External SRAM 2M */
  MPC55XX_MMU_TAG_INITIALIZER(2,  0x20000000,               MPC55XX_MMU_2M,   0, 1, 1, 0),
  /* Internal SRAM 256k */
  MPC55XX_MMU_TAG_INITIALIZER(3,  0x40000000 +   0 * 1024,  MPC55XX_MMU_256K, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(11, 0x40000000 + 128 * 1024,  MPC55XX_MMU_64K,  0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(12, 0x40000000 + 192 * 1024,  MPC55XX_MMU_32K,  0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(13, 0x40000000 + 224 * 1024,  MPC55XX_MMU_16K,  0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(14, 0x40000000 + 240 * 1024,  MPC55XX_MMU_16K,  0, 1, 1, 1),  // used as cache-inhibited area (ADC, DSPI queues)
  /* External Ethernet controller */
  MPC55XX_MMU_TAG_INITIALIZER(15, 0x3fff8000,               MPC55XX_MMU_1K,   0, 1, 1, 1),
  /* External MRAM 128k */
  MPC55XX_MMU_TAG_INITIALIZER(16, 0x3ffa0000,               MPC55XX_MMU_128K, 0, 1, 1, 0),
  /* External ARCNET controller */
  MPC55XX_MMU_TAG_INITIALIZER(17, 0x3ffc0000,               MPC55XX_MMU_1K,   0, 1, 1, 1)
  /* Peripheral Bridge A-Registers on MMU-table pos 4 */
  /* Peripheral Bridge B-Registers on MMU-table pos 0 */
#elif MPC55XX_CHIP_TYPE / 10 == 564
  /* Internal flash 1M */
  MPC55XX_MMU_TAG_INITIALIZER(0, 0x00000000, MPC55XX_MMU_1M, 1, 0, 1, 0),
  /* IO */
  MPC55XX_MMU_TAG_INITIALIZER(1, 0xffe00000, MPC55XX_MMU_2M, 0, 1, 1, 1),
  MPC55XX_MMU_TAG_INITIALIZER(2, 0xc3f00000, MPC55XX_MMU_1M, 0, 1, 1, 1),
  /* Internal SRAM 64k + 64k */
  MPC55XX_MMU_TAG_INITIALIZER(3, 0x40000000, MPC55XX_MMU_64K, 0, 1, 1, 0),
  MPC55XX_MMU_TAG_INITIALIZER(4, 0x50000000, MPC55XX_MMU_64K, 0, 1, 1, 0)
#endif
};

BSP_START_TEXT_SECTION const size_t mpc55xx_start_config_mmu_count [] = {
  sizeof(mpc55xx_start_config_mmu) / sizeof(mpc55xx_start_config_mmu [0])
};
