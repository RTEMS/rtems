/* SPDX-License-Identifier: BSD-2-Clause */

/*  AHBSTAT driver interface
 *
 *  COPYRIGHT (c) 2011.
 *  Cobham Gaisler AB.
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

#ifndef __AHBSTAT_H__
#define __AHBSTAT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* AHBSTAT Registers layout */
struct ahbstat_regs {
	volatile uint32_t status;
	volatile uint32_t failing;
	volatile uint32_t status2;
	volatile uint32_t failing2;
};

/* AHB fail interrupt callback to user. This function is declared weak so that
 * the user can define a function pointer variable containing the address
 * responsible for handling errors
 *
 * minor              Index of AHBSTAT hardware
 * regs               Register address of AHBSTAT
 * status             AHBSTAT status register at IRQ
 * failing_address    AHBSTAT Failing address register at IRQ
 *
 * * User return 
 *  0: print error onto terminal with printk and reenable AHBSTAT
 *  1: just re-enable AHBSTAT
 *  2: just print error
 *  3: do nothing, let user do custom handling
 */
extern int (*ahbstat_error)(
	int minor,
	struct ahbstat_regs *regs,
	uint32_t status,
	uint32_t failing_address);

/* Get Last received AHB Error
 *
 * \param minor    Index used to indentify a specific AHBSTAT core
 * \param status   Status register at time of error IRQ was recevied
 * \param address  Failing address register at time of error IRQ
 *
 * Return
 *   0: No error received
 *   1: Error Received, last status and address stored into argument pointers
 *  -1: No such AHBSTAT device
 */
extern int ahbstat_last_error(int minor, uint32_t *status, uint32_t *address);

/* Get AHBSTAT registers address from minor. Can also be used to check if
 * AHBSTAT hardware is present.
 *
 * Return 
 *   NULL       returned if no such device
 *   non-zero   Address to AHBSTAT register
 */
extern struct ahbstat_regs *ahbstat_get_regs(int minor);

/* Registers the AHBSTAT driver to the Driver Manager */
void ahbstat_register_drv (void);

#ifdef __cplusplus
}
#endif

#endif
