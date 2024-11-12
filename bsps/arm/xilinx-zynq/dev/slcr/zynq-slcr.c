/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * SLCR Support Implementation
 *
 * At this point, only a few operations related to programming the FPGA are
 * supported.
 *
 * Copyright (c) 2017
 *  NSF Center for High-Performance Reconfigurable Computing (CHREC),
 *  University of Pittsburgh.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of CHREC.
 *
 * Author: Patrick Gauvin <gauvin@hcs.ufl.edu>
 */
#include <stdint.h>
#include <rtems.h>
#include <dev/slcr/zynq-slcr.h>
#include <dev/slcr/zynq-slcr-regs.h>

static uint32_t rst_mask = 0xf;

#if RTEMS_INTERRUPT_LOCK_NEEDS_OBJECT
static rtems_interrupt_lock zynq_slcr_lock =
    RTEMS_INTERRUPT_LOCK_INITIALIZER( "zynq_slcr" );
#endif

static inline void slcr_unlock( void )
{
  zynq_slcr_write32( ZYNQ_SLCR_UNLOCK_OFF, ZYNQ_SLCR_UNLOCK_KEY );
}

static inline void slcr_lock( void )
{
  zynq_slcr_write32( ZYNQ_SLCR_LOCK_OFF, ZYNQ_SLCR_LOCK_KEY );
}

void zynq_slcr_fpga_clk_rst_mask_set(
  uint32_t mask
)
{
  rtems_interrupt_lock_context lcontext;

  rtems_interrupt_lock_acquire( &zynq_slcr_lock, &lcontext );
  rst_mask = 0xf & mask;
  rtems_interrupt_lock_release( &zynq_slcr_lock, &lcontext );
}

void zynq_slcr_fpga_clk_rst(
  uint32_t val
)
{
  uint32_t rst_ctrl;
  rtems_interrupt_lock_context lcontext;

  rtems_interrupt_lock_acquire( &zynq_slcr_lock, &lcontext );
  slcr_unlock();
  rst_ctrl = ZYNQ_SLCR_FPGA_RST_CTRL_FPGA_OUT_RST_GET(
    zynq_slcr_read32( ZYNQ_SLCR_FPGA_RST_CTRL_OFF )
  );
  /* Only modify resets that are set in the mask */
  zynq_slcr_write32( ZYNQ_SLCR_FPGA_RST_CTRL_OFF,
    ZYNQ_SLCR_FPGA_RST_CTRL_FPGA_OUT_RST(
      ( ~rst_mask & rst_ctrl ) | ( rst_mask & val )
    )
  );
  slcr_lock();
  rtems_interrupt_lock_release( &zynq_slcr_lock, &lcontext );
}

void zynq_slcr_level_shifter_enable(
  uint32_t val
)
{
  rtems_interrupt_lock_context lcontext;

  rtems_interrupt_lock_acquire( &zynq_slcr_lock, &lcontext );
  slcr_unlock();
  zynq_slcr_write32( ZYNQ_SLCR_LVL_SHFTR_EN_OFF, val );
  slcr_lock();
  rtems_interrupt_lock_release( &zynq_slcr_lock, &lcontext );
}
