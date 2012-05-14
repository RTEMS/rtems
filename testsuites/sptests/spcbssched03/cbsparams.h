/*  cbsparams.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* This file was generated with the following parameters:
-T 30,10,0 -T 40,10,0 -T 50,10,0 -T 70,10,0 -A 1,100,7 -A 100,5,4
*/

#ifndef __CBSPARAMS_H_
#define __CBSPARAMS_H_

#include "system.h"

rtems_task_priority Priorities[1+NUM_TASKS]= { 0, 30, 40, 50, 70, 254, 254 };

uint32_t  Periods[1+NUM_PERIODIC_TASKS]    = { 0, 30, 40, 50, 70 };

uint32_t  Execution[1+NUM_TASKS]           = { 0, 10, 10, 10, 10, 100, 5 };

uint32_t  Phases[1+NUM_TASKS]              = { 0, 0, 0, 0, 0, 7, 4 };

#define build_task_name() do { \
Task_name[ 1 ] =  rtems_build_name( 'P', 'T', '1', ' ' );\
Task_name[ 2 ] =  rtems_build_name( 'P', 'T', '2', ' ' );\
Task_name[ 3 ] =  rtems_build_name( 'P', 'T', '3', ' ' );\
Task_name[ 4 ] =  rtems_build_name( 'P', 'T', '4', ' ' );\
Task_name[ 5 ] =  rtems_build_name( 'A', 'T', '5', ' ' );\
Task_name[ 6 ] =  rtems_build_name( 'A', 'T', '6', ' ' );\
} while(0)

#endif
