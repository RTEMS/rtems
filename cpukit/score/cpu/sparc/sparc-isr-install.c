/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUSPARC
 *
 * @brief This source file contains the SPARC-specific implementation of
 *   _CPU_ISR_install_raw_handler() and _CPU_ISR_install_vector().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/score/isr.h>
#include <rtems/rtems/cache.h>

/*
 *  This initializes the set of opcodes placed in each trap
 *  table entry.  The routine which installs a handler is responsible
 *  for filling in the fields for the _handler address and the _vector
 *  trap type.
 *
 *  The constants following this structure are masks for the fields which
 *  must be filled in when the handler is installed.
 */
const CPU_Trap_table_entry _CPU_Trap_slot_template = {
  0xa1480000,      /* mov   %psr, %l0           */
  0x29000000,      /* sethi %hi(_handler), %l4  */
  0x81c52000,      /* jmp   %l4 + %lo(_handler) */
  0xa6102000       /* mov   _vector, %l3        */
};

/*
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs the specified handler as a "raw" non-executive
 *  supported trap handler (a.k.a. interrupt service routine).
 *
 *  Input Parameters:
 *    vector      - trap table entry number plus synchronous
 *                    vs. asynchronous information
 *    new_handler - address of the handler to be installed
 *    old_handler - pointer to an address of the handler previously installed
 *
 *  Output Parameters: NONE
 *    *new_handler - address of the handler previously installed
 *
 *  NOTE:
 *
 *  On the SPARC, there are really only 256 vectors.  However, the executive
 *  has no easy, fast, reliable way to determine which traps are synchronous
 *  and which are asynchronous.  By default, synchronous traps return to the
 *  instruction which caused the interrupt.  So if you install a software
 *  trap handler as an executive interrupt handler (which is desirable since
 *  RTEMS takes care of window and register issues), then the executive needs
 *  to know that the return address is to the trap rather than the instruction
 *  following the trap.
 *
 *  So vectors 0 through 255 are treated as regular asynchronous traps which
 *  provide the "correct" return address.  Vectors 256 through 512 are assumed
 *  by the executive to be synchronous and to require that the return address
 *  be fudged.
 *
 *  If you use this mechanism to install a trap handler which must reexecute
 *  the instruction which caused the trap, then it should be installed as
 *  an asynchronous trap.  This will avoid the executive changing the return
 *  address.
 */

void _CPU_ISR_install_raw_handler(
  uint32_t             vector,
  CPU_ISR_raw_handler  new_handler,
  CPU_ISR_raw_handler *old_handler
)
{
  uint32_t               real_vector;
  CPU_Trap_table_entry  *tbr;
  CPU_Trap_table_entry  *slot;
  uint32_t               u32_tbr;
  uint32_t               u32_handler;

  /*
   *  Get the "real" trap number for this vector ignoring the synchronous
   *  versus asynchronous indicator included with our vector numbers.
   */

  real_vector = SPARC_REAL_TRAP_NUMBER( vector );

  /*
   *  Get the current base address of the trap table and calculate a pointer
   *  to the slot we are interested in.
   */

  sparc_get_tbr( u32_tbr );

  u32_tbr &= 0xfffff000;

  tbr = (CPU_Trap_table_entry *) u32_tbr;

  slot = &tbr[ real_vector ];

  /*
   *  Get the address of the old_handler from the trap table.
   *
   *  NOTE: The old_handler returned will be bogus if it does not follow
   *        the RTEMS model.
   */

#define HIGH_BITS_MASK   0xFFFFFC00
#define HIGH_BITS_SHIFT  10
#define LOW_BITS_MASK    0x000003FF

  if ( slot->mov_psr_l0 == _CPU_Trap_slot_template.mov_psr_l0 ) {
    u32_handler =
      (slot->sethi_of_handler_to_l4 << HIGH_BITS_SHIFT) |
      (slot->jmp_to_low_of_handler_plus_l4 & LOW_BITS_MASK);
    *old_handler = (CPU_ISR_raw_handler) u32_handler;
  } else
    *old_handler = 0;

  /*
   *  Copy the template to the slot and then fix it.
   */

  *slot = _CPU_Trap_slot_template;

  u32_handler = (uint32_t) new_handler;

  slot->mov_vector_l3 |= vector;
  slot->sethi_of_handler_to_l4 |=
    (u32_handler & HIGH_BITS_MASK) >> HIGH_BITS_SHIFT;
  slot->jmp_to_low_of_handler_plus_l4 |= (u32_handler & LOW_BITS_MASK);

  /*
   * There is no instruction cache snooping, so we need to invalidate
   * the instruction cache to make sure that the processor sees the
   * changes to the trap table. This step is required on both single-
   * and multiprocessor systems.
   *
   * In a SMP configuration a change to the trap table might be
   * missed by other cores. If the system state is up, the other
   * cores can be notified using SMP messages that they need to
   * flush their icache. If the up state has not been reached
   * there is no need to notify other cores. They will do an
   * automatic flush of the icache just after entering the up
   * state, but before enabling interrupts.
   */
  rtems_cache_invalidate_entire_instruction();
}

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
)
{
   uint32_t            real_vector;
   CPU_ISR_raw_handler ignored;

  /*
   *  Get the "real" trap number for this vector ignoring the synchronous
   *  versus asynchronous indicator included with our vector numbers.
   */

   real_vector = SPARC_REAL_TRAP_NUMBER( vector );

   /*
    *  Return the previous ISR handler.
    */

   *old_handler = _ISR_Vector_table[ real_vector ];

   /*
    *  Install the wrapper so this ISR can be invoked properly.
    */

   _CPU_ISR_install_raw_handler( vector, _ISR_Handler, &ignored );

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ real_vector ] = new_handler;
}
