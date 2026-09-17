/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME3100
 *
 * @brief MVME3100 Miscellaneous Support
 *
 * Miscellaneous small BSP routines; reboot, board CSR, ...
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

#include <bsp.h>
#include <bsp/bootcard.h>


void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
uint8_t v;

	(void) source;
	(void) code;

	/*
	 * AFAIK, the hardest reset available; cleared
	 * some errors a VME-bus reset wouldn't (hung
	 * i2c bus)...
	 */
	v  = in_8( BSP_MVME3100_SYS_CR );
	v &= ~BSP_MVME3100_SYS_CR_RESET_MSK;
	v |=  BSP_MVME3100_SYS_CR_RESET;
	out_8( BSP_MVME3100_SYS_CR, v );
	RTEMS_UNREACHABLE();
}

uint8_t
BSP_setSysReg(volatile uint8_t *r, uint8_t mask)
{
uint8_t               v;
rtems_interrupt_level l;

	if ( !mask )
		return in_8( r );

	rtems_interrupt_disable(l);
		v = in_8( r );
		if ( mask ) {
			out_8( r,  v | mask );
		}
	rtems_interrupt_enable(l);
	return v;
}

uint8_t
BSP_clrSysReg(volatile uint8_t *r, uint8_t mask)
{
uint8_t               v;
rtems_interrupt_level l;

	if ( !mask )
		return in_8( r );

	rtems_interrupt_disable(l);
		v = in_8( r );
		if ( mask ) {
			out_8( r,  v & ~mask );
		}
	rtems_interrupt_enable(l);
	return v;
}

uint8_t
BSP_setLEDs(uint8_t mask)
{
	return BSP_setSysReg( BSP_MVME3100_SYS_IND_REG, mask );
}

uint8_t
BSP_clrLEDs(uint8_t mask)
{
	return BSP_clrSysReg( BSP_MVME3100_SYS_IND_REG, mask );
}

uint8_t
BSP_eeprom_write_protect(void)
{
uint8_t           m = BSP_MVME3100_SYS_CR_EEPROM_WP;
volatile uint8_t *r = BSP_MVME3100_SYS_CR;

	return m & BSP_setSysReg( r, m );
}

uint8_t
BSP_eeprom_write_enable(void)
{
uint8_t           m = BSP_MVME3100_SYS_CR_EEPROM_WP;
volatile uint8_t *r = BSP_MVME3100_SYS_CR;

	return m & BSP_clrSysReg( r, m );
}
