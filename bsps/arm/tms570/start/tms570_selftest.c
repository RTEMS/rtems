/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief TMS570 selftest support functions implementation.
 */
/*
 * Copyright (C) 2009-2015 Texas Instruments Incorporated - www.ti.com
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <bsp/tms570.h>
#include <bsp/tms570_selftest.h>
#include <bsp/tms570_hwinit.h>

/**
 * @brief Checks to see if the EFUSE Stuck at zero test is completed successfully (HCG:efcStuckZeroTest).
 /
 * @return 1 if EFUSE Stuck at zero test completed, otherwise 0.
 *
 * Checks to see if the EFUSE Stuck at zero test is completed successfully.
 */
/* SourceId : SELFTEST_SourceId_012 */
/* DesignId : SELFTEST_DesignId_014 */
/* Requirements : HL_SR402 */
bool tms570_efc_stuck_zero( void )
{
  uint32_t esm_estatus4, esm_estatus1;

  bool result = false;

  uint32_t output_enable = TMS570_EFUSE_EFCBOUND_Self_Test_Error_OE |
                           TMS570_EFUSE_EFCBOUND_Single_Bit_Error_OE |
                           TMS570_EFUSE_EFCBOUND_Instruction_Error_OE |
                           TMS570_EFUSE_EFCBOUND_Autoload_Error_OE;

  uint32_t error_checks = TMS570_EFUSE_EFCBOUND_EFC_Single_Bit_Error |
                          TMS570_EFUSE_EFCBOUND_EFC_Instruction_Error |
                          TMS570_EFUSE_EFCBOUND_EFC_Autoload_Error |
                          TMS570_EFUSE_EFCBOUND_EFC_Self_Test_Error;

  /* configure the output enable for auto load error , instruction info,
       instruction error, and self test error using boundary register
       and drive values one across all the errors */
  TMS570_EFUSE.EFCBOUND = output_enable | error_checks;

  /* Read from the pin register. This register holds the current values
       of above errors. This value should be 0x5c00.If not at least one of
       the above errors is stuck at 0. */
  if ( ( TMS570_EFUSE.EFCPINS & 0x5C00U ) == 0x5C00U ) {
    esm_estatus4 = TMS570_ESM.SR4;
    esm_estatus1 = TMS570_ESM.SR[ 2U ];

    /* check if the ESM group1 channel 41 is set and group3 channel 1 is set */
    if ( ( ( esm_estatus4 & 0x200U ) == 0x200U ) &&
         ( ( esm_estatus1 & 0x2U ) == 0x2U ) ) {
      /* stuck-at-zero test passed */
      result = true;
    }
  }

  /* put the pins back low */
  TMS570_EFUSE.EFCBOUND = output_enable;

  /* clear group1 flag */
  TMS570_ESM.SR4 = 0x200U;

  /* clear group3 flag */
  TMS570_ESM.SR[ 2U ] = 0x2U;

  /* The nERROR pin will become inactive once the LTC counter expires */
  TMS570_ESM.EKR = 0x5U;

  return result;
}

/**
 * @brief EFUSE module self check Driver (HCG:efcSelfTest)
 *
 * This function self checks the EFSUE module.
 */
/* SourceId : SELFTEST_SourceId_013 */
/* DesignId : SELFTEST_DesignId_013 */
/* Requirements : HL_SR402 */
void tms570_efc_self_test( void )
{
  /* configure self-test cycles */
  TMS570_EFUSE.EFC_ST_CY = 0x258U;

  /* configure self-test signature */
  TMS570_EFUSE.EFC_ST_SIG = 0x5362F97FU;

  /* configure boundary register to start ECC self-test */
  TMS570_EFUSE.EFCBOUND = 0x00002000 |
                          TMS570_EFUSE_EFCBOUND_Input_Enable( 0xF );
}

/**
 * @brief EFUSE module self check Driver (HCG:checkefcSelfTest)
 *
 * @return Returns TRUE if EFC Selftest was a PASS, else FALSE
 *
 * This function returns the status of efcSelfTest.
 * Note: This function can be called only after calling efcSelfTest
 */
