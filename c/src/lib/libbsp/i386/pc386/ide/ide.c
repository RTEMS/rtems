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
|  http://www.OARcorp.com/rtems/license.html.                     |
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
boolean pc386_ide_probe
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
|    TRUE, when flash disk available                                        |
\*=========================================================================*/
{
  boolean ide_card_plugged = TRUE; /* assume: we have a disk here */
  
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
 unsigned16                *value   /* ptr to return value location  */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  unsigned32  port = IDE_Controller_Table[minor].port1;
  unsigned8 bval1,bval2;

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
 unsigned16                 value   /* value to write                */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  unsigned32  port = IDE_Controller_Table[minor].port1;

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
 unsigned16 block_size, 
 blkdev_sg_buffer *bufs, 
 rtems_unsigned32 *cbuf,
 rtems_unsigned32 *pos
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  unsigned32  port = IDE_Controller_Table[minor].port1;
  unsigned16  cnt = 0;
  unsigned32  llength = bufs[(*cbuf)].length;
  unsigned8 status_val;
  unsigned16 *lbuf = (unsigned16 *)
    ((unsigned8 *)(bufs[(*cbuf)].buffer) + (*pos));

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
 unsigned16 block_size, 
 blkdev_sg_buffer *bufs, 
 rtems_unsigned32 *cbuf,
 rtems_unsigned32 *pos
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  unsigned32  port = IDE_Controller_Table[minor].port1;
  unsigned16  cnt = 0;
  unsigned32  llength = bufs[(*cbuf)].length;
  unsigned8   status_val;
  unsigned16 *lbuf = (unsigned16 *)
    ((unsigned8 *)(bufs[(*cbuf)].buffer) + (*pos));
    
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
 unsigned32 cmd,                    /* command to send               */
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
 int       minor,                   /* controller minor number       */
 unsigned8 modes_avail              /* optional argument             */
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

