/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/smpimpl.h>

#include <bsp/start.h>

#include <bsp/socal/alt_rstmgr.h>
#include <bsp/socal/alt_sysmgr.h>
#include <bsp/socal/hps.h>
#include <bsp/socal/socal.h>

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  bool started;

  if (cpu_index == 1) {
    alt_write_word(
      ALT_SYSMGR_ROMCODE_ADDR + ALT_SYSMGR_ROMCODE_CPU1STARTADDR_OFST,
      ALT_SYSMGR_ROMCODE_CPU1STARTADDR_VALUE_SET((uint32_t) _start)
    );

    alt_clrbits_word(
      ALT_RSTMGR_MPUMODRST_ADDR,
      ALT_RSTMGR_MPUMODRST_CPU1_SET_MSK
    );

    /*
     * Wait for secondary processor to complete its basic initialization so
     * that we can enable the unified L2 cache.
     */
    started = _Per_CPU_State_wait_for_non_initial_state(cpu_index, 0);
  } else {
    started = false;
  }

  return started;
}
