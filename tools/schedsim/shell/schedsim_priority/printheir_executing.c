/*
 *  printheir_executing
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdio.h>
#include <rtems.h>

void PRINT_EXECUTING() {
  printf(
    "  Thread Executing: 0x%08x priority=%ld\n",
    _Thread_Executing->Object.id,
    (long) _Thread_Executing->current_priority
  );
}

void PRINT_HEIR() {
  printf( 
    "  Thread Heir: 0x%08x priority=%ld\n", 
    _Thread_Heir->Object.id, 
    (long) _Thread_Heir->current_priority 
  );
}