/* SourceId : SELFTEST_SourceId_014 */
/* DesignId : SELFTEST_DesignId_015 */
/* Requirements : HL_SR403 */
bool tms570_efc_check_self_test( void )
{
  bool result = false;

  uint32_t efc_pins, efc_error;
  uint32_t esmCh40Stat, esmCh41Stat = 0U;

  /* wait until EFC self-test is done */
  /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
  while ( ( TMS570_EFUSE.EFCPINS & TMS570_EFUSE_EFCPINS_EFC_Selftest_Done ) ==
          0U ) {
  }                                                                               /* Wait */

  /* check if EFC self-test error occurred */
  efc_pins = TMS570_EFUSE.EFCPINS;
  efc_error = TMS570_EFUSE.EFC_ERR_STAT;

  if ( ( ( efc_pins & TMS570_EFUSE_EFCPINS_EFC_Selftest_Error ) == 0U ) &&
       ( ( efc_error & 0x1FU ) == 0U ) ) {
    /* check if EFC self-test error is set */
    esmCh40Stat = TMS570_ESM.SR4 & 0x100U;
    esmCh41Stat = TMS570_ESM.SR4 & 0x200U;

    if ( ( esmCh40Stat == 0U ) && ( esmCh41Stat == 0U ) ) {
      result = true;
    }
  }

  return result;
}

/**
 * @brief EFUSE module self check Driver (HCG:efcCheck)
 * @return Returns 0 if no error was detected during autoload and Stuck At Zero Test passed
 *                 1 if no error was detected during autoload but Stuck At Zero Test failed
 *                 2 if there was a single-bit error detected during autoload
 *                 3 if some other error occurred during autoload
 *
 *   This function self checks the EFUSE module.
 */
/* SourceId : SELFTEST_SourceId_011 */
/* DesignId : SELFTEST_DesignId_012 */
/* Requirements : HL_SR402 */
uint32_t tms570_efc_check( void )
{
  uint32_t efc_status = 0U;
  uint32_t status;

  /* read the EFC Error Status Register */
  efc_status = TMS570_EFUSE.EFC_ERR_STAT;

  if ( efc_status == 0x0U ) {
    /* run stuck-at-zero test and check if it passed */
    if ( tms570_efc_stuck_zero() == true ) {
      /* start EFC ECC logic self-test */
      tms570_efc_self_test();
      status = 0U;
    } else {
      /* EFC output is stuck-at-zero, device operation unreliable */
      bsp_selftest_fail_notification( EFCCHECK_FAIL1 );
      status = 1U;
    }
  }
  /* EFC Error Register is not zero */
  else {
    /* one-bit error detected during autoload */
    if ( efc_status == 0x15U ) {
      /* start EFC ECC logic self-test */
      tms570_efc_self_test();
      status = 2U;
    } else {
      /* Some other EFC error was detected */
      bsp_selftest_fail_notification( EFCCHECK_FAIL1 );
      status = 3U;
    }
  }

  return status;
}

/**
 * @brief PBIST self test Driver (HCG:pbistSelfCheck)
 *
 * This function is called to perform PBIST self test.
 */
