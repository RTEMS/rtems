/*===============================================================*\
| Project: RTEMS PC386 IDE harddisc driver tables                 |
+-----------------------------------------------------------------+
| File: idecfg.c                                                  |
+-----------------------------------------------------------------+
|                    Copyright (c) 2003 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|               <Thomas.Doerfler@imd-systems.de>                  |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file contains the table of functions for the BSP layer     |
| for IDE access below the libchip IDE harddisc driver            |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 01.14.03  creation                                         doe  |
\*===============================================================*/

#include <rtems.h>
#include <bsp.h>
#include <libchip/ide_ctrl.h>
#include <libchip/ide_ctrl_cfg.h>
#include <libchip/ide_ctrl_io.h>


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
  {"/dev/ide",
   IDE_STD, /* standard IDE controller */
   &pc386_ide_ctrl_fns,
   NULL, /* probe for IDE standard registers */
   FALSE, /* not (yet) initialized */
   0x1f0,  /* base I/O address for first IDE controller */
   FALSE,0, /* not (yet) interrupt driven */
   NULL
  }
};

/* Number of rows in IDE_Controller_Table */
unsigned long IDE_Controller_Count = 
   sizeof(IDE_Controller_Table)/sizeof(IDE_Controller_Table[0]);


