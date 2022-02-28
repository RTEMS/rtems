/* SPDX-License-Identifier: BSD-2-Clause */

/* Driver Manager Driver Translate Interface Implementation
 *
 * COPYRIGHT (c) 2010 Cobham Gaisler AB.
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

#include <drvmgr/drvmgr.h>

#include <rtems/bspIo.h>

/* Calls drvmgr_translate() to translate an address range and check the result,
 * a printout is generated if the check fails. See paramters of
 * drvmgr_translate().
 * If size=0 only the starting address is not checked.
 */
int drvmgr_translate_check(
	struct drvmgr_dev *dev,
	unsigned int options,
	void *src_address,
	void **dst_address,
	unsigned int size)
{
	unsigned int max;

	max = drvmgr_translate(dev, options, src_address, dst_address);
	if (max == 0 || (max < size && (size != 0))) {
		printk(" ### dev %p (%s) failed mapping %p\n",
			dev, dev->name ? dev->name : "unnamed", src_address);
		return -1;
	}

	return 0;
}
