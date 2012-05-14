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
#include <bsp/tsec.h>
#include <bsp/u-boot.h>
#include <mpc83xx/mpc83xx.h>
#include <string.h>
#include <libcpu/spr.h>

#if MPC83XX_CHIP_TYPE / 10 != 830

#define TSEC_IFMODE_RGMII 0
#define TSEC_IFMODE_GMII  1

#if defined( MPC83XX_BOARD_MPC8313ERDB)

#define TSEC_IFMODE TSEC_IFMODE_RGMII

#elif defined( MPC83XX_BOARD_MPC8349EAMDS)

#define TSEC_IFMODE TSEC_IFMODE_GMII

#elif defined( MPC83XX_BOARD_HSC_CM01)

#define TSEC_IFMODE TSEC_IFMODE_RGMII

#else

#warning No TSEC configuration available

#endif

/* System Version Register */
#define SVR 286
SPR_RO( SVR)

/* Processor Version Register */
SPR_RO( PPC_PVR)

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
  tsec_config tsec_cfg;
  int    unitNumber;
  char *unitName;
  uint32_t svr = _read_SVR();
  uint32_t pvr = _read_PPC_PVR();

  memset(&tsec_cfg, 0, sizeof(tsec_cfg));
  config->drv_ctrl = &tsec_cfg;

  /*
   * Parse driver name
   */
  if((unitNumber = rtems_bsdnet_parse_driver_name(config, &unitName)) < 0) {
    return 0;
  }

  tsec_cfg.reg_ptr = &mpc83xx.tsec [unitNumber - 1];
  tsec_cfg.mdio_ptr = &mpc83xx.tsec [0];

  if (attaching) {
#if (TSEC_IFMODE==TSEC_IFMODE_GMII)
#if !defined(MPC83XX_BOARD_HSC_CM01)

      /*
       * do not change system I/O configuration registers on HSC board
       * because should initialize from RCW
       */


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
#endif /* !defined(MPC83XX_BOARD_HSC_CM01) */
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
#if !defined(HAS_UBOOT)
    static char hw_addr [TSEC_COUNT][6];
    volatile tsec_registers *reg_ptr = tsec_cfg.reg_ptr;

    /* read MAC address from hardware register */
    /* we expect it htere from the boot loader */
    config->hardware_address = hw_addr[unitNumber-1];

    hw_addr[unitNumber-1][5] = (reg_ptr->macstnaddr[0] >> 24) & 0xff;
    hw_addr[unitNumber-1][4] = (reg_ptr->macstnaddr[0] >> 16) & 0xff;
    hw_addr[unitNumber-1][3] = (reg_ptr->macstnaddr[0] >>  8) & 0xff;
    hw_addr[unitNumber-1][2] = (reg_ptr->macstnaddr[0] >>  0) & 0xff;
    hw_addr[unitNumber-1][1] = (reg_ptr->macstnaddr[1] >> 24) & 0xff;
    hw_addr[unitNumber-1][0] = (reg_ptr->macstnaddr[1] >> 16) & 0xff;
#endif

#if defined(HAS_UBOOT)
    switch (unitNumber) {
      case 1:
        config->hardware_address = bsp_uboot_board_info.bi_enetaddr;
        break;

#ifdef CONFIG_HAS_ETH1
      case 2:
        config->hardware_address = bsp_uboot_board_info.bi_enet1addr;
        break;
#endif /* CONFIG_HAS_ETH1 */

#ifdef CONFIG_HAS_ETH2
      case 3:
        config->hardware_address = bsp_uboot_board_info.bi_enet2addr;
        break;
#endif /* CONFIG_HAS_ETH2 */

#ifdef CONFIG_HAS_ETH3
      case 4:
        config->hardware_address = bsp_uboot_board_info.bi_enet3addr;
        break;
#endif /* CONFIG_HAS_ETH3 */

      default:
        return 0;
    }

#endif /* HAS_UBOOT */

  }
  /*
   * set interrupt number for given interface
   */
  config->irno = (unsigned) (
      unitNumber == 1
      ? BSP_IPIC_IRQ_TSEC1_TX
      : BSP_IPIC_IRQ_TSEC2_TX
    );

  if (svr == 0x80b00010 && pvr == 0x80850010) {
    /*
     * This is a special case for MPC8313ERDB with silicon revision 1.  Look in
     * "MPC8313ECE Rev. 3, 3/2008" errata for "IPIC 1".
     */
    if (unitNumber == 1) {
      tsec_cfg.irq_num_tx      = 37;
      tsec_cfg.irq_num_rx      = 36;
      tsec_cfg.irq_num_err     = 35;
    } else if (unitNumber == 2) {
      tsec_cfg.irq_num_tx      = 34;
      tsec_cfg.irq_num_rx      = 33;
      tsec_cfg.irq_num_err     = 32;
    } else {
      return 0;
    }
  } else {
    rtems_irq_number irno = unitNumber == 1 ?
      BSP_IPIC_IRQ_TSEC1_TX : BSP_IPIC_IRQ_TSEC2_TX;

    /* get base interrupt number (for Tx irq, Rx=base+1,Err=base+2) */
    tsec_cfg.irq_num_tx = irno + 0;
    tsec_cfg.irq_num_rx = irno + 1;
    tsec_cfg.irq_num_err = irno + 2;
  }

  /*
   * XXX: Although most hardware builders will assign the PHY addresses
   * like this, this should be more configurable
   */
#ifdef MPC83XX_BOARD_MPC8313ERDB
  if (unitNumber == 2) {
	  tsec_cfg.phy_default = 4;
  } else {
	  /* TODO */
	  return 0;
  }
#else /* MPC83XX_BOARD_MPC8313ERDB */
  tsec_cfg.phy_default = unitNumber-1;
#endif /* MPC83XX_BOARD_MPC8313ERDB */

  tsec_cfg.unit_number = unitNumber;
  tsec_cfg.unit_name = unitName;

  /*
   * call attach function of board independent driver
   */
  return tsec_driver_attach_detach(config, attaching);
}

#endif /* MPC83XX_CHIP_TYPE / 10 != 830 */
