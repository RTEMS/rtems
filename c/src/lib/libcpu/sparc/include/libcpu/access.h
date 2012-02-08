/*
 *  access.h  - access routines for SPARC. SPARC is big endian only.
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _LIBCPU_ACCESS_H
#define _LIBCPU_ACCESS_H

#include <rtems/system.h>
#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/* "Raw" access */
extern uint8_t _ld8(uint8_t *addr);
extern void _st8(uint8_t *addr, uint8_t val);
extern uint16_t _ld16(uint16_t *addr);
extern void _st16(uint16_t *addr, uint16_t val);
extern uint32_t _ld32(uint32_t *addr);
extern void _st32(uint32_t *addr, uint32_t val);
extern uint64_t _ld64(uint64_t *addr);
extern void _st64(uint64_t *addr, uint64_t val);

/* Aliases for Big Endian */
extern uint16_t _ld_be16(uint16_t *addr);
extern void _st_be16(uint16_t *addr, uint16_t val);
extern uint32_t _ld_be32(uint32_t *addr);
extern void _st_be32(uint32_t *addr, uint32_t val);
extern uint64_t _ld_be64(uint64_t *addr);
extern void _st_be64(uint64_t *addr, uint64_t val);

/* Little endian */
extern uint16_t _ld_le16(uint16_t *addr);
extern void _st_le16(uint16_t *addr, uint16_t val);
extern uint32_t _ld_le32(uint32_t *addr);
extern void _st_le32(uint32_t *addr, uint32_t val);

#ifdef __cplusplus
}
#endif

#endif
