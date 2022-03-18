/* SPDX-License-Identifier: BSD-2-Clause */

/*  PCI Access Library
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
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

#include <pci.h>

/* Get PCI I/O or Configuration space access function */
static int pci_ioc_func(int wr, int size, void **func, void **ops)
{
	int ofs;

	ofs = 0;
	if (wr)
		ofs += 3;
	if (size == 4)
		size = 3;
	ofs += (size & 0x3) - 1;
	if (ops[ofs] == NULL)
		return -1;
	if (func)
		*func = ops[ofs];
	return 0;
}

/* Get Registers-over-Memory Space access function */
static int pci_memreg_func(int wr, int size, void **func, int endian)
{
	void **ops;
	int ofs = 0;

	ops = (void **)pci_access_ops.memreg;
	if (!ops)
		return -1;

	if (size == 2)
		ofs += 2;
	else if (size == 4)
		ofs += 6;

	if (size != 1 && endian == PCI_BIG_ENDIAN)
		ofs += 2;

	if (wr)
		ofs += 1;

	if (ops[ofs] == NULL)
		return -1;
	if (func)
		*func = ops[ofs];
	return 0;
}

/* Get function pointer from Host/BSP driver definitions */
int pci_access_func(int wr, int size, void **func, int endian, int type)
{
	switch (type) {
	default:
	case 2: /* Memory Space - not implemented */
		return -1;
	case 1: /* I/O space */
		return pci_ioc_func(wr, size, func, (void**)&pci_access_ops.cfg);
	case 3: /* Registers over Memory space */
		return pci_memreg_func(wr, size, func, endian);
	case 4: /* Configuration space */
		return pci_ioc_func(wr, size, func, (void**)&pci_access_ops.io);
	}
}
