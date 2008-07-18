/*===============================================================*\
| Project: RTEMS support for MPC83xx                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the board specific portion                   |
| of the network interface driver                                 |
\*===============================================================*/

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>
#include <bsp.h>
#include <mpc83xx/tsec.h>
#include <mpc83xx/mpc83xx.h>
#include <stdio.h>

#define TSEC_IFMODE_RGMII 0
#define TSEC_IFMODE_GMII  1

#if defined( MPC8313ERDB)

#define TSEC_IFMODE TSEC_IFMODE_RGMII

#elif defined( MPC8349EAMDS)

#define TSEC_IFMODE TSEC_IFMODE_GMII

#elif defined( HSC_CM01)

#define TSEC_IFMODE TSEC_IFMODE_RGMII

#else

#warning No TSEC configuration available

#endif

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int BSP_tsec_attach
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   attach or detach the driver                                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct rtems_bsdnet_ifconfig *config, /* interface configuration          */
 int attaching                         /* 0 = detach, else attach          */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    1, if success                                                       |
\*=========================================================================*/
{
  int    unitNumber;
  char   *unitName;

  /*
   * Parse driver name
   */
  if((unitNumber = rtems_bsdnet_parse_driver_name(config, &unitName)) < 0) {
    return 0;
  }
  if (attaching) {
#if (TSEC_IFMODE==TSEC_IFMODE_GMII)
    if (unitNumber == 1) {
      /*
       * init system I/O configuration registers 
       * to ensure proper pin functions 
       */
      mpc83xx.syscon.sicrh = mpc83xx.syscon.sicrh & ~0x1F800000;
      /*
       * init port registers (GPIO2DIR) for TSEC1
       */
      mpc83xx.gpio[1].gpdir = ((mpc83xx.gpio[1].gpdir & ~0x00000FFF)
			       |                         0x0000001f);
    }
    if (unitNumber == 2) {
      /*
       * init system I/O configuration registers 
       * to ensure proper pin functions 
       */
      mpc83xx.syscon.sicrh = mpc83xx.syscon.sicrh & ~0x007f8000;
      /*
       * init port registers (GPIO2DIR) for TSEC2
       */
      mpc83xx.gpio[0].gpdir = ((mpc83xx.gpio[0].gpdir & ~0x000FFFFF)
			       |                         0x00087881);
    }
#endif
#if (TSEC_IFMODE==TSEC_IFMODE_RGMII)

    /*
     * Nothing special needed for TSEC1 operation
     */
#endif    
  }
  /*
   * add MAC address into config->hardware_adderss
   * FIXME: get the real address we need
   */
  if (config->hardware_address == NULL) {

#ifdef HAS_UBOOT

    switch (unitNumber) {
      case 1:
        config->hardware_address = mpc83xx_uboot_board_info.bi_enetaddr;
        break;

#ifdef CONFIG_HAS_ETH1
      case 2:
        config->hardware_address = mpc83xx_uboot_board_info.bi_enet1addr;
        break;
#endif /* CONFIG_HAS_ETH1 */

#ifdef CONFIG_HAS_ETH2
      case 3:
        config->hardware_address = mpc83xx_uboot_board_info.bi_enet2addr;
        break;
#endif /* CONFIG_HAS_ETH2 */

#ifdef CONFIG_HAS_ETH3
      case 4:
        config->hardware_address = mpc83xx_uboot_board_info.bi_enet3addr;
        break;
#endif /* CONFIG_HAS_ETH3 */

      default:
        return 0;
    }

#else /* HAS_UBOOT */

    char hw_addr [6] = { 0x00, 0x04, 0x9f, 0x00, 0x2f, 0xcb};

    config->hardware_address = hw_addr;

#endif /* HAS_UBOOT */

  }
  /*
   * set interrupt number for given interface
   */
  config->irno = ((unitNumber == 1)
		  ? BSP_IPIC_IRQ_TSEC1_TX
		  : BSP_IPIC_IRQ_TSEC2_TX);
  /*
   * call attach function of board independent driver
   */
  if (0 == rtems_mpc83xx_tsec_driver_attach_detach(config,attaching)) {
    return 0;
  }
  return 1;
}
