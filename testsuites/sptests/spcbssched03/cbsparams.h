/* SPDX-License-Identifier: BSD-2-Clause */

/*  cbsparams.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
