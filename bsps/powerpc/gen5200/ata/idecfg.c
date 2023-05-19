/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic MPC5200 BSP
 *
 * This file contains the IDE configuration.
 */

/*
 * Copyright (c) 2005 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/mpc5200.h>
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


