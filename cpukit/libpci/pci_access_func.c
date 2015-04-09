/*  PCI Access Library
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
