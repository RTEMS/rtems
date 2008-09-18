/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  Called by RTEMS::RTEMS constructor in startup-ctor.cc
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <bsp.h>
#include <rtems/libcsupport.h>

/*
 * Amount to increment itimer by each pass
 * It is a variable instead of a #define to allow the 'looptest'
 * script to bump it without recompiling rtems
 */

uint32_t         CPU_CLICKS_PER_TICK;

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start(void)
{
  cpu_number = 0;

  #if defined(RTEMS_MULTIPROCESSING)
    /*
     *  If the node number is -1 then the application better provide
     *  it through environment variables RTEMS_NODE.
     *  Ditto for RTEMS_MAXIMUM_NODES
     */
    if (Configuration.User_multiprocessing_table) {
      char *p;

      if (Configuration.User_multiprocessing_table->node == -1) {
        p = getenv("RTEMS_NODE");
        Configuration.User_multiprocessing_table->node = p ? atoi(p) : 1;
      }

      /* If needed provide maximum_nodes also */
      if (Configuration.User_multiprocessing_table->maximum_nodes == -1) {
        p = getenv("RTEMS_MAXIMUM_NODES");
       Configuration.User_multiprocessing_table->maximum_nodes = p ? atoi(p) : 1;
      }
    }

    /*
     * Set cpu_number to accurately reflect our cpu number
     */
      if (Configuration.User_multiprocessing_table)
        cpu_number = Configuration.User_multiprocessing_table->node - 1;
   #endif

    CPU_CLICKS_PER_TICK = 1;
}
