/* Driver Manager Configuration file.
 *
 * COPYRIGHT (c) 2009.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/*
 *  The configuration consist of an array with function pointers that
 *  register one or more drivers that will be used by the Driver Manger.
 *
 *  The Functions are called in the order they are declared.
 *
 */

#ifndef _DRIVER_MANAGER_CONFDEFS_H_
#define _DRIVER_MANAGER_CONFDEFS_H_

#include "drvmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct drvmgr_drv_reg_func drvmgr_drivers[];

#ifdef CONFIGURE_INIT

/*** AMBA Plug & Play Drivers ***/
#define DRIVER_AMBAPP_GAISLER_GPTIMER_REG {gptimer_register_drv}
extern void gptimer_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_APBUART_REG {apbuart_cons_register_drv}
extern void apbuart_cons_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRETH_REG {greth_register_drv}
extern void greth_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRSPW_REG {grspw_register_drv}
extern void grspw_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRSPW2_REG {grspw2_register_drv}
extern void grspw2_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRCAN_REG {grcan_register_drv}
extern void grcan_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_OCCAN_REG {occan_register_drv}
extern void occan_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GR1553B_REG {gr1553_register}
extern void gr1553_register(void);

#define DRIVER_AMBAPP_GAISLER_GR1553BC_REG {gr1553bc_register}
extern void gr1553bc_register(void);

#define DRIVER_AMBAPP_GAISLER_GR1553BM_REG {gr1553bm_register}
extern void gr1553bm_register(void);

#define DRIVER_AMBAPP_GAISLER_GR1553RT_REG {gr1553rt_register}
extern void gr1553rt_register(void);

#define DRIVER_AMBAPP_GAISLER_B1553BRM_REG {b1553brm_register_drv}
extern void b1553brm_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_B1553RT_REG {b1553rt_register_drv}
extern void b1553rt_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRTM_REG {grtm_register_drv}
extern void grtm_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRTC_REG {grtc_register_drv}
extern void grtc_register_drv(void);

#define DRIVER_AMBAPP_MCTRL_REG {mctrl_register_drv}
extern void mctrl_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_PCIF_REG {pcif_register_drv}
extern void pcif_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRPCI_REG {grpci_register_drv}
extern void grpci_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRPCI2_REG {grpci2_register_drv}
extern void grpci2_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_SPICTRL_REG {spictrl_register_drv}
extern void spictrl_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_I2CMST_REG {i2cmst_register_drv}
extern void i2cmst_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRGPIO_REG {grgpio_register_drv}
extern void grgpio_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRPWM_REG {grpwm_register_drv}
extern void grpwm_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_GRADCDAC_REG {gradcdac_register_drv}
extern void gradcdac_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_SPWCUC_REG {spwcuc_register}
extern void spwcuc_register(void);

#define DRIVER_AMBAPP_GAISLER_GRCTM_REG {grctm_register}
extern void grctm_register(void);

#define DRIVER_AMBAPP_GAISLER_SPW_ROUTER_REG {router_register_drv}
extern void router_register_drv(void);

#define DRIVER_AMBAPP_GAISLER_AHBSTAT_REG {ahbstat_register_drv}
extern void ahbstat_register_drv(void);


/*** LEON2 AMBA Hard coded bus Drivers ***/
#define DRIVER_LEON2_AT697PCI_REG {at697pci_register_drv}
extern void at697pci_register_drv(void);

#define DRIVER_LEON2_AMBAPP_REG {ambapp_leon2_register}
extern void ambapp_leon2_register(void);


/*** PCI Bus Drivers (PCI Target drivers) ***/
#define DRIVER_PCI_GR_RASTA_ADCDAC {gr_rasta_adcdac_register_drv}
extern void gr_rasta_adcdac_register_drv(void);

#define DRIVER_PCI_GR_RASTA_IO {gr_rasta_io_register_drv}
extern void gr_rasta_io_register_drv(void);

#define DRIVER_PCI_GR_RASTA_TMTC {gr_rasta_tmtc_register_drv}
extern void gr_rasta_tmtc_register_drv(void);

#define DRIVER_PCI_GR_701 {gr701_register_drv}
extern void gr701_register_drv(void);

#define DRIVER_PCI_GR_TMTC_1553 {gr_tmtc_1553_register_drv}
extern void gr_tmtc_1553_register_drv(void);

#define DRIVER_PCI_GR_RASTA_SPW_ROUTER {gr_rasta_spw_router_register_drv}
extern void gr_rasta_spw_router_register_drv(void);

#define DRIVER_PCI_GR_LEON4_N2X {gr_cpci_leon4_n2x_register_drv}
extern void gr_cpci_leon4_n2x_register_drv(void);


