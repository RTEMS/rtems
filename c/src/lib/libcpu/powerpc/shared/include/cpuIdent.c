/*
 *  cpuIdent.c -- Cpu identification code
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  Added MPC8260 Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 *
 */

#include <libcpu/spr.h>
#include <bspIo.h>

/*
 * Generate inline code to read Processor Version Register
 */
SPR_RO(PVR)

ppc_cpu_id_t current_ppc_cpu = PPC_UNKNOWN;
ppc_cpu_revision_t current_ppc_revision = 0xff;

ppc_cpu_id_t get_ppc_cpu_type()
{
  unsigned int pvr = (_read_PVR() >> 16);

  current_ppc_cpu = (ppc_cpu_id_t) pvr;
  switch (pvr) {
    case PPC_601:
    case PPC_603:
    case PPC_603ev:
    case PPC_604:
    case PPC_750:
    case PPC_604e:
    case PPC_604r:
    case PPC_620:
    case PPC_860:
    case PPC_8260:
      current_ppc_cpu = (ppc_cpu_id_t) pvr;
      return current_ppc_cpu;
    default:
      printk("Unknown PVR value of 0x%x. Please add it to <libcpu/powerpc/shared/cpu.h>\n", pvr );
    return PPC_UNKNOWN;
  }
  
}
ppc_cpu_revision_t get_ppc_cpu_revision()
{
  ppc_cpu_revision_t rev = (ppc_cpu_revision_t) (_read_PVR() & 0xffff);
  current_ppc_revision = rev;
  return rev;
}
