/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2005                           |
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
| this file contains the IDE configuration                        |
\*===============================================================*/
#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include "../include/mpc5200.h"
#include "./pcmcia_ide.h"

#include <libchip/ide_ctrl.h>
#include <libchip/ide_ctrl_cfg.h>
#include <libchip/ide_ctrl_io.h>

/*
 * The following table configures the IDE driver used in this BSP.
 */
extern ide_ctrl_fns_t mpc5200_pcmciaide_ctrl_fns;

volatile uint32_t * mpc5200_ata_drive_regs[] =
  {
  (uint32_t *)&(mpc5200.ata_ddr),       /* data (offset 0x00)                */
  (uint32_t *)&(mpc5200.ata_dfr_der),   /* features / error (offset 0x01)    */
  (uint32_t *)&(mpc5200.ata_dscr),      /* sector count (offset 0x02)        */
  (uint32_t *)&(mpc5200.ata_dsnr),      /* sector no.   / lba0 (offset 0x03) */
  (uint32_t *)&(mpc5200.ata_dclr),      /* cylinder low / lba1 (offset 0x04) */
  (uint32_t *)&(mpc5200.ata_dchr),      /* cylinder high/ lba2 (offset 0x05) */
  (uint32_t *)&(mpc5200.ata_ddhr),      /* device head  / lba3 (offset 0x06) */
  (uint32_t *)&(mpc5200.ata_dcr_dsr),   /* command /status (offset 0x07)     */

  (uint32_t *)&(mpc5200.ata_dctr_dasr), /* device control / alternate status (offset 0x08) */
  (uint32_t *)&(mpc5200.ata_ddr),       /* (offset 0x09) */
  (uint32_t *)&(mpc5200.ata_ddr),       /* (offset 0x0A) */
  NULL,                                 /* (offset 0x0B) */
  NULL,                                 /* (offset 0x0C) */
  NULL,                                 /* (offset 0x0D) */
  NULL,                                 /* (offset 0x0E) */
  NULL                                  /* (offset 0x0F) */
  };

/* IDE controllers Table */
ide_controller_bsp_table_t IDE_Controller_Table[] =
  {
    {
	"/dev/idepcmcia",
    IDE_CUSTOM,                                        /* PCMCIA Flash cards emulate custom IDE controller */
    &mpc5200_pcmciaide_ctrl_fns,                       /* pointer to function set used for IDE drivers in this BSP */
    NULL,                                              /* no BSP dependent probe needed */
    FALSE,                                             /* not (yet) initialized */
    (uint32_t)0,                                     /* no port address but custom reg.set in params is used */
#ifdef ATA_USE_INT
    TRUE,                                              /* interrupt driven */
#else
    FALSE,                                             /* non interrupt driven     */
#endif
    BSP_SIU_IRQ_ATA,                                   /* interrupt vector         */
    NULL                                               /* no additional parameters */
    }
};

/* Number of rows in IDE_Controller_Table */
unsigned long IDE_Controller_Count = sizeof(IDE_Controller_Table)/sizeof(IDE_Controller_Table[0]);

uint32_t ata_pio_timings[2][6] =
 {
   /* PIO3 timings in nanosconds */
   {
   180, /* t0 cycle time */
   80,  /* t2 DIOR-/DIOW pulse width 8 bit */
   80,  /* t1 DIOR-/DIOW pulse width 16 bit */
   10,  /* t4 DIOW- data hold */
   30,  /* t1 Addr.valid to DIOR-/DIOW setup */
   35,  /* ta IORDY setup time */
   },
   /* PIO4 timings in nanosconds */
   {
   120, /* t0 cycle time */
   70,  /* t1 DIOR-/DIOW pulse width 8 bit */
   70,  /* t1 DIOR-/DIOW pulse width 16 bit */
   10,  /* t4 DIOW- data hold */
   25,  /* t1 Addr.valid to DIOR-/DIOW setup */
   35,  /* ta IORDY setup time */
   }
 };