/* CONFIGURE DRIVER MANAGER */
struct drvmgr_drv_reg_func drvmgr_drivers[] = {
	/*** AMBA Plug & Play Drivers ***/
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
	DRIVER_AMBAPP_GAISLER_GPTIMER_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_APBUART
	DRIVER_AMBAPP_GAISLER_APBUART_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRETH
	DRIVER_AMBAPP_GAISLER_GRETH_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRSPW
	DRIVER_AMBAPP_GAISLER_GRSPW_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRSPW2
	DRIVER_AMBAPP_GAISLER_GRSPW2_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRCAN
	DRIVER_AMBAPP_GAISLER_GRCAN_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_OCCAN
	DRIVER_AMBAPP_GAISLER_OCCAN_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GR1553B
	DRIVER_AMBAPP_GAISLER_GR1553B_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GR1553BC
	DRIVER_AMBAPP_GAISLER_GR1553BC_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GR1553BM
	DRIVER_AMBAPP_GAISLER_GR1553BM_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GR1553RT
	DRIVER_AMBAPP_GAISLER_GR1553RT_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_B1553BRM
	DRIVER_AMBAPP_GAISLER_B1553BRM_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_B1553RT
	DRIVER_AMBAPP_GAISLER_B1553RT_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRTM
	DRIVER_AMBAPP_GAISLER_GRTM_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRTC
	DRIVER_AMBAPP_GAISLER_GRTC_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_PCIF
	DRIVER_AMBAPP_GAISLER_PCIF_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI
	DRIVER_AMBAPP_GAISLER_GRPCI_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPCI2
	DRIVER_AMBAPP_GAISLER_GRPCI2_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_MCTRL
	DRIVER_AMBAPP_MCTRL_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_SPICTRL
	DRIVER_AMBAPP_GAISLER_SPICTRL_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_I2CMST
	DRIVER_AMBAPP_GAISLER_I2CMST_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRGPIO
	DRIVER_AMBAPP_GAISLER_GRGPIO_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRPWM
	DRIVER_AMBAPP_GAISLER_GRPWM_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRADCDAC
	DRIVER_AMBAPP_GAISLER_GRADCDAC_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_SPWCUC
	DRIVER_AMBAPP_GAISLER_SPWCUC_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_GRCTM
	DRIVER_AMBAPP_GAISLER_GRCTM_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_SPW_ROUTER
	DRIVER_AMBAPP_GAISLER_SPW_ROUTER_REG,
#endif
#ifdef CONFIGURE_DRIVER_AMBAPP_GAISLER_AHBSTAT
	DRIVER_AMBAPP_GAISLER_AHBSTAT_REG,
#endif

	/*** LEON2 AMBA Drivers ***/
#ifdef CONFIGURE_DRIVER_LEON2_AT697PCI
	DRIVER_LEON2_AT697PCI_REG,
#endif
#ifdef CONFIGURE_DRIVER_LEON2_AMBAPP
	DRIVER_LEON2_AMBAPP_REG,
#endif

	/*** PCI Target Drivers ***/
#ifdef CONFIGURE_DRIVER_PCI_GR_RASTA_ADCDAC
	DRIVER_PCI_GR_RASTA_ADCDAC,
#endif
#ifdef CONFIGURE_DRIVER_PCI_GR_RASTA_IO
	DRIVER_PCI_GR_RASTA_IO,
#endif
#ifdef CONFIGURE_DRIVER_PCI_GR_RASTA_TMTC
	DRIVER_PCI_GR_RASTA_TMTC,
#endif
#ifdef CONFIGURE_DRIVER_PCI_GR_701
	DRIVER_PCI_GR_701,
#endif
#ifdef CONFIGURE_DRIVER_PCI_GR_TMTC_1553
	DRIVER_PCI_GR_TMTC_1553,
#endif
#ifdef CONFIGURE_DRIVER_PCI_GR_RASTA_SPW_ROUTER
	DRIVER_PCI_GR_RASTA_SPW_ROUTER,
#endif
#ifdef CONFIGURE_DRIVER_PCI_GR_LEON4_N2X
	DRIVER_PCI_GR_LEON4_N2X,
#endif


/* Macros for adding custom drivers without needing to recompile
 * kernel.
 */
#ifdef CONFIGURE_DRIVER_CUSTOM1
	DRIVER_CUSTOM1_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM2
	DRIVER_CUSTOM2_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM3
	DRIVER_CUSTOM3_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM4
	DRIVER_CUSTOM4_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM5
	DRIVER_CUSTOM5_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM6
	DRIVER_CUSTOM6_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM7
	DRIVER_CUSTOM7_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM8
	DRIVER_CUSTOM8_REG,
#endif
#ifdef CONFIGURE_DRIVER_CUSTOM9
	DRIVER_CUSTOM9_REG,
#endif

	/* End array with NULL */
	{NULL}
};

#endif /* CONFIGURE_INIT */

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_MANAGER_CONFDEFS_H_ */
