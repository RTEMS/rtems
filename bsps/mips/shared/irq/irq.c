/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (C) 2005 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
 *
 * Copyright (C) 1989-2012 On-Line Applications Research Corporation (OAR)
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

#include <inttypes.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <libcpu/isr_entries.h>
#include <rtems/bspIo.h>

static const char *const cause_strings[32] = {
  /*  0 */ "Int",
  /*  1 */ "TLB Mods",
  /*  2 */ "TLB Load",
  /*  3 */ "TLB Store",
  /*  4 */ "Address Load",
  /*  5 */ "Address Store",
  /*  6 */ "Instruction Bus Error",
  /*  7 */ "Data Bus Error",
  /*  8 */ "Syscall",
  /*  9 */ "Breakpoint",
  /* 10 */ "Reserved Instruction",
  /* 11 */ "Coprocessor Unuseable",
  /* 12 */ "Overflow",
  /* 13 */ "Trap",
  /* 14 */ "Instruction Virtual Coherency Error",
  /* 15 */ "FP Exception",
  /* 16 */ "Reserved 16",
  /* 17 */ "Reserved 17",
  /* 18 */ "Reserved 18",
  /* 19 */ "Reserved 19",
  /* 20 */ "Reserved 20",
  /* 21 */ "Reserved 21",
  /* 22 */ "Reserved 22",
  /* 23 */ "Watch",
  /* 24 */ "Reserved 24",
  /* 25 */ "Reserved 25",
  /* 26 */ "Reserved 26",
  /* 27 */ "Reserved 27",
  /* 28 */ "Reserved 28",
  /* 29 */ "Reserved 29",
  /* 30 */ "Reserved 30",
  /* 31 */ "Data Virtual Coherency Error"
};

static inline bool bsp_irq_is_valid(rtems_vector_number vector)
{
  return vector < BSP_INTERRUPT_VECTOR_COUNT;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  (void) vector;
  (void) attributes;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  (void) vector;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t *priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(priority != NULL);
  return RTEMS_UNSATISFIED;
}

void bsp_interrupt_facility_initialize(void)
{
  mips_install_isr_entries();
}

void bsp_interrupt_handler_default(rtems_vector_number vector)
{
  uint32_t sr;
  uint32_t cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  printk( "Unhandled exception %" PRId32 "\n", vector );
  printk( "sr: 0x%08" PRIu32 "  cause: 0x%08" PRIu32 " --> %s\n", sr, cause,
     cause_strings[(cause >> 2) &0x1f] );
  #if 0
    mips_dump_exception_frame( frame );
  #endif
  rtems_fatal_error_occurred(1);
}
