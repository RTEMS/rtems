/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief EBI chip-select configuration.
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
 */

#include <bsp/mpc55xx-config.h>
#include <bsp/start.h>
#include <bsp.h>

#ifdef MPC55XX_HAS_EBI

BSP_START_TEXT_SECTION const struct EBI_CS_tag
  mpc55xx_start_config_ebi_cs [] = {
#if defined(MPC55XX_BOARD_GWLCFM)
	/* CS0: External SRAM (16 bit, 1 wait states, 512kB, no burst) */
	{
        {
            .B.BA = 0x20000000>>15,
            .B.PS = 1,
            .B.AD_MUX = 1,
            .B.WEBS = 1,
            .B.TBDIP = 0,
            .B.BI = 1,
            .B.V = 1
        },
        {
            .B.AM = 0x1fff0,
            .B.SCY = 1,
            .B.BSCY = 0
        }
    },
	/* CS1: External USB controller (16 bit, 3 wait states, 32kB, no burst) */
	{
        {
            .B.BA = 0x22000000>>15,
            .B.PS = 1,
            .B.AD_MUX = 1,
            .B.WEBS = 0,
            .B.TBDIP = 0,
            .B.BI = 1,
            .B.V = 1
        },
        {
            .B.AM = 0x1ffff,
            .B.SCY = 3,
            .B.BSCY = 0
        }
    },
	/* CS2: Ethernet (16 bit, 2 wait states, 32kB, no burst) */
	{
        {
            .B.BA = 0x22800000>>15,
            .B.PS = 1,
            .B.AD_MUX = 1,
            .B.WEBS = 1,
            .B.TBDIP = 0,
            .B.BI = 1,
            .B.V = 1
        },
        {
            .B.AM = 0x1ffff,
            .B.SCY = 1,
            .B.BSCY = 0
        }
    },
    {				/* CS3: MOST Companion. */
        {
            .B.BA = 0x23000000>>15,
            .B.PS = 1,
            .B.AD_MUX = 1,
            .B.WEBS = 0,
            .B.TBDIP = 0,
            .B.BI = 1,
            .B.V = 1
        },

        {
            .B.AM = 0x1fff0,
            .B.SCY = 1,
            .B.BSCY = 0
        }
    }
#elif defined(MPC55XX_BOARD_PHYCORE_MPC5554)
    /* CS0: External flash. */
    {
        { .R = 0x20000003 },   /* Base 0x2000000, Burst Inhibit, Valid */
        { .R = 0xff000050 }
    },
    /* CS1: External synchronous burst mode SRAM. */
    {
        { .R = 0x21000051 },   /* Base 0x2100000, 4-word Burst Enabled, Valid */
        { .R = 0xff000000 }    /* No wait states. */
    },
    /* CS2: External LAN91C111 */
    {
        { .R = 0x22000003 },   /* Base 0x22000000, Burst inhibit, valid */
        { .R = 0xff000010 }
    },

    /* CS3: External FPGA */
    {
        { .R = 0x23000003 },   /* Base 0x23000000, Burst inhibit, valid. */
        { .R = 0xff000020 }
    }
#elif defined(MPC55XX_BOARD_MPC5566EVB)
	/* CS0: External SRAM (2 wait states, 512kB, 4 word burst) */
    {
        {
            .B.BA = 0,
            .B.PS = 1,
            .B.BL = 1,
            .B.WEBS = 0,
            .B.TBDIP = 0,
            .B.BI = 1, /* TODO: Enable burst */
            .B.V = 1
        },

        {
            .B.AM = 0x1fff0,
            .B.SCY = 0,
            .B.BSCY = 0
        }
    },
    { { .R = 0 }, { .R = 0 } },   /* CS1: Unused. */
    { { .R = 0 }, { .R = 0 } },   /* CS2: Unused. */
    {   /* CS3: ethernet? */
        {
            .B.BA = 0x7fff,
            .B.PS = 1,
            .B.BL = 0,
            .B.WEBS = 0,
            .B.TBDIP = 0,
            .B.BI = 1,
            .B.V = 1
        },

        {
            .B.AM = 0x1ffff,
            .B.SCY = 1,
            .B.BSCY = 0
        }
    }
#endif
};

BSP_START_TEXT_SECTION const size_t mpc55xx_start_config_ebi_cs_count [] = {
  sizeof(mpc55xx_start_config_ebi_cs) / sizeof(mpc55xx_start_config_ebi_cs [0])
};

#endif /* MPC55XX_HAS_EBI */
