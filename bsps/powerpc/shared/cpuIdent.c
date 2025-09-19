/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  cpuIdent.c -- Cpu identification code
 */

/*
 * Copyright (C) 2000 Andy Dachs <a.dachs@sstl.co.uk>
 * Copyright (C) 1999 Eric Valette <eric.valette@free.fr>
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

#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>
#include <rtems/bspIo.h>

/*
 * Generate inline code to read Processor Version Register
 */
SPR_RO(PPC_PVR)

ppc_cpu_id_t       current_ppc_cpu      = PPC_UNKNOWN;
ppc_cpu_revision_t current_ppc_revision = 0xff;
ppc_feature_t      current_ppc_features;

const char *get_ppc_cpu_type_name(ppc_cpu_id_t cpu)
{
  switch (cpu) {
    case PPC_405:		return "PPC405";
	case PPC_405GP:		return "PPC405GP";
	case PPC_405EX:		return "PPC405EX";
    case PPC_440:               return "PPC440";
    case PPC_601:		return "MPC601";
    case PPC_5XX:		return "MPC5XX";
    case PPC_603:		return "MPC603";
    case PPC_603ev:		return "MPC603ev";
    case PPC_604:		return "MPC604";
    case PPC_750:		return "MPC750";
    case PPC_750_IBM:		return "IBM PPC750";
    case PPC_7400:		return "MPC7400";
    case PPC_7455:		return "MPC7455";
    case PPC_7457:              return "MPC7457";
    case PPC_603le:		return "MPC603le";
    case PPC_604e:		return "MPC604e";
    case PPC_604r:		return "MPC604r";
    case PPC_620:		return "MPC620";
    case PPC_860:		return "MPC860";
    case PPC_8260:		return "MPC8260";
    case PPC_8245:		return "MPC8245";
    case PPC_8540:		return "MPC8540";
    case PPC_PSIM:		return "PSIM";
    case PPC_e200z0:		return "e200z0";
    case PPC_e200z1:		return "e200z1";
    case PPC_e200z4:		return "e200z4";
    case PPC_e200z6:		return "e200z6";
    case PPC_e200z7:		return "e200z7";
    case PPC_e500v2:		return "e500v2";
    case PPC_e6500:		return "e6500";
    default:
      printk("Unknown CPU value of 0x%x. Please add it to "
             "<libcpu/powerpc/shared/include/cpuIdent.c>\n", cpu );
  }
  return "UNKNOWN";
}

ppc_cpu_id_t get_ppc_cpu_type(void)
{
  /*
   * cpu types listed here have the lowermost nibble as a version identifier
   * we will tweak them to the standard version
   */
  const uint32_t ppc_cpu_id_version_nibble[] = {
    PPC_e200z0,
    PPC_e200z1,
    PPC_e200z4,
    PPC_e200z6,
    PPC_e200z7
  };

  unsigned int pvr;
  int i;

  if ( PPC_UNKNOWN != current_ppc_cpu )
  	return current_ppc_cpu;

  pvr = (_read_PPC_PVR() >> 16);
  /*
   * apply tweaks to ignore version
   */
  for (i = 0;
       i < (sizeof(ppc_cpu_id_version_nibble)
	    /sizeof(ppc_cpu_id_version_nibble[0]));
       i++) {
    if ((pvr & 0xfff0) == (ppc_cpu_id_version_nibble[i] & 0xfff0)) {
      pvr = ppc_cpu_id_version_nibble[i];
      break;
    }
  }

  current_ppc_cpu = (ppc_cpu_id_t) pvr;

  switch (pvr) {
    case PPC_405:
    case PPC_405GP:
    case PPC_405EX:
    case PPC_440:
    case PPC_601:
    case PPC_5XX:
    case PPC_603:
    case PPC_603ev:
    case PPC_603le:
    case PPC_604:
    case PPC_604r:
    case PPC_750:
    case PPC_750_IBM:
    case PPC_7400:
    case PPC_7455:
    case PPC_7457:
    case PPC_604e:
    case PPC_620:
    case PPC_860:
    case PPC_8260:
    case PPC_8245:
    case PPC_PSIM:
    case PPC_8540:
    case PPC_e200z0:
    case PPC_e200z1:
    case PPC_e200z4:
    case PPC_e200z6:
    case PPC_e200z7:
    case PPC_e300c1:
    case PPC_e300c2:
    case PPC_e300c3:
    case PPC_e500v2:
    case PPC_e6500:
      break;
    default:
      printk("Unknown PVR value of 0x%x. Please add it to "
             "<libcpu/powerpc/shared/include/cpuIdent.c>\n", pvr );
    return PPC_UNKNOWN;
  }

  /* determine features */

  /* FIXME: This is incomplete; I couldn't go through all the
   * manuals (yet).
   */
  switch ( current_ppc_cpu ) {
    case PPC_7455:
    case PPC_7457:
		current_ppc_features.has_8_bats			= 1;
		RTEMS_FALL_THROUGH();
    case PPC_7400:
	/* NOTE: PSIM PVR doesn't tell us anything (its
     *       contents are not set based on what model
       *       the user chooses but has to be programmed via
	 *       the device file with the special value 0xfffe
	 *       telling us that we have a 'psim cpu').
	 *
	 *       I'm not sure pagetables work if the user chooses
	 *       e.g., the 603 model...
	 */
	case PPC_PSIM:
		current_ppc_features.has_altivec		= 1;
		RTEMS_FALL_THROUGH();
	case PPC_604:
	case PPC_604e:
	case PPC_604r:
	case PPC_750:
	case PPC_750_IBM:
		current_ppc_features.has_hw_ptbl_lkup	= 1;
		RTEMS_FALL_THROUGH();
	case PPC_8260:
	case PPC_8245:
	case PPC_601:
	case PPC_603:
	case PPC_603e:
	case PPC_603ev:
	case PPC_603le:
		current_ppc_features.is_60x				= 1;
		RTEMS_FALL_THROUGH();
	default:
	break;
  }

  switch ( current_ppc_cpu ) {
    case PPC_e6500:
      current_ppc_features.has_altivec = 1;
      break;
    default:
      break;
  }

  switch ( current_ppc_cpu ) {
  	case PPC_405:
 	case PPC_405GP:
 	case PPC_405EX:
		current_ppc_features.is_bookE			= PPC_BOOKE_405;
	break;
    case PPC_440:
      current_ppc_features.is_bookE          = PPC_BOOKE_STD;
      break;
  	case PPC_8540:
  	case PPC_e200z0:
  	case PPC_e200z1:
  	case PPC_e200z4:
  	case PPC_e200z6:
  	case PPC_e200z7:
  	case PPC_e500v2:
  	case PPC_e6500:
		current_ppc_features.is_bookE			= PPC_BOOKE_E500;
		break;
	default:
	break;
  }

  switch ( current_ppc_cpu ) {
  	case PPC_860:
		current_ppc_features.has_16byte_clne	= 1;
	default:
	break;
  }

  switch ( current_ppc_cpu ) {
    case PPC_603e:
    case PPC_603ev:
    case PPC_603le:
    case PPC_e300c1:
    case PPC_e300c2:
    case PPC_e300c3:
    case PPC_8240:
		current_ppc_features.has_shadowed_gprs  = 1;
	default:
	break;
  }

  return current_ppc_cpu;
}

ppc_cpu_revision_t get_ppc_cpu_revision(void)
{
  ppc_cpu_revision_t rev = (ppc_cpu_revision_t) (_read_PPC_PVR() & 0xffff);
  current_ppc_revision = rev;
  return rev;
}
