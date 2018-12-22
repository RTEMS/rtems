/*  GR-RASTA-TMTC PCI Target driver.
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  Configures the GR-RASTA-TMTC interface PCI board.
 *  This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *  Driver resources for the AMBA PnP bus provided can be set using
 *  gr_rasta_tmtc_set_resources().
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GR_RASTA_TMTC_H__
#define __GR_RASTA_TMTC_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* GPIO TM/TC configuration pin definitions
 * --31         PWRX                (1=PW2APB,     0=TM VC3/4)
 * --30         PWTC                (1=APB2PW,     0=TC MAP1/2)
 * --29         Redundant TM        (1=enable,     0=disable)
 * --28         Redundant TC        (1=enable,     0=disable)
 * --27         Select TM output    (1=GRTM,       0=PTME)
 * --26         Loop back PW        (1=enable,     0=disable)
 * --25         Transponder clock   (1=PLL,        0=PLL bypass)
 * --24         PWTX-SELECT         (0=TX0-0,      1=TX0-1)
 * --23         PDEC Map Switch     (1=on,         0=off)
 * --22         PDEC Ext CPDU       (1=on,         0=off)
 * --21         PDEC Super User     (1=on,         0=off)
 * --20         PDEC RM On          (1=on,         0=off)
 * --19         PDEC AU Enable      (1=on,         0=off)
 * --18         PDEC Dynamic Mode   (1=on,         0=off)
 * --17         PDEC Priority       (1=on,         0=off)
 * --16         TC PSS Support      (1=on,         0=off)
 * --15         TC Mark             (1=on,         0=off)
 * --14         TC Pseudo           (1=on,         0=off)
 * --13         TC Rising Clock     (1=rise,       0=fall)
 * --12         TC Active High      (1=high,       0=low)
 * --11         Bit Lock Positive   (1=high,       0=low)
 * --10         RF Avail Positive   (1=high,       0=low)
 * -- 9 : 0     SpaceCraft ID 
 */

#define GR_TMTC_GPIO_PWRX		(1<<31)
#define GR_TMTC_GPIO_PWTC		(1<<30)
#define GR_TMTC_GPIO_RED_TM		(1<<29)
#define GR_TMTC_GPIO_RED_TC		(1<<28)
#define GR_TMTC_GPIO_GRTM_SEL		(1<<27)
#define GR_TMTC_GPIO_LB_PW		(1<<26)
#define GR_TMTC_GPIO_TRANSP_CLK		(1<<25)
#define GR_TMTC_GPIO_PWTX_SEL		(1<<24)
#define GR_TMTC_GPIO_PDEC_MAP		(1<<23)
#define GR_TMTC_GPIO_PDEC_CPDU		(1<<22)
#define GR_TMTC_GPIO_PDEC_SU		(1<<21)
#define GR_TMTC_GPIO_PDEC_RM		(1<<20)
#define GR_TMTC_GPIO_PDEC_AU		(1<<19)
#define GR_TMTC_GPIO_PDEC_DYN_MODE	(1<<18)
#define GR_TMTC_GPIO_PDEC_PRIO		(1<<17)
#define GR_TMTC_GPIO_TC_PSS		(1<<16)
#define GR_TMTC_GPIO_TC_MARK		(1<<15)
#define GR_TMTC_GPIO_TC_PSEUDO		(1<<14)
#define GR_TMTC_GPIO_TC_RISING_CLK	(1<<13)
#define GR_TMTC_GPIO_TC_ACTIVE_HIGH	(1<<12)
#define GR_TMTC_GPIO_TC_BIT_LOCK	(1<<11)
#define GR_TMTC_GPIO_TC_RF_AVAIL	(1<<10)
#define GR_TMTC_GPIO_SCID		(0x000003ff)

/* An array of pointers to GR-RASTA-TMTC bus resources. The resources will be
 * used by the device drivers controlling the cores on the GR-RASTA-IO target
 * AMBA bus.
 *
 * The array is defined weak, and defaults to no resources. The array must be
 * terminated with a NULL resource.
 */
extern struct drvmgr_bus_res *gr_rasta_tmtc_resources[];

/* Options to gr_rasta_io_print function */
#define RASTA_TMTC_OPTIONS_AMBA   0x01 /* Print AMBA bus devices */
#define RASTA_TMTC_OPTIONS_IRQ    0x02 /* Print current IRQ setup */

/* Print information about GR-RASTA-TMTC PCI boards */
void gr_rasta_tmtc_print(int options);

/* Print information about a GR-RASTA-TMTC PCI boards */
void gr_rasta_tmtc_print_dev(struct drvmgr_dev *dev, int options);

/* Register GR-RASTA-TMTC driver */
void gr_rasta_tmtc_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif
