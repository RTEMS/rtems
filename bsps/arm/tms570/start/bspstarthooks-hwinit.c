#include <stdint.h>
#include <bsp.h>
#include <bsp/start.h>
#include <bsp/tms570.h>

#include <bsp/tms570_selftest.h>
#include <bsp/tms570_selftest_parity.h>
#include <bsp/tms570_hwinit.h>

void bsp_start_hook_0_done( void );

static inline
int tms570_running_from_tcram( void )
{
  void *fncptr = (void*)bsp_start_hook_0;
  return ( fncptr >= (void*)TMS570_TCRAM_START_PTR ) &&
         ( fncptr < (void*)TMS570_TCRAM_WINDOW_END_PTR );
}

static inline
int tms570_running_from_sdram( void )
{
  void *fncptr = (void*)bsp_start_hook_0;
  return ( ( (void*)fncptr >= (void*)TMS570_SDRAM_START_PTR ) &&
           ( (void*)fncptr < (void*)TMS570_SDRAM_WINDOW_END_PTR ) );
}

#define PBIST_March13N_SP        0x00000008U  /**< March13 N Algo for 1 Port mem */

BSP_START_TEXT_SECTION void bsp_start_hook_0( void )
{
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

  /* Enable CPU Event Export */
  /* This allows the CPU to signal any single-bit or double-bit errors detected
   * by its ECC logic for accesses to program flash or data RAM.
   */
  _coreEnableEventBusExport_();

  /* Workaround for Errata CORTEXR4 66 */
  _errata_CORTEXR4_66_();

  /* Workaround for Errata CORTEXR4 57 */
  _errata_CORTEXR4_57_();

  /* check for power-on reset condition */
  /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
  if ( ( TMS570_SYS1.SYSESR & TMS570_SYS1_SYSESR_PORST ) != 0U ) {
    /* clear all reset status flags */
    TMS570_SYS1.SYSESR = 0xFFFFU;

    /* continue with normal start-up sequence */
  }
  /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
  else if ( ( TMS570_SYS1.SYSESR & TMS570_SYS1_SYSESR_OSCRST ) != 0U ) {
    /* Reset caused due to oscillator failure.
       Add user code here to handle oscillator failure */
  }
  /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
  else if ( ( TMS570_SYS1.SYSESR & TMS570_SYS1_SYSESR_WDRST ) != 0U ) {
    /* Reset caused due
     *  1) windowed watchdog violation - Add user code here to handle watchdog violation.
     *  2) ICEPICK Reset - After loading code via CCS / System Reset through CCS
     */
    /* Check the WatchDog Status register */
    if ( TMS570_RTI.WDSTATUS != 0U ) {
      /* Add user code here to handle watchdog violation. */
      /* Clear the Watchdog reset flag in Exception Status register */
      TMS570_SYS1.SYSESR = TMS570_SYS1_SYSESR_WDRST;
    } else {
      /* Clear the ICEPICK reset flag in Exception Status register */
      TMS570_SYS1.SYSESR = TMS570_SYS1_SYSESR_WDRST;
    }
  }
  /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
  else if ( ( TMS570_SYS1.SYSESR & TMS570_SYS1_SYSESR_CPURST ) != 0U ) {
    /* Reset caused due to CPU reset.
       CPU reset can be caused by CPU self-test completion, or
       by toggling the "CPU RESET" bit of the CPU Reset Control Register. */

    /* clear all reset status flags */
    TMS570_SYS1.SYSESR = TMS570_SYS1_SYSESR_CPURST;
  }
  /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
  else if ( ( TMS570_SYS1.SYSESR & TMS570_SYS1_SYSESR_SWRST ) != 0U ) {
    /* Reset caused due to software reset.
       Add user code to handle software reset. */
  } else {
    /* Reset caused by nRST being driven low externally.
       Add user code to handle external reset. */
  }

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
    for (;; ) {
    }           /* Wait */
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
  tms570_pbist_run( (uint32_t) STC_ROM_PBIST_RAM_GROUP,
    ( (uint32_t) PBIST_TripleReadSlow | (uint32_t) PBIST_TripleReadFast ) );

  /* Wait for PBIST for STC ROM to be completed */
  /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
  while ( tms570_pbist_is_test_completed() != TRUE ) {
  }                                                  /* Wait */

  /* Check if PBIST on STC ROM passed the self-test */
  if ( tms570_pbist_is_test_passed() != TRUE ) {
    /* PBIST and STC ROM failed the self-test.
     * Need custom handler to check the memory failure
     * and to take the appropriate next step.
     */
    tms570_pbist_fail();
  }

  /* Disable PBIST clocks and disable memory self-test mode */
  tms570_pbist_stop();

  /* Run PBIST on PBIST ROM */
  tms570_pbist_run( (uint32_t) PBIST_ROM_PBIST_RAM_GROUP,
    ( (uint32_t) PBIST_TripleReadSlow | (uint32_t) PBIST_TripleReadFast ) );

  /* Wait for PBIST for PBIST ROM to be completed */
  /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
  while ( tms570_pbist_is_test_completed() != TRUE ) {
  }                                                  /* Wait */

  /* Check if PBIST ROM passed the self-test */
  if ( tms570_pbist_is_test_passed() != TRUE ) {
    /* PBIST and STC ROM failed the self-test.
     * Need custom handler to check the memory failure
     * and to take the appropriate next step.
     */
    tms570_pbist_fail();
  }

  /* Disable PBIST clocks and disable memory self-test mode */
  tms570_pbist_stop();

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
    tms570_pbist_run( 0x08300020U,   /* ESRAM Single Port PBIST */
      (uint32_t) PBIST_March13N_SP );

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

    /* Disable PBIST clocks and disable memory self-test mode */
    tms570_pbist_stop();

    /*
     * Initialize CPU RAM.
     * This function uses the system module's hardware for auto-initialization of memories and their
     * associated protection schemes. The CPU RAM is initialized by setting bit 0 of the MSIENA register.
     * Hence the value 0x1 passed to the function.
     * This function will initialize the entire CPU RAM and the corresponding ECC locations.
     */
    tms570_memory_init( 0x1U );

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

    /* Test the CPU ECC mechanism for RAM accesses.
     * The checkBxRAMECC functions cause deliberate single-bit and double-bit errors in TCRAM accesses
     * by corrupting 1 or 2 bits in the ECC. Reading from the TCRAM location with a 2-bit error
     * in the ECC causes a data abort exception. The data abort handler is written to look for
     * deliberately caused exception and to return the code execution to the instruction
     * following the one that caused the abort.
     */
    tms570_check_tcram_ecc();

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
  tms570_memory_init( (uint32_t) ( (uint32_t) 1U << 1U ) |  /* DMA RAM */
    (uint32_t) ( (uint32_t) 1U << 2U ) |                /* VIM RAM */
    (uint32_t) ( (uint32_t) 1U << 5U ) |                /* CAN1 RAM */
    (uint32_t) ( (uint32_t) 1U << 6U ) |                /* CAN2 RAM */
    (uint32_t) ( (uint32_t) 1U << 10U ) |               /* CAN3 RAM */
    (uint32_t) ( (uint32_t) 1U << 8U ) |                /* ADC1 RAM */
    (uint32_t) ( (uint32_t) 1U << 14U ) |               /* ADC2 RAM */
    (uint32_t) ( (uint32_t) 1U << 3U ) |                /* HET1 RAM */
    (uint32_t) ( (uint32_t) 1U << 4U ) |                /* HTU1 RAM */
    (uint32_t) ( (uint32_t) 1U << 15U ) |               /* HET2 RAM */
    (uint32_t) ( (uint32_t) 1U << 16U )                 /* HTU2 RAM */
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

#if 1
  /*
   * Do not depend on link register to be restored to
   * correct value from stack. If TCRAM self test is enabled
   * the all stack content is zeroed there.
   */
  bsp_start_hook_0_done();
#endif
}

BSP_START_TEXT_SECTION void bsp_start_hook_1( void )
{
  /* At this point we can use objects outside the .start section  */
#if 0
  /* Do not run attempt to initialize MPU when code is running from SDRAM */
  if ( !tms570_running_from_sdram() ) {
    /*
     * MPU background areas setting has to be overlaid
     * if execution of code is required from external memory/SDRAM.
     * This region is non executable by default.
     */
    _mpuInit_();
  }
#endif
  tms570_emif_sdram_init();

  bsp_start_copy_sections();
  bsp_start_clear_bss();
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
