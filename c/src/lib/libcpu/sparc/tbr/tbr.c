/*
 *  SPARC Dependent Source
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <libcpu/tbr.h>

void
sparc_init_tbr ()
{
#ifndef NO_TABLE_MOVE
  unsigned32 trap_table_start;
  unsigned32 tbr_value;
  CPU_Trap_table_entry *old_tbr;
  CPU_Trap_table_entry *trap_table;

  /*
   *  Install the executive's trap table.  All entries from the original
   *  trap table are copied into the executive's trap table.  This is essential
   *  since this preserves critical trap handlers such as the window underflow 
   *  and overflow handlers.  It is the responsibility of the BSP to provide   
   *  install these in the initial trap table.
   */


  trap_table_start = (unsigned32) & _CPU_Trap_Table_area;
  if (trap_table_start & (SPARC_TRAP_TABLE_ALIGNMENT - 1))
    trap_table_start = (trap_table_start + SPARC_TRAP_TABLE_ALIGNMENT) &
      ~(SPARC_TRAP_TABLE_ALIGNMENT - 1);

  trap_table = (CPU_Trap_table_entry *) trap_table_start;

  sparc_get_tbr (tbr_value);

  old_tbr = (CPU_Trap_table_entry *) (tbr_value & 0xfffff000);

  memcpy (trap_table, (void *) old_tbr, 256 * sizeof (CPU_Trap_table_entry));

  sparc_set_tbr (trap_table_start);

#endif
}
