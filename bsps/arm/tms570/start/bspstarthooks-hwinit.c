/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the bsp_start_hook_0() implementation.
 */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
 * Copyright (C) 2016 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#include <stdint.h>
#include <bsp.h>
#include <bsp/start.h>
#include <bsp/tms570.h>

#include <bsp/tms570_selftest.h>
#include <bsp/tms570_selftest_parity.h>
#include <bsp/tms570_hwinit.h>
#include <bsp/ti_herc/errata_SSWF021_45.h>

#define PBIST_March13N_SP        0x00000008U  /**< March13 N Algo for 1 Port mem */

/* Use assembly code to avoid using the stack */
__attribute__((__naked__)) void bsp_start_hook_0( void )
{
  __asm__ volatile (
    /* Check if we run in SRAM */
    "ldr r0, =#" RTEMS_XSTRING( TMS570_MEMORY_SRAM_ORIGIN ) "\n"
    "ldr r1, =#" RTEMS_XSTRING( TMS570_MEMORY_SRAM_SIZE ) "\n"
    "sub r0, lr, r0\n"
    "cmp r1, r0\n"
    "blt 1f\n"

    /*
     * Initialize the SRAM if we are not running in SRAM.  While we are called,
     * non-volatile register r7 is not used by start.S.
     */
    "movs r0, #0x1\n"
    "mov r7, lr\n"
    "bl tms570_memory_init\n"
    "mov lr, r7\n"

    /* Jump to the high level start hook */
    "1: b tms570_start_hook_0\n"
  );
}

