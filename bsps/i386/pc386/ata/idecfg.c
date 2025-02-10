/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS PC386 IDE harddisc driver tables
 *
 * This file contains the table of functions for the BSP layer
 * for IDE access below the libchip IDE harddisc driver.
 */

/*
 * Copyright (C) 2003 IMD Ingenieurbuero fuer Microcomputertechnik
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
#include <libchip/ide_ctrl.h>
#include <libchip/ide_ctrl_cfg.h>
#include <libchip/ide_ctrl_io.h>

extern bool pc386_ide_show;

/*
 * The following table configures the functions used for IDE drivers
 * in this BSP.
 */

/*
 * The following table configures the IDE drivers used in this BSP.
 */
extern ide_ctrl_fns_t pc386_ide_ctrl_fns;

/* IDE controllers Table */
ide_controller_bsp_table_t IDE_Controller_Table[] = {
  {"/dev/ide0",
   IDE_STD, /* standard IDE controller */
   &pc386_ide_ctrl_fns,
   NULL, /* probe for IDE standard registers */
   FALSE, /* not (yet) initialized */
   0x1f0,  /* base I/O address for first IDE controller */
   FALSE,0, /* not (yet) interrupt driven */
   NULL
  }
  , /* colon only needed when both interfaces present */
  {"/dev/ide1",
   IDE_STD, /* standard IDE controller */
   &pc386_ide_ctrl_fns,
   NULL, /* probe for IDE standard registers */
   FALSE, /* not (yet) initialized */
   0x170,  /* base I/O address for second IDE controller */
   FALSE,0, /* not (yet) interrupt driven */
   NULL
  }
};

/* Number of rows in IDE_Controller_Table. Default is 0. */
unsigned long IDE_Controller_Count;

#if IDE_USE_PRIMARY_INTERFACE
#define IDE1_DEFAULT true
#else
#define IDE1_DEFAULT false
#endif
#if IDE_USE_SECONDARY_INTERFACE
#define IDE2_DEFAULT true
#else
#define IDE2_DEFAULT false
#endif

void bsp_ide_cmdline_init(void)
{
  const char* ide_disable;

  ide_disable = bsp_cmdline_arg ("--ide-disable");

  if (ide_disable == NULL) {
    bool ide1 = IDE1_DEFAULT;
    bool ide2 = IDE2_DEFAULT;
    const char* ide;

    /*
     * Can have:
     *  --ide=0,1
     */
    ide = bsp_cmdline_arg ("--ide=");

    if (ide)
    {
      int i;
      /*
       * If a command line option exists remove the defaults.
       */
      ide1 = ide2 = false;

      ide += sizeof ("--ide=") - 1;

      for (i = 0; i < 3; i++)
      {
        switch (ide[i])
        {
          case '0':
            ide1 = true;
            break;
          case '1':
            ide2 = true;
            break;
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
          case ',':
            break;
          default:
            break;
        }
      }
    }

    if (ide2 && !ide1)
      IDE_Controller_Table[0] = IDE_Controller_Table[1];

    if (ide1)
      IDE_Controller_Count++;
    if (ide2)
      IDE_Controller_Count++;

    /*
     * Allow the user to get the initialise to print probing
     * type information.
     */
    ide = bsp_cmdline_arg ("--ide-show");

    if (ide)
      pc386_ide_show = true;
  }
}
