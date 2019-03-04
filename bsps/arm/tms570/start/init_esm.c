/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief Error signaling module initialization
 */

#include <stdint.h>
#include <bsp/tms570.h>
#include <bsp/tms570_hwinit.h>

/**
 * @brief Error signaling module initialization (HCG:esmInit)
 *
 */
void tms570_esm_init( void )
{
  /** - Disable error pin channels */
  TMS570_ESM.DEPAPR1 = 0xFFFFFFFFU;
  TMS570_ESM.IEPCR4 = 0xFFFFFFFFU;

  /** - Disable interrupts */
  TMS570_ESM.IECR1 = 0xFFFFFFFFU;
  TMS570_ESM.IECR4 = 0xFFFFFFFFU;

  /** - Clear error status flags */
  TMS570_ESM.SR[0U] = 0xFFFFFFFFU;
  TMS570_ESM.SR[1U] = 0xFFFFFFFFU;
  TMS570_ESM.SSR2   = 0xFFFFFFFFU;
  TMS570_ESM.SR[2U] = 0xFFFFFFFFU;
  TMS570_ESM.SR4    = 0xFFFFFFFFU;

  /** - Setup LPC preload */
  TMS570_ESM.LTCPR = 16384U - 1U;

  /** - Reset error pin */
  if (TMS570_ESM.EPSR == 0U) {
    TMS570_ESM.EKR = 0x00000005U;
  } else {
    TMS570_ESM.EKR = 0x00000000U;
  }

  /** - Clear interrupt level */
  TMS570_ESM.ILCR1 = 0xFFFFFFFFU;
  TMS570_ESM.ILCR4 = 0xFFFFFFFFU;

  /** - Set interrupt level */
  TMS570_ESM.ILSR1 = 0x00000000;

  TMS570_ESM.ILSR4 = 0x00000000;

  /** - Enable error pin channels */
  TMS570_ESM.EEPAPR1 = 0x00000000;

  TMS570_ESM.IEPSR4 = 0x00000000;

  /** - Enable interrupts */
  TMS570_ESM.IESR1 = 0x00000000;

  TMS570_ESM.IESR4 = 0x00000000;
}
