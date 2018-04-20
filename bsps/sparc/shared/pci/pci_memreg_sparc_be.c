/*  Registers-over-Memory Space - SPARC Big endian PCI bus definitions
 *
 *  COPYRIGHT (c) 2011.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci.h>
#include <libcpu/access.h>

struct pci_memreg_ops pci_memreg_sparc_be_ops = {
	.ld8    = _ld8,
	.st8    = _st8,

	.ld_le16 = _ld_be16,
	.st_le16 = _st_be16,
	.ld_be16 = _ld_le16,
	.st_be16 = _st_le16,

	.ld_le32 = _ld_be32,
	.st_le32 = _st_be32,
	.ld_be32 = _ld_le32,
	.st_be32 = _st_le32,
};
