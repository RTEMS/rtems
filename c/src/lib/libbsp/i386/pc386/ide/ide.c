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

bool pc386_ide_show;

/* #define DEBUG_OUT */

static bool pc386_ide_status_busy (uint32_t port,
                                   uint32_t timeout,
                                   uint8_t* status_val)
{
  do
  {
    inport_byte (port + IDE_REGISTER_STATUS, *status_val);
    if ((*status_val & IDE_REGISTER_STATUS_BSY) == 0)
      return true;

    if (timeout)
    {
      timeout--;
      rtems_task_wake_after (TOD_MICROSECONDS_TO_TICKS (1000));
    }
  }
  while (timeout);

  return false;
}

static bool pc386_ide_status_data_ready (uint32_t port,
                                         uint32_t timeout,
                                         uint8_t* status_val)
{
  do
  {
    inport_byte (port + IDE_REGISTER_STATUS, *status_val);
    
    if (((*status_val & IDE_REGISTER_STATUS_BSY) == 0) &&
        (*status_val & IDE_REGISTER_STATUS_DRQ))
      return true;

    if (timeout)
    {
      timeout--;
      rtems_task_wake_after (TOD_MICROSECONDS_TO_TICKS (1000));
    }
  }
  while (timeout);

  return false;
}

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

static void wait(volatile uint32_t loops)
{
  while (loops)
    loops--;
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
  uint32_t port = IDE_Controller_Table[minor].port1;
  uint8_t  dev = 0;

  if (pc386_ide_show)
    printk("IDE%d: port base: %04x\n", minor, port);

  outport_byte(port+IDE_REGISTER_DEVICE_HEAD,
               (dev << IDE_REGISTER_DEVICE_HEAD_DEV_POS) | 0xE0);
  wait(10000);
  outport_byte(port+IDE_REGISTER_DEVICE_CONTROL,
               IDE_REGISTER_DEVICE_CONTROL_SRST | IDE_REGISTER_DEVICE_CONTROL_nIEN);
  wait(10000);
  outport_byte(port+IDE_REGISTER_DEVICE_CONTROL,
               IDE_REGISTER_DEVICE_CONTROL_nIEN);
  wait(10000);

  for (dev = 0; dev < 2; dev++)
  {
    uint32_t    byte;
    uint8_t     status;
    uint8_t     error;
    uint8_t     cyllsb;
    uint8_t     cylmsb;
    const char* label = dev ? " slave" : "master";
    char        model_number[41];
    char*       p = &model_number[0];

    memset(model_number, 0, sizeof(model_number));

    outport_byte(port+IDE_REGISTER_DEVICE_HEAD,
                 (dev << IDE_REGISTER_DEVICE_HEAD_DEV_POS) | 0xE0);
    /*
    outport_byte(port+IDE_REGISTER_SECTOR_NUMBER,
                 (dev << IDE_REGISTER_DEVICE_HEAD_DEV_POS) | IDE_REGISTER_LBA3_L);
    */

    outport_byte(port+IDE_REGISTER_COMMAND, 0x00);
    
    if (!pc386_ide_status_busy (port, 6000, &status))
      continue;
    
    inport_byte(port+IDE_REGISTER_STATUS,        status);
    inport_byte(port+IDE_REGISTER_ERROR,         error);
    inport_byte(port+IDE_REGISTER_CYLINDER_LOW,  cyllsb);
    inport_byte(port+IDE_REGISTER_CYLINDER_HIGH, cylmsb);

    if (pc386_ide_show)
    {
      printk("IDE%d:%s: status=%02x\n", minor, label, status);
      printk("IDE%d:%s: error=%02x\n", minor, label, error);
      printk("IDE%d:%s: cylinder-low=%02x\n", minor, label, cyllsb);
      printk("IDE%d:%s: cylinder-high=%02x\n", minor, label, cylmsb);
    }

#if 0
    /*
     * Filter based on the cylinder values and the status.
     * Taken from grub's ata.c.
     */
    if (cyllsb != 0x14 || cylmsb != 0xeb)
      if (status == 0 || (cyllsb != 0 && cylmsb != 0 &&
                          cyllsb != 0xc3 && cylmsb != 0x3c))
      {
        if (pc386_ide_show)
          printk("IDE%d:%s: bad device\n", minor, label);
      }
#endif
    
    outport_byte(port+IDE_REGISTER_COMMAND, 0xec);

    if (!pc386_ide_status_busy (port, 6000, &status))
    {
      if (pc386_ide_show)
        printk("IDE%d:%s: device busy: %02x\n", minor, label, status);
      continue;
    }
    
    byte = 0;
    while (byte < 512)
    {
      uint16_t word;
      bool     data_ready;

      if (pc386_ide_show && ((byte % 16) == 0))
        printk("\n %04x : ", byte);
      
      data_ready = pc386_ide_status_data_ready (port, 100, &status);

      if (status & IDE_REGISTER_STATUS_ERR)
      {
        inport_byte(port+IDE_REGISTER_ERROR, error);
        if (error != 4)
        {
          if (pc386_ide_show)
            printk("IDE%d:%s: error=%04x\n", minor, label, error);
          break;
        }
        /*
         * The device is an ATAPI device.
         */
        outport_byte(port+IDE_REGISTER_COMMAND, 0xa1);
        continue;
      }

      if (!data_ready)
        break;
      
      inport_word(port+IDE_REGISTER_DATA, word);

      if (pc386_ide_show)
        printk ("%04x ", word);

      if (byte >= 54 && byte < (54 + 40))
      {
        *p = word >> 8;
        p++;
        *p = word;
        p++;
      }
        
      byte += 2;
    }
    
    if (pc386_ide_show)
      printk("\nbytes read = %d\n", byte);

    if (p != &model_number[0])
      printk("IDE%d:%s: %s\n", minor, label, model_number);
  }
  
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
  uint8_t     status_val;
  uint16_t   *lbuf = (uint16_t*)
    ((uint8_t*)(bufs[(*cbuf)].buffer) + (*pos));

  while (cnt < block_size)
  {
    if (!pc386_ide_status_data_ready (port, 100, &status_val))
    {
      printk ("pc386_ide_read_block: status=%02x, cnt=%d bs=%d\n", status_val, cnt, block_size);
      /* FIXME: add an error here. */
      return;
    }
    
    if (status_val & IDE_REGISTER_STATUS_ERR)
      printk("pc386_ide_read_block: error: %02x\n", status_val);
    
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
  }
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
  printk("pc386_ide_write_block()\n");
#endif

  while (cnt < block_size)
  {
    if (!pc386_ide_status_data_ready (port, 100, &status_val))
    {
      printk ("pc386_ide_write_block: status=%02x, cnt=%d bs=%d\n", status_val, cnt, block_size);
      /* FIXME: add an error here. */
      return;
    }
    
    if (status_val & IDE_REGISTER_STATUS_ERR)
      printk("pc386_ide_write_block: error: %02x\n", status_val);
    
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
