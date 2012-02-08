/*
 *  Little-endian access routines for SPARC
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/byteorder.h>

uint16_t _ld_le16(uint16_t *addr)
{
	return ld_le16(addr);
}

void _st_le16(uint16_t *addr, uint16_t val)
{
	st_le16(addr, val);
}

uint32_t _ld_le32(uint32_t *addr)
{
	return ld_le32(addr);
}

void _st_le32(uint32_t *addr, uint32_t val)
{
	st_le32(addr, val);
}
