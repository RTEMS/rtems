/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVNOEL
 *
 * @brief This source file contains the start of the secondary processors of
 *   the NOEL-V BSP.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH & Co. KG
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/bootcard.h>
#include <bsp/irq.h>
#include <amba.h>

#include <rtems/score/riscv-utility.h>
#include <rtems/score/smpimpl.h>

#ifdef RISCV_USE_S_MODE
#include <machine/sbi.h>

extern void RISCV_Start_on_processor( uint32_t hartid );

#endif

void bsp_start_on_secondary_processor( Per_CPU_Control *cpu_self )
{
  uint32_t cpu_index_self;

  cpu_index_self = _Per_CPU_Get_index( cpu_self );

  /**
   * There is a slight problem here. Opposite to SPARC, RISC-V cores all start
   * executing the same code. This means that the secondary cores will also
   * execute the code in bsp_start() and thus also call
   * bsp_start_on_secondary_processor(). We need to make sure that only the core
   * that is supposed to start multitasking on secondary processors does so,
   * while the other cores should just idle. The check for
   * _SMP_Should_start_processor() should be sufficient but it also means the
   * ts-validation-smp-only-2.exe test will always fail on this BSP as it sets
   * the second processor as offline and then check that it has not been
   * started. Adding a check on _Per_CPU_Is_processor_online() makes the test
   * pass but breaks other tests so for now we will leave it as is and
   * investigate further if needed.
   */
  if ( _SMP_Should_start_processor( cpu_index_self ) ) {
#ifdef RISCV_USE_S_MODE
    set_csr( sie, SIP_SSIP | MIP_SEIP );
#else
    set_csr( mie, MIP_MSIP | MIP_MEIP );
#endif
    _SMP_Start_multitasking_on_secondary_processor( cpu_self );
  } else {
    _CPU_Thread_Idle_body( 0 );
  }
}

static int for_each_hart( struct ambapp_dev *dev, int index, void *arg )
{
  uint32_t *count = (uint32_t *) arg;

  (void) dev;
  (void) index;

  ( *count )++;

  return 0;
}

static uint32_t grlib_get_cpu_count( void )
{
  uint32_t cpu_count = 0;

  ambapp_for_each(
    ambapp_plb(),
    ( OPTIONS_ALL | OPTIONS_AHB_MSTS ),
    VENDOR_GAISLER,
    GAISLER_RV64GC,
    for_each_hart,
    &cpu_count
  );
  return cpu_count;
}

uint32_t _CPU_SMP_Initialize( void )
{
  return grlib_get_cpu_count();
}

bool _CPU_SMP_Start_processor( uint32_t cpu_index )
{
  uint32_t hartid;
  hartid = _RISCV_Map_cpu_index_to_hartid( cpu_index );

#ifdef RISCV_USE_S_MODE
  sbi_hsm_hart_start( hartid, (uintptr_t) RISCV_Start_on_processor, 1 );
#else
  (void) hartid;
#endif

  return true;
}

void _CPU_SMP_Finalize_initialization( uint32_t cpu_count )
{
  (void) cpu_count;
#ifdef RISCV_USE_S_MODE
  set_csr( sie, SIP_SSIP );
#else
  set_csr( mie, MIP_MSIP );
#endif
}

void _CPU_SMP_Prepare_start_multitasking( void )
{
  /* Do nothing */
}

#ifdef RISCV_USE_S_MODE
static void riscv_smp_s_mode_send_ipi( uint32_t target_processor_index )
{
  uint32_t      hartid;
  unsigned long hart_mask;

  hartid = _RISCV_Map_cpu_index_to_hartid( target_processor_index );
  hart_mask = 1UL << hartid;
  sbi_send_ipi( &hart_mask );
}
#else
static void riscv_smp_m_mode_send_ipi( uint32_t target_processor_index )
{
  Per_CPU_Control *cpu;

  cpu = _Per_CPU_Get_by_index( target_processor_index );
  *cpu->cpu_per_cpu.clint_msip = 0x1;
}
#endif

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index )
{
#ifdef RISCV_USE_S_MODE
  riscv_smp_s_mode_send_ipi( target_processor_index );
#else
  riscv_smp_m_mode_send_ipi( target_processor_index );
#endif
}

#ifdef RISCV_USE_S_MODE
uint32_t _CPU_SMP_Get_current_processor( void )
{
  return _Per_CPU_Get_index( _CPU_Get_current_per_CPU_control() );
}
#endif
