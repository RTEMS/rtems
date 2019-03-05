/**
 * @file
 * @ingroup RTEMSBSPsSPARCShared
 * @brief ERC32/LEON2/LEON3 BSP specific exit handler.
 */

/*
 *  COPYRIGHT (c) 2014.
 *  Aeroflex Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/score/cpu.h>

void BSP_fatal_exit(uint32_t error)
{
  sparc_syscall_exit(RTEMS_FATAL_SOURCE_BSP, error);
}