static RTEMS_USED void tms570_start_hook_0( void )
{
#if TMS570_VARIANT == 3137
  /*
   * Work Around for Errata DEVICE#140: ( Only on Rev A silicon)
   *
   * Errata Description:
   *            The Core Compare Module(CCM-R4) may cause nERROR to be asserted after a cold power-on
   * Workaround:
   *            Clear ESM Group2 Channel 2 error in ESMSR2 and Compare error in CCMSR register
   */
  if ( TMS570_SYS1.DEVID == 0x802AAD05U ) {
    _esmCcmErrorsClear_();
  }
#endif

#if TMS570_VARIANT == 4357
  uint32_t pll_result;

  do {
    pll_result = _errata_SSWF021_45_both_plls(10);
  } while (pll_result != 0 && pll_result != 4);
#endif

  /* Enable CPU Event Export */
  /* This allows the CPU to signal any single-bit or double-bit errors detected
   * by its ECC logic for accesses to program flash or data RAM.
   */
  _coreEnableEventBusExport_();

#if TMS570_VARIANT == 3137
  /* Workaround for Errata CORTEXR4 66 */
  _errata_CORTEXR4_66_();

  /* Workaround for Errata CORTEXR4 57 */
  _errata_CORTEXR4_57_();
#endif

  /*
   * Check if there were ESM group3 errors during power-up.
   * These could occur during eFuse auto-load or during reads from flash OTP
   * during power-up. Device operation is not reliable and not recommended
   * in this case.
   * An ESM group3 error only drives the nERROR pin low. An external circuit
   * that monitors the nERROR pin must take the appropriate action to ensure that
   * the system is placed in a safe state, as determined by the application.
   */
  if ( ( TMS570_ESM.SR[ 2 ] ) != 0U ) {
    /*SAFETYMCUSW 5 C MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
    /*SAFETYMCUSW 26 S MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
#if TMS570_VARIANT == 4357
    /*
     * During code-loading/debug-resets SR[2][4] may get set (indicates double
     * ECC error in internal RAM) ignore for now as its resolved with ESM
     * init/reset below.
     */
    if ((TMS570_SYS1.SYSESR & TMS570_SYS1_SYSESR_DBGRST) == 0) {
      for (;; ) {
      }           /* Wait */
    }
#else
    for (;; ) {
    }           /* Wait */
#endif
  }

  /* Initialize System - Clock, Flash settings with Efuse self check */
  tms570_system_hw_init();

  /* Workaround for Errata PBIST#4 */
  /* FIXME */
  //errata_PBIST_4();

  /*
   * Run a diagnostic check on the memory self-test controller.
   * This function chooses a RAM test algorithm and runs it on an on-chip ROM.
   * The memory self-test is expected to fail. The function ensures that the PBIST controller
   * is capable of detecting and indicating a memory self-test failure.
   */
  tms570_pbist_self_check();

  /* Run PBIST on STC ROM */
  tms570_pbist_run_and_check( (uint32_t) STC_ROM_PBIST_RAM_GROUP,
    ( (uint32_t) PBIST_TripleReadSlow | (uint32_t) PBIST_TripleReadFast ) );

  /* Run PBIST on PBIST ROM */
  tms570_pbist_run_and_check( (uint32_t) PBIST_ROM_PBIST_RAM_GROUP,
    ( (uint32_t) PBIST_TripleReadSlow | (uint32_t) PBIST_TripleReadFast ) );

  if ( !tms570_running_from_tcram() ) {
    /*
     * The next sequence tests TCRAM, main TMS570 system operation RAM area.
     * The tests are destructive, lead the first to fill memory by 0xc5c5c5c5
     * and then to clear it to zero. The sequence is obliviously incompatible
     * with RTEMS image running from TCRAM area (code clears itself).
     *
     * But TCRAM clear leads to overwrite of stack which is used to store
     * value of bsp_start_hook_0 call return address from link register.
     *
     * If the bsp_start_hook_0 by jump to bsp_start_hook_0_done
     * then generated C code does not use any variable which
     * is stores on stack and code works OK even that memory
     * is cleared during bsp_start_hook_0 execution.
     *
     * The last assumption is a little fragile in respect to
     * code and compiler changes.
     */

    /* Disable RAM ECC before doing PBIST for Main RAM */
    _coreDisableRamEcc_();

    /* Run PBIST on CPU RAM.
     * The PBIST controller needs to be configured separately for single-port and dual-port SRAMs.
     * The CPU RAM is a single-port memory. The actual "RAM Group" for all on-chip SRAMs is defined in the
     * device datasheet.
     */
    tms570_pbist_run_and_check( 0x08300020U,   /* ESRAM Single Port PBIST */
      (uint32_t) PBIST_March13N_SP );

    /*
     * Enable ECC checking for TCRAM accesses.
     * This function enables the CPU's ECC logic for accesses to B0TCM and B1TCM.
     */
    _coreEnableRamEcc_();
  } /* end of the code skipped for tms570_running_from_tcram() */

  /* Start PBIST on all dual-port memories */
  /* NOTE : Please Refer DEVICE DATASHEET for the list of Supported Dual port Memories.
     PBIST test performed only on the user selected memories in HALCoGen's GUI SAFETY INIT tab.
   */
  tms570_pbist_run( (uint32_t) 0x00000000U | /* EMAC RAM */
    (uint32_t) 0x00000000U |                 /* USB RAM */
    (uint32_t) 0x00000800U |                 /* DMA RAM */
    (uint32_t) 0x00000200U |                 /* VIM RAM */
    (uint32_t) 0x00000040U |                 /* MIBSPI1 RAM */
    (uint32_t) 0x00000080U |                 /* MIBSPI3 RAM */
    (uint32_t) 0x00000100U |                 /* MIBSPI5 RAM */
    (uint32_t) 0x00000004U |                 /* CAN1 RAM */
    (uint32_t) 0x00000008U |                 /* CAN2 RAM */
    (uint32_t) 0x00000010U |                 /* CAN3 RAM */
    (uint32_t) 0x00000400U |                 /* ADC1 RAM */
    (uint32_t) 0x00020000U |                 /* ADC2 RAM */
    (uint32_t) 0x00001000U |                 /* HET1 RAM */
    (uint32_t) 0x00040000U |                 /* HET2 RAM */
    (uint32_t) 0x00002000U |                 /* HTU1 RAM */
    (uint32_t) 0x00080000U |                 /* HTU2 RAM */
    (uint32_t) 0x00004000U |                 /* RTP RAM */
    (uint32_t) 0x00008000U,                  /* FRAY RAM */
    (uint32_t) PBIST_March13N_DP );

  if ( !tms570_running_from_tcram() ) {

#if TMS570_VARIANT == 3137
    /* Test the CPU ECC mechanism for RAM accesses.
     * The checkBxRAMECC functions cause deliberate single-bit and double-bit errors in TCRAM accesses
     * by corrupting 1 or 2 bits in the ECC. Reading from the TCRAM location with a 2-bit error
     * in the ECC causes a data abort exception. The data abort handler is written to look for
     * deliberately caused exception and to return the code execution to the instruction
     * following the one that caused the abort.
     */
    tms570_check_tcram_ecc();
#endif

    /* Wait for PBIST for CPU RAM to be completed */
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    while ( tms570_pbist_is_test_completed() != TRUE ) {
    }                                                  /* Wait */

    /* Check if CPU RAM passed the self-test */
    if ( tms570_pbist_is_test_passed() != TRUE ) {
      /* CPU RAM failed the self-test.
       * Need custom handler to check the memory failure
       * and to take the appropriate next step.
       */
      tms570_pbist_fail();
    }

  } /* end of the code skipped for tms570_running_from_tcram() */

  /* Disable PBIST clocks and disable memory self-test mode */
  tms570_pbist_stop();

  /* Release the MibSPI1 modules from local reset.
   * This will cause the MibSPI1 RAMs to get initialized along with the parity memory.
   */
  TMS570_SPI1.GCR0 = TMS570_SPI_GCR0_nRESET;

  /* Release the MibSPI3 modules from local reset.
   * This will cause the MibSPI3 RAMs to get initialized along with the parity memory.
   */
  TMS570_SPI3.GCR0 = TMS570_SPI_GCR0_nRESET;

  /* Release the MibSPI5 modules from local reset.
   * This will cause the MibSPI5 RAMs to get initialized along with the parity memory.
   */
  TMS570_SPI5.GCR0 = TMS570_SPI_GCR0_nRESET;

  /* Enable parity on selected RAMs */
  tms570_enable_parity();

  /* Initialize all on-chip SRAMs except for MibSPIx RAMs
   * The MibSPIx modules have their own auto-initialization mechanism which is triggered
   * as soon as the modules are brought out of local reset.
   */
  /* The system module auto-init will hang on the MibSPI RAM if the module is still in local reset.
   */
  /* NOTE : Please Refer DEVICE DATASHEET for the list of Supported Memories and their channel numbers.
            Memory Initialization is perfomed only on the user selected memories in HALCoGen's GUI SAFETY INIT tab.
   */
  tms570_memory_init(
    ( UINT32_C(1) << 1 ) |                /* DMA RAM */
    ( UINT32_C(1) << 2 ) |                /* VIM RAM */
    ( UINT32_C(1) << 5 ) |                /* CAN1 RAM */
    ( UINT32_C(1) << 6 ) |                /* CAN2 RAM */
    ( UINT32_C(1) << 10 ) |               /* CAN3 RAM */
    ( UINT32_C(1) << 8 ) |                /* ADC1 RAM */
    ( UINT32_C(1) << 14 ) |               /* ADC2 RAM */
    ( UINT32_C(1) << 3 ) |                /* HET1 RAM */
    ( UINT32_C(1) << 4 ) |                /* HTU1 RAM */
    ( UINT32_C(1) << 15 ) |               /* HET2 RAM */
    ( UINT32_C(1) << 16 )                 /* HTU2 RAM */
  );

  /* Disable parity */
  tms570_disable_parity();

  /*
   * Test the parity protection mechanism for peripheral RAMs
   * Refer DEVICE DATASHEET for the list of Supported Memories
   * with parity.
   */

  tms570_selftest_par_run( tms570_selftest_par_list,
    tms570_selftest_par_list_size );

#if 0
  /*
   * RTEMS VIM initialization is implemented by the function
   * bsp_interrupt_facility_initialize(). RTEMS does not
   * gain performance from use of vectors targets provided
   * directly by VIM. RTEMS require to route all interrupts
   * through _ARMV4_Exception_interrupt handler.
   *
   * But actual RTEMS VIM initialization lefts some registers
   * default values untouched. All registers values should be
   * ensured/configured in future probably.
   */

  /* Enable IRQ offset via Vic controller */
  _coreEnableIrqVicOffset_();

  /* Initialize VIM table */
  vimInit();
#endif

  /* Configure system response to error conditions signaled to the ESM group1 */
  tms570_esm_init();

  tms570_emif_sdram_init();

  /* Configures and enables the ARM-core Memory Protection Unit (MPU) */
  _mpuInit_();

#if 1
  /*
   * Do not depend on link register to be restored to
   * correct value from stack. If TCRAM self test is enabled
   * the all stack content is zeroed there.
   */
  bsp_start_hook_0_done();
#endif
}

/*
 * Chip specific list of peripherals which should be tested
 * for functional RAM parity reporting
 */
const tms570_selftest_par_desc_t *const
tms570_selftest_par_list[] = {
  &tms570_selftest_par_het1_desc,
  &tms570_selftest_par_htu1_desc,
  &tms570_selftest_par_het2_desc,
  &tms570_selftest_par_htu2_desc,
  &tms570_selftest_par_adc1_desc,
  &tms570_selftest_par_adc2_desc,
  &tms570_selftest_par_can1_desc,
  &tms570_selftest_par_can2_desc,
  &tms570_selftest_par_can3_desc,
  &tms570_selftest_par_vim_desc,
  &tms570_selftest_par_dma_desc,
  &tms570_selftest_par_spi1_desc,
  &tms570_selftest_par_spi3_desc,
  &tms570_selftest_par_spi5_desc,
};

const int tms570_selftest_par_list_size =
  RTEMS_ARRAY_SIZE( tms570_selftest_par_list );
