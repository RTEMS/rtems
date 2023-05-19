/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief EBI chip-select configuration.
 */

/*
 * Copyright (C) 2008, 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp/mpc55xx-config.h>

#ifdef MPC55XX_HAS_EBI

const struct EBI_CS_tag mpc55xx_start_config_ebi_cs [] = {
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

const size_t mpc55xx_start_config_ebi_cs_count [] = {
  RTEMS_ARRAY_SIZE(mpc55xx_start_config_ebi_cs)
};

#endif /* MPC55XX_HAS_EBI */