/* SourceId : SELFTEST_SourceId_005 */
/* DesignId : SELFTEST_DesignId_005 */
/* Requirements : HL_SR399 */
void tms570_pbist_self_check( void )
{
  volatile uint32_t i = 0U;
  uint32_t          PBIST_wait_done_loop = 0U;

  /* Run a diagnostic check on the memory self-test controller */
  /* First set up the PBIST ROM clock as this clock frequency is limited to 90MHz */

  /* Disable PBIST clocks and ROM clock */
  TMS570_PBIST.PACT = 0x0U;

  /* PBIST ROM clock frequency = HCLK frequency /2 */
  /* Disable memory self controller */
  TMS570_SYS1.MSTGCR = 0x00000105U;

  /* Disable Memory Initialization controller */
  TMS570_SYS1.MINITGCR = 0x5U;

  /* Enable memory self controller */
  TMS570_SYS1.MSTGCR = 0x0000010AU;

  /* Clear PBIST Done */
  TMS570_SYS1.MSTCGSTAT = 0x1U;

  /* Enable PBIST controller */
  TMS570_SYS1.MSIENA = 0x1U;

  /* wait for 32 VBUS clock cycles at least, based on HCLK to VCLK ratio */
  /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "Wait for few clock cycles (Value of i not used)" */
  /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "Wait for few clock cycles (Value of i not used)" */
  for ( i = 0U; i < ( 32U + ( 32U * 1U ) ); i++ ) { /* Wait */
  }

  /* Enable PBIST clocks and ROM clock */
  TMS570_PBIST.PACT = 0x3U;

  /* CPU control of PBIST */
  TMS570_PBIST.DLR = 0x10U;

  /* Custom always fail algo, this will not use the ROM and just set a fail */
  TMS570_PBIST.RAMT = 0x00002000U;
  *(volatile uint32_t *) 0xFFFFE400U = 0x4C000001U;
  *(volatile uint32_t *) 0xFFFFE440U = 0x00000075U;
  *(volatile uint32_t *) 0xFFFFE404U = 0x4C000002U;
  *(volatile uint32_t *) 0xFFFFE444U = 0x00000075U;
  *(volatile uint32_t *) 0xFFFFE408U = 0x4C000003U;
  *(volatile uint32_t *) 0xFFFFE448U = 0x00000075U;
  *(volatile uint32_t *) 0xFFFFE40CU = 0x4C000004U;
  *(volatile uint32_t *) 0xFFFFE44CU = 0x00000075U;
  *(volatile uint32_t *) 0xFFFFE410U = 0x4C000005U;
  *(volatile uint32_t *) 0xFFFFE450U = 0x00000075U;
  *(volatile uint32_t *) 0xFFFFE414U = 0x4C000006U;
  *(volatile uint32_t *) 0xFFFFE454U = 0x00000075U;
  *(volatile uint32_t *) 0xFFFFE418U = 0x00000000U;
  *(volatile uint32_t *) 0xFFFFE458U = 0x00000001U;

  /* PBIST_RUN */
  ( &TMS570_PBIST.DLR )[ 2 ] = 1;

  /* wait until memory self-test done is indicated */
  /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
  while ( ( TMS570_SYS1.MSTCGSTAT & 0x1U ) != 0x1U )
    PBIST_wait_done_loop++;

  /* Wait */

  /* Check for the failure */
  if ( ( TMS570_PBIST.FSRF0 & 0x1U ) != 0x1U ) {
    /* No failure was indicated even if the always fail algorithm was run*/
    bsp_selftest_fail_notification( PBISTSELFCHECK_FAIL1 );
  } else {
    /* Check that the algorithm executed in the expected amount of time. */
    /* This time is dependent on the ROMCLKDIV selected above            */
    if ( PBIST_wait_done_loop >= 2U ) {
      bsp_selftest_fail_notification( PBISTSELFCHECK_FAIL2 );
    }

    /* Disable PBIST clocks and ROM clock */
    TMS570_PBIST.PACT = 0x0U;

    /* Disable PBIST */
    TMS570_SYS1.MSTGCR &= 0xFFFFFFF0U;
    TMS570_SYS1.MSTGCR |= 0x5U;
  }
}

/**
 * @brief CPU self test Driver (HCG:pbistRun)
 * @param[in] raminfoL   - Select the list of RAM to be tested.
 * @param[in] algomask   - Select the list of Algorithm to be run.
 *
 * This function performs Memory Built-in Self test using PBIST module.
 */
/* SourceId : SELFTEST_SourceId_006 */
/* DesignId : SELFTEST_DesignId_006 */
/* Requirements : HL_SR400 */
void tms570_pbist_run(
  uint32_t raminfoL,
  uint32_t algomask
)
{
  volatile uint32_t i = 0U;

  /* PBIST ROM clock frequency = HCLK frequency /2 */
  /* Disable memory self controller */
  TMS570_SYS1.MSTGCR = 0x00000105U;

  /* Disable Memory Initialization controller */
  TMS570_SYS1.MINITGCR = 0x5U;

  /* Enable PBIST controller */
  TMS570_SYS1.MSIENA = 0x1U;

  /* Enable memory self controller */
  TMS570_SYS1.MSTGCR = 0x0000010AU;

  /* wait for 32 VBUS clock cycles at least, based on HCLK to VCLK ratio */
  /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "Wait for few clock cycles (Value of i not used)" */
  /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "Wait for few clock cycles (Value of i not used)" */
  for ( i = 0U; i < ( 32U + ( 32U * 1U ) ); i++ ) { /* Wait */
  }

  /* Enable PBIST clocks and ROM clock */
  TMS570_PBIST.PACT = 0x3U;

  /* Select all algorithms to be tested */
  TMS570_PBIST.ALGO = algomask;

  /* Select RAM groups */
  TMS570_PBIST.RINFOL = raminfoL;

  /* Select all RAM groups */
  TMS570_PBIST.RINFOUL = 0x00000000U;

  /* ROM contents will not override RINFOx settings */
  TMS570_PBIST.OVER = 0x0U;

  /* Algorithm code is loaded from ROM */
  TMS570_PBIST.ROM = 0x3U;

  /* Start PBIST */
  TMS570_PBIST.DLR = 0x14U;
}

/**
 *  @brief Routine to stop PBIST test enabled (HCG:pbistStop)
 *
 *  This function is called to stop PBIST after test is performed.
 */
