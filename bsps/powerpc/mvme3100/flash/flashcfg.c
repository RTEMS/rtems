/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME3100
 *
 * @brief BSP-specific bits of flash programmer support.
 */

/*
 * Copyright (C) 2005 - 2007 Till Straumann <strauman@slac.stanford.edu>
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

#include <rtems.h>
#include <bsp.h>
#include <libcpu/spr.h>
#include <stdio.h>

#include <bsp/flashPgmPvt.h>

SPR_RO(TBRL)

#define STATIC	static

static struct bankdesc mvme3100Flash[] = {
	/*
	 * Bank is populated from the top; make max_size negative to
	 * indicate this
	 */
	{ 0xf8000000, 0, - 0x08000000, 2, BSP_flash_vendor_spansion, 0, 0, 0  },
};

STATIC struct bankdesc *
bankcheck(int bank, int quiet)
{
	if ( bank ) {
		if ( !quiet )
			fprintf(stderr,"Invalid flash bank #%i\n",bank);
		return 0;
	}
	return &mvme3100Flash[bank];
}

STATIC int
flash_wp(int bank, int enbl)
{
uint8_t mask = enbl < 0 ? 0 : BSP_MVME3100_FLASH_CSR_F_WP_SW;
uint8_t val;

	if ( bank != 0 ) {
		fprintf(stderr,"Invalid flash bank #%i\n",bank);
		return -1;
	}

	if ( enbl )
		val = BSP_setSysReg( BSP_MVME3100_FLASH_CSR, mask );
	else
		val = BSP_clrSysReg( BSP_MVME3100_FLASH_CSR, mask );

	if ( BSP_MVME3100_FLASH_CSR_F_WP_HW & val ) {
		fprintf(stderr,"Flash: hardware write-protection engaged (switch)\n");
		return -1;
	}
	if ( enbl < 0 )
		return val & (BSP_MVME3100_FLASH_CSR_F_WP_HW | BSP_MVME3100_FLASH_CSR_F_WP_SW );
	return 0;
}

STATIC uint32_t
read_us_timer(void)
{
uint32_t mhz = BSP_bus_frequency/BSP_time_base_divisor/1000;

	return _read_TBRL()/mhz;
}

/* BSP ops (detect banks, handle write-protection on board) */
struct flash_bsp_ops BSP_flashBspOps = {
	.bankcheck       = bankcheck,
	.flash_wp        = flash_wp,
	.read_us_timer   = read_us_timer,
};
