/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief Initialization of external memory/SDRAM interface.
 */

#include <stdint.h>
#include <bsp/tms570.h>
#include <bsp/tms570_hwinit.h>

void tms570_emif_sdram_init( void )
{
  uint32_t dummy;
  uint32_t sdtimr = 0;
  uint32_t sdcr = 0;

  /* Do not run attempt to initialize SDRAM when code is running from it */
  if ( ( (void*)tms570_emif_sdram_init >= (void*)TMS570_SDRAM_START_PTR ) &&
       ( (void*)tms570_emif_sdram_init <= (void*)TMS570_SDRAM_WINDOW_END_PTR ) )
    return;

  sdtimr = TMS570_EMIF_SDTIMR_T_RFC_SET( sdtimr, 6 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RP_SET( sdtimr, 2 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RCD_SET( sdtimr, 2 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_WR_SET( sdtimr, 2 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RAS_SET( sdtimr, 4 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RC_SET( sdtimr, 6 - 1 );
  sdtimr = TMS570_EMIF_SDTIMR_T_RRD_SET( sdtimr, 2 - 1 );

  TMS570_EMIF.SDTIMR = sdtimr;

  /* Minimum number of ECLKOUT cycles from Self-Refresh exit to any command */
  TMS570_EMIF.SDSRETR = 5;
  /* Define the SDRAM refresh period in terms of EMIF_CLK cycles. */
  TMS570_EMIF.SDRCR = 2000;

  /* SR - Self-Refresh mode bit. */
  sdcr |= TMS570_EMIF_SDCR_SR * 0;
  /* field: PD - Power Down bit controls entering and exiting of the power-down mode. */
  sdcr |= TMS570_EMIF_SDCR_PD * 0;
  /* PDWR - Perform refreshes during power down. */
  sdcr |= TMS570_EMIF_SDCR_PDWR * 0;
  /* NM - Narrow mode bit defines whether SDRAM is 16- or 32-bit-wide */
  sdcr |= TMS570_EMIF_SDCR_NM * 1;
  /* CL - CAS Latency. */
  sdcr = TMS570_EMIF_SDCR_CL_SET( sdcr, 2 );
  /* CL can only be written if BIT11_9LOCK is simultaneously written with a 1. */
  sdcr |= TMS570_EMIF_SDCR_BIT11_9LOCK * 1;
  /* IBANK - Internal SDRAM Bank size. */
  sdcr = TMS570_EMIF_SDCR_IBANK_SET( sdcr, 2 ); /* 4-banks device */
  /* Page Size. This field defines the internal page size of connected SDRAM devices. */
  sdcr = TMS570_EMIF_SDCR_PAGESIZE_SET( sdcr, 0 ); /* elements_256 */

  TMS570_EMIF.SDCR = sdcr;

  dummy = *(volatile uint32_t*)TMS570_SDRAM_START_PTR;
  (void) dummy;
  TMS570_EMIF.SDRCR = 31;

  /* Define the SDRAM refresh period in terms of EMIF_CLK cycles. */
  TMS570_EMIF.SDRCR = 312;
}