/* SourceId : SELFTEST_SourceId_007 */
/* DesignId : SELFTEST_DesignId_007 */
/* Requirements : HL_SR523 */
void tms570_pbist_stop( void )
{
  /* disable pbist clocks and ROM clock */
  TMS570_PBIST.PACT = 0x0U;
  TMS570_SYS1.MSTGCR &= 0xFFFFFFF0U;
  TMS570_SYS1.MSTGCR |= 0x5U;
}

/**
 * @brief Checks to see if the PBIST test is completed (HCG:pbistIsTestCompleted)
 * @return 1 if PBIST test completed, otherwise 0.
 *
 * Checks to see if the PBIST test is completed.
 */
/* SourceId : SELFTEST_SourceId_008 */
/* DesignId : SELFTEST_DesignId_008 */
/* Requirements : HL_SR401 */
bool tms570_pbist_is_test_completed( void )
{
  return ( ( TMS570_SYS1.MSTCGSTAT & 0x1U ) != 0U );
}

/**
 * @brief Checks to see if the PBIST test is completed successfully (HCG:pbistIsTestPassed)
 * @return 1 if PBIST test passed, otherwise 0.
 *
 * Checks to see if the PBIST test is completed successfully.
 */
/* SourceId : SELFTEST_SourceId_009 */
/* DesignId : SELFTEST_DesignId_009 */
/* Requirements : HL_SR401 */
bool tms570_pbist_is_test_passed( void )
{
  bool status;

  if ( TMS570_PBIST.FSRF0 == 0U ) {
    status = true;
  } else {
    status = false;
  }

  return status;
}

/**
 * @brief Checks to see if the PBIST Port test is completed successfully (HCG:pbistPortTestStatus)
 * @param[in] port   - Select the port to get the status.
 * @return 1 if PBIST Port test completed successfully, otherwise 0.
 *
 * Checks to see if the selected PBIST Port test is completed successfully.
 */
/* SourceId : SELFTEST_SourceId_010 */
/* DesignId : SELFTEST_DesignId_010 */
/* Requirements : HL_SR401 */
bool tms570_pbist_port_test_status( uint32_t port )
{
  bool status;

  if ( port == (uint32_t) PBIST_PORT0 ) {
    status = ( TMS570_PBIST.FSRF0 == 0U );
  } else {
    /* Invalid Input */
    status = false;
  }

  return status;
}

/**
 * @brief Reaction to PBIST failure (HCG:pbistFail)
 *
 * @return Void.
 */
/* SourceId : SELFTEST_SourceId_042 */
/* DesignId : SELFTEST_DesignId_011 */
/* Requirements : HL_SR401 */
void tms570_pbist_fail( void )
{
  uint32_t PBIST_RAMT, PBIST_FSRA0, PBIST_FSRDL0;

  /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> "LDRA Tool issue" */
  PBIST_RAMT = TMS570_PBIST.RAMT;
  PBIST_FSRA0 = TMS570_PBIST.FSRA0;
  PBIST_FSRDL0 = TMS570_PBIST.FSRDL0;

  if ( tms570_pbist_port_test_status( (uint32_t) PBIST_PORT0 ) != true ) {
    uint32_t groupSelect = ( PBIST_RAMT & 0xFF000000U ) >> 24U;
    uint32_t dataSelect = ( PBIST_RAMT & 0x00FF0000U ) >> 16U;
    uint32_t address = PBIST_FSRA0;
    uint32_t data = PBIST_FSRDL0;
    tms570_memory_port0_fail_notification( groupSelect,
      dataSelect,
      address,
      data );
  } else {
/*SAFETYMCUSW 5 C MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
/*SAFETYMCUSW 26 S MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
/*SAFETYMCUSW 28 D MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
    for (;; ) {
    }           /* Wait */
  }
}

/**
 * @brief Memory Initialization Driver (HCG:memoryInit)
 *
 * This function is called to perform Memory initialization of selected RAM's.
 */
/* SourceId : SELFTEST_SourceId_002 */
/* DesignId : SELFTEST_DesignId_004 */
/* Requirements : HL_SR396 */
void tms570_memory_init( uint32_t ram )
{
  /* Enable Memory Hardware Initialization */
  TMS570_SYS1.MINITGCR = 0xAU;

  /* Enable Memory Hardware Initialization for selected RAM's */
  TMS570_SYS1.MSIENA = ram;

  /* Wait until Memory Hardware Initialization complete */
  /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
  while ( ( TMS570_SYS1.MSTCGSTAT & 0x00000100U ) != 0x00000100U ) {
  }                                                              /* Wait */

  /* Disable Memory Hardware Initialization */
  TMS570_SYS1.MINITGCR = 0x5U;
}

