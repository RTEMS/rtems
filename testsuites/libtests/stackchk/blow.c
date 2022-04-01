/* SPDX-License-Identifier: BSD-2-Clause */

/*  task1.c
 *
 *  This set of three tasks do some simple task switching for about
 *  15 seconds and then call a routine to "blow the stack".
 *
 *  COPYRIGHT (c) 1989-2012.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/stackchk.h>

#include "system.h"

void blow_stack(void)
{
  Thread_Control *executing;
  char *area;
  volatile uintptr_t *low;
  volatile uintptr_t *high;

  executing = _Thread_Get_executing();

  /*
   *  Destroy the first and last 4 words of our stack area... Hope it
   *  does not cause problems :)
   */

  area = (char *) executing->Start.Initial_stack.area;
  low  = (uintptr_t *) area;
  high = (uintptr_t *)
    (area + executing->Start.Initial_stack.size - 4 * sizeof(*high));

  low[0] = 0x11111111;
  low[1] = 0x22222222;
  low[2] = 0x33333333;
  low[3] = 0x44444444;

  high[0] = 0x55555555;
  high[1] = 0x66666666;
  high[2] = 0x77777777;
  high[3] = 0x88888888;

  rtems_stack_checker_report_usage();
}
