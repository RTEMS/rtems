/*===============================================================*\
| Project: RTEMS PC386 IDE harddisc driver                        |
+-----------------------------------------------------------------+
| File: ide.c                                                     |
+-----------------------------------------------------------------+
|                    Copyright (c) 2003 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|               <Thomas.Doerfler@imd-systems.de>                  |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file contains the BSP layer for IDE access below the       |
| libchip IDE harddisc driver                                     |
| based on a board specific driver from                           |
| Eugeny S. Mints, Oktet                                          |
|                                                                 |
|  The license and distribution terms for this file may be        |
|  found in the file LICENSE in this distribution or at           |
|  http://www.rtems.com/license/LICENSE.                     |
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

/* #define DEBUG_OUT */
/*
 * support functions for IDE harddisk IF
 */
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
bool pc386_ide_probe
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  This function should probe, whether a IDE disk is available              |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int minor
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    true, when flash disk available                                        |
\*=========================================================================*/
{
  bool ide_card_plugged = true; /* assume: we have a disk here */

  return ide_card_plugged;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void pc386_ide_initialize
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  initialize IDE access                                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int  minor                              /* controller minor number       */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  /*
   * FIXME: enable interrupts, if needed
   */
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void pc386_ide_read_reg
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  read a IDE controller register                                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int                        minor,  /* controller minor number       */
 int                        reg,    /* register index to access      */
 uint16_t                  *value   /* ptr to return value location  */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t    port = IDE_Controller_Table[minor].port1;
  uint8_t   bval1,bval2;

  if (reg == IDE_REGISTER_DATA_WORD) {
    inport_byte(port+reg, bval1);
    inport_byte(port+reg+1, bval2);
    *value = bval1 + (bval2 << 8);
  }
  else {
    inport_byte(port+reg, bval1);
    *value = bval1;
  }
#ifdef DEBUG_OUT
  printk("pc386_ide_read_reg (0x%x)=0x%x\r\n",reg,*value & 0xff);
#endif
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void pc386_ide_write_reg
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  write a IDE controller register                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int                        minor,  /* controller minor number       */
 int                        reg,    /* register index to access      */
 uint16_t                   value   /* value to write                */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t    port = IDE_Controller_Table[minor].port1;

#ifdef DEBUG_OUT
  printk("pc386_ide_write_reg(0x%x,0x%x)\r\n",reg,value & 0xff);
#endif
  if (reg == IDE_REGISTER_DATA_WORD) {
    outport_word(port+reg,value);
  }
  else {
    outport_byte(port+reg,value);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void pc386_ide_read_block
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  read a IDE controller data block                                         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int minor,
 uint16_t                block_size,
 rtems_blkdev_sg_buffer *bufs,
 uint32_t               *cbuf,
 uint32_t               *pos
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t    port = IDE_Controller_Table[minor].port1;
  uint16_t    cnt = 0;
  uint32_t    llength = bufs[(*cbuf)].length;
  uint8_t   status_val;
  uint16_t   *lbuf = (uint16_t*)
    ((uint8_t*)(bufs[(*cbuf)].buffer) + (*pos));

  inport_byte(port+IDE_REGISTER_STATUS,status_val);
  while ((status_val & IDE_REGISTER_STATUS_DRQ) &&
	 (cnt < block_size)) {
    inport_word(port+IDE_REGISTER_DATA,*lbuf);

#ifdef DEBUG_OUT
    printk("0x%x ",*lbuf);
#endif
    lbuf++;
    cnt    += sizeof(*lbuf);
    (*pos) += sizeof(*lbuf);
    if ((*pos) == llength) {
      (*pos) = 0;
      (*cbuf)++;
      lbuf = bufs[(*cbuf)].buffer;
      llength = bufs[(*cbuf)].length;
    }
    inport_byte(port+IDE_REGISTER_STATUS,status_val);
  }
#ifdef DEBUG_OUT
  printk("pc386_ide_read_block()\r\n");
#endif
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void pc386_ide_write_block
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  write a IDE controller data block                                        |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int minor,
 uint16_t                block_size,
 rtems_blkdev_sg_buffer *bufs,
 uint32_t               *cbuf,
 uint32_t               *pos
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t    port = IDE_Controller_Table[minor].port1;
  uint16_t    cnt = 0;
  uint32_t    llength = bufs[(*cbuf)].length;
  uint8_t     status_val;
  uint16_t   *lbuf = (uint16_t*)
    ((uint8_t*)(bufs[(*cbuf)].buffer) + (*pos));

#ifdef DEBUG_OUT
  printk("pc386_ide_write_block()\r\n");
#endif
  inport_byte(port+IDE_REGISTER_STATUS,status_val);
  while ((status_val & IDE_REGISTER_STATUS_DRQ) &&
	 (cnt < block_size)) {
#ifdef DEBUG_OUT
    printk("0x%x ",*lbuf);
#endif
    outport_word(port+IDE_REGISTER_DATA,*lbuf);
    lbuf++;
    cnt    += sizeof(*lbuf);
    (*pos) += sizeof(*lbuf);
    if ((*pos) == llength) {
      (*pos) = 0;
      (*cbuf)++;
      lbuf = bufs[(*cbuf)].buffer;
      llength = bufs[(*cbuf)].length;
    }
    inport_byte(port+IDE_REGISTER_STATUS,status_val);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int pc386_ide_control
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  control interface for controller                                         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int  minor,                        /* controller minor number       */
 uint32_t   cmd,                    /* command to send               */
 void * arg                         /* optional argument             */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code pc386_ide_config_io_speed
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  set up transfer speed, if possible                                       |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int        minor,                   /* controller minor number       */
 uint16_t   modes_avail              /* optional argument             */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  return RTEMS_SUCCESSFUL;
}

/*
 * The following table configures the functions used for IDE drivers
 * in this BSP.
 */

ide_ctrl_fns_t pc386_ide_ctrl_fns = {
  pc386_ide_probe,
  pc386_ide_initialize,
  pc386_ide_control,
  pc386_ide_read_reg,
  pc386_ide_write_reg,
  pc386_ide_read_block,
  pc386_ide_write_block,
  pc386_ide_config_io_speed
};