volatile uint32_t *const
tms570_esm_group_channel_to_sr_table[ 4 ][ 2 ] = {
  { NULL, NULL },
  { &TMS570_ESM.SR[ 0 ], &TMS570_ESM.SR4 },
  { &TMS570_ESM.SR[ 1 ], NULL },
  { &TMS570_ESM.SR[ 2 ], NULL },
};

/**
 * @brief Routine to clear specified error channel signalling bit
 * @param[in] grp   - ESM error channels group
 * @param[in] chan  - ESM error channel number inside specified group
 */
void tms570_esm_channel_sr_clear(
  unsigned grp,
  unsigned chan
)
{
  volatile uint32_t *sr_reg;

  sr_reg = tms570_esm_group_channel_to_sr_table[ grp ][ chan >> 5 ];

  if ( sr_reg != NULL )
    *sr_reg = 1 << (chan & 0x1f);
}

/** tms570_esm_channel_sr_get
 * @brief Routine to test is specified error channel is signalling error
 * @param[in] grp   - ESM error channels group
 * @param[in] chan  - ESM error channel number inside specified group
 */
int tms570_esm_channel_sr_get(
  unsigned grp,
  unsigned chan
)
{
  volatile uint32_t *sr_reg;

  sr_reg = tms570_esm_group_channel_to_sr_table[ grp ][ chan >> 5 ];

  if ( sr_reg != NULL )
    return *sr_reg & ( 1 << ( chan & 0x1f ) );
  else
    return 0;
}

/**
 * @brief Enable peripheral RAM parity (HCG:enableParity)
 *
 * This function enables RAM parity for all peripherals for which RAM parity check is enabled.
 * This function is called before memoryInit in the startup
 *
 */
void tms570_enable_parity( void )
{
  TMS570_DMA.DMAPCR = 0xAU;                   /* Enable DMA RAM parity */
  TMS570_VIM.PARCTL = 0xAU;                   /* Enable VIM RAM parity */
  TMS570_DCAN1.CTL = ((uint32_t)0xAU << 10U) | 1U; /* Enable CAN1 RAM parity */
  TMS570_DCAN2.CTL = ((uint32_t)0xAU << 10U) | 1U; /* Enable CAN2 RAM parity */
  TMS570_DCAN3.CTL = ((uint32_t)0xAU << 10U) | 1U; /* Enable CAN3 RAM parity */
  TMS570_ADC1.PARCR = 0xAU;                   /* Enable ADC1 RAM parity */
  TMS570_ADC2.PARCR = 0xAU;                   /* Enable ADC2 RAM parity */
  TMS570_NHET1.PCR  = 0xAU;                   /* Enable HET1 RAM parity */
  TMS570_HTU1.PCR   = 0xAU;                   /* Enable HTU1 RAM parity */
  TMS570_NHET2.PCR  = 0xAU;                   /* Enable HET2 RAM parity */
  TMS570_HTU2.PCR   = 0xAU;                   /* Enable HTU2 RAM parity */
}

/**
 * @brief Disable peripheral RAM parity (HCG:disableParity)
 *
 * This function disables RAM parity for all peripherals for which RAM parity check is enabled.
 * This function is called after memoryInit in the startup
 *
 */
void tms570_disable_parity( void )
{
  TMS570_DMA.DMAPCR = 0x5U;                   /* Disable DMA RAM parity */
  TMS570_VIM.PARCTL = 0x5U;                   /* Disable VIM RAM parity */
  TMS570_DCAN1.CTL = ((uint32_t)0x5U << 10U) | 1U; /* Disable CAN1 RAM parity */
  TMS570_DCAN2.CTL = ((uint32_t)0x5U << 10U) | 1U; /* Disable CAN2 RAM parity */
  TMS570_DCAN3.CTL = ((uint32_t)0x5U << 10U) | 1U; /* Disable CAN3 RAM parity */
  TMS570_ADC1.PARCR = 0x5U;                   /* Disable ADC1 RAM parity */
  TMS570_ADC2.PARCR = 0x5U;                   /* Disable ADC2 RAM parity */
  TMS570_NHET1.PCR  = 0x5U;                   /* Disable HET1 RAM parity */
  TMS570_HTU1.PCR   = 0x5U;                   /* Disable HTU1 RAM parity */
  TMS570_NHET2.PCR  = 0x5U;                   /* Disable HET2 RAM parity */
  TMS570_HTU2.PCR   = 0x5U;                   /* Disable HTU2 RAM parity */
}
