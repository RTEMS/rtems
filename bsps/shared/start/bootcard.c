/**
 * @file
 *
 * @ingroup RTEMSBSPsSharedStartup
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp/bootcard.h>

#include <rtems.h>
#include <rtems/sysinit.h>

/*
 *  At most a single pointer to the cmdline for those target
 *  short on memory and not supporting a command line.
 */
const char *bsp_boot_cmdline;

RTEMS_SYSINIT_ITEM(
  bsp_work_area_initialize,
  RTEMS_SYSINIT_BSP_WORK_AREAS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

RTEMS_SYSINIT_ITEM(
  bsp_start,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

/*
 *  This is the initialization framework routine that weaves together
 *  calls to RTEMS and the BSP in the proper sequence to initialize
 *  the system while maximizing shared code and keeping BSP code in C
 *  as much as possible.
 */
void boot_card(
  const char *cmdline
)
{
  ISR_Level bsp_isr_level;

  /*
   * Make sure interrupts are disabled.  Directly use the CPU port API to allow
   * tracing of the higher level interrupt disable/enable routines, e.g.
   * _ISR_Local_disable() and _ISR_Local_enable().
   */
  _CPU_ISR_Disable( bsp_isr_level );
  (void) bsp_isr_level;

  bsp_boot_cmdline = cmdline;

  rtems_initialize_executive();

  /***************************************************************
   ***************************************************************
   *  APPLICATION RUNS NOW!!!  We will not return to here!!!     *
   ***************************************************************
   ***************************************************************/
}
