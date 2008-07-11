/*
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  Added MPC8260 Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited
 *
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _LIBCPU_CPUIDENT_H
#define _LIBCPU_CPUIDENT_H

#include <stdbool.h>

#ifndef ASM
typedef enum
{
  PPC_601 = 0x1,
  PPC_5XX = 0x2,
  PPC_603 = 0x3,
  PPC_604 = 0x4,
  PPC_603e = 0x6,
  PPC_603ev = 0x7,
  PPC_750 = 0x8,
  PPC_604e = 0x9,
  PPC_604r = 0xA,
  PPC_7400 = 0xC,
  PPC_405  = 0x2001,
  PPC_7455 = 0x8001, /* Kate Feng */
  PPC_7457 = 0x8002,
  PPC_620 = 0x16,
  PPC_860 = 0x50,
  PPC_821 = PPC_860,
  PPC_823 = PPC_860,
  PPC_8260 = 0x81,
  PPC_8240 = PPC_8260,
  PPC_8245 = 0x8081,
  PPC_8540 = 0x8020,
  PPC_603le = 0x8082, /* 603le core, in MGT5100 and MPC5200 */
  PPC_e300c1  = 0x8083, /* e300c1  core, in MPC83xx*/
  PPC_e300c2  = 0x8084, /* e300c2  core */
  PPC_e300c3  = 0x8085, /* e300c3  core */
  PPC_e200z6 = 0x8115,
  PPC_PSIM = 0xfffe,  /* GDB PowerPC simulator -- fake version */
  PPC_UNKNOWN = 0xffff
} ppc_cpu_id_t;

/* Bitfield of for identifying features or groups of cpu flavors.
 * DO NOT USE DIRECTLY (as implementation may change)
 * only use the 'ppc_is_xxx() / ppc_has_xxx()' macros/inlines
 * below.
 */

typedef struct { 
	unsigned has_altivec		: 1;
	unsigned has_fpu			: 1;
	unsigned has_hw_ptbl_lkup	: 1;
#define PPC_BOOKE_405   1	/* almost like booke but with some significant differences */
#define PPC_BOOKE_STD	2
#define PPC_BOOKE_E500	3	/* bookE with extensions */
	unsigned is_bookE			: 2;
	unsigned has_16byte_clne	: 1;
	unsigned is_60x             : 1;
	unsigned has_8_bats			: 1;
	unsigned has_epic           : 1;
	unsigned has_shadowed_gprs  : 1;
	unsigned has_ivpr_and_ivor  : 1;
} ppc_feature_t;

extern ppc_feature_t   current_ppc_features;
extern ppc_cpu_id_t current_ppc_cpu;

typedef unsigned short ppc_cpu_revision_t;

extern ppc_cpu_id_t get_ppc_cpu_type ();
extern char *get_ppc_cpu_type_name(ppc_cpu_id_t cpu);
extern ppc_cpu_revision_t get_ppc_cpu_revision ();
extern ppc_cpu_revision_t current_ppc_revision;

/* PUBLIC ACCESS ROUTINES */
#define _PPC_FEAT_DECL(x) \
static inline unsigned ppc_cpu_##x() { \
  if ( PPC_UNKNOWN == current_ppc_cpu ) \
    get_ppc_cpu_type(); \
  return current_ppc_features.x; \
}

_PPC_FEAT_DECL(has_altivec)
/* has_fpu not implemented yet */
_PPC_FEAT_DECL(has_hw_ptbl_lkup)
_PPC_FEAT_DECL(is_bookE)
_PPC_FEAT_DECL(is_60x)
_PPC_FEAT_DECL(has_8_bats)
_PPC_FEAT_DECL(has_epic)
_PPC_FEAT_DECL(has_shadowed_gprs)
_PPC_FEAT_DECL(has_ivpr_and_ivor)

static inline bool ppc_cpu_is_e300()
{
	if (current_ppc_cpu == PPC_UNKNOWN) {
		get_ppc_cpu_type();
	}
	return current_ppc_cpu == PPC_e300c1
		|| current_ppc_cpu == PPC_e300c2
		|| current_ppc_cpu == PPC_e300c3;
}

#undef _PPC_FEAT_DECL
#endif /* ASM */

#endif
