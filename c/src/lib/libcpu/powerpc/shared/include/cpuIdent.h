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

#ifndef _libcpu_cpuIdent_h
#define _libcpu_cpuIdent_h

#ifndef ASM
typedef enum
{
  PPC_601 = 0x1,
  PPC_603 = 0x3,
  PPC_604 = 0x4,
  PPC_603e = 0x6,
  PPC_603ev = 0x7,
  PPC_750 = 0x8,
  PPC_604e = 0x9,
  PPC_604r = 0xA,
  PPC_7400 = 0xC,
  PPC_620 = 0x16,
  PPC_860 = 0x50,
  PPC_821 = PPC_860,
  PPC_8260 = 0x81,
  PPC_UNKNOWN = 0xff
} ppc_cpu_id_t;

typedef unsigned short ppc_cpu_revision_t;

extern ppc_cpu_id_t get_ppc_cpu_type ();
extern ppc_cpu_id_t current_ppc_cpu;
extern char *get_ppc_cpu_type_name(ppc_cpu_id_t cpu);
extern ppc_cpu_revision_t get_ppc_cpu_revision ();
extern ppc_cpu_revision_t current_ppc_revision;
#endif /* ASM */

#endif
