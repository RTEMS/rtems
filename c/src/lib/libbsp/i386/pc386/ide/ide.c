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

#define ATA_SECTOR_SIZE (512)

/*
 * Use during initialisation.
 */
extern void Wait_X_ms(unsigned int msecs);

bool pc386_ide_show;
uint32_t pc386_ide_timeout;

#define PC386_IDE_DEBUG_OUT 0

#if PC386_IDE_DEBUG_OUT
bool pc386_ide_trace;
#define pc386_ide_printk if (pc386_ide_trace) printk
#endif

#define PC386_IDE_PROBE_TIMEOUT    (500)
#define PC386_IDE_PRESTART_TIMEOUT (1000)
#define PC386_IDE_TASKING_TIMEOUT  (2000)

/*
 * Prestart sleep using a calibrated timing loop.
 */
static void pc386_ide_prestart_sleep (void)
{
  Wait_X_ms (10);
}

/*
 * Once tasking has started we use a task sleep.
 */
static void pc386_ide_tasking_sleep (void)
{
  rtems_task_wake_after (TOD_MICROSECONDS_TO_TICKS (10000) ?
                         TOD_MICROSECONDS_TO_TICKS (10000) : 1);
}

typedef void (*pc386_ide_sleeper)(void);

static void pc386_ide_sleep (pc386_ide_sleeper sleeper)
{
  sleeper ();
}

static void wait(volatile uint32_t loops)
{
  while (loops)
    loops--;
}

static bool pc386_ide_status_busy (uint32_t          port,
                                   volatile uint32_t timeout,
                                   uint8_t*          status_val,
                                   pc386_ide_sleeper sleeper)
{
  volatile uint8_t status;
  int              polls;

  do
  {
    polls = 500;
    while (polls)
    {
      inport_byte (port + IDE_REGISTER_STATUS, status);
      if ((status & IDE_REGISTER_STATUS_BSY) == 0)
      {
        *status_val = status;
        return true;
      }
      polls--;
    }

    if (timeout)
    {
      timeout--;
      pc386_ide_sleep (sleeper);
    }
  }
  while (timeout);

  *status_val = status;
  return false;
}

static bool pc386_ide_status_data_ready (uint32_t          port,
                                         volatile uint32_t timeout,
                                         uint8_t*          status_val,
                                         pc386_ide_sleeper sleeper)
{
  volatile uint8_t status;
  int              polls;

  do
  {
    polls = 1000;
    while (polls)
    {
      inport_byte (port + IDE_REGISTER_STATUS, status);

      if (((status & IDE_REGISTER_STATUS_BSY) == 0) &&
          (status & IDE_REGISTER_STATUS_DRQ))
      {
        *status_val = status;
        return true;
      }

      polls--;
    }

    if (timeout)
    {
      timeout--;
      pc386_ide_sleep (sleeper);
    }
  }
  while (timeout);

  *status_val = status;
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
    uint16_t    capabilities = 0;
    uint32_t    byte;
    uint8_t     status;
    uint8_t     error;
    uint8_t     cyllsb;
    uint8_t     cylmsb;
    const char* label = dev ? " slave" : "master";
    int         max_multiple_sectors = 0;
    int         cur_multiple_sectors = 0;
    uint32_t    cylinders = 0;
    uint32_t    heads = 0;
    uint32_t    sectors = 0;
    uint32_t    lba_sectors = 0;
    char        model_number[41];
    char*       p = &model_number[0];
    bool        data_ready;

    memset(model_number, 0, sizeof(model_number));

    outport_byte(port+IDE_REGISTER_DEVICE_HEAD,
                 (dev << IDE_REGISTER_DEVICE_HEAD_DEV_POS) | 0xE0);
    /*
      outport_byte(port+IDE_REGISTER_SECTOR_NUMBER,
      (dev << IDE_REGISTER_DEVICE_HEAD_DEV_POS) | IDE_REGISTER_LBA3_L);
    */

    outport_byte(port+IDE_REGISTER_COMMAND, 0x00);

    if (!pc386_ide_status_busy (port, PC386_IDE_PROBE_TIMEOUT,
                                &status, pc386_ide_prestart_sleep))
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

    outport_byte(port+IDE_REGISTER_COMMAND, 0xec);

    if (!pc386_ide_status_busy (port, PC386_IDE_PRESTART_TIMEOUT,
                                &status, pc386_ide_prestart_sleep))
    {
      if (pc386_ide_show)
        printk("IDE%d:%s: device busy: %02x\n", minor, label, status);
      continue;
    }

    data_ready = pc386_ide_status_data_ready (port,
                                              250,
                                              &status,
                                              pc386_ide_prestart_sleep);

    if (status & IDE_REGISTER_STATUS_ERR)
    {
      inport_byte(port+IDE_REGISTER_ERROR, error);
      if (error != 4)
      {
        if (pc386_ide_show)
          printk("IDE%d:%s: error=%04x\n", minor, label, error);
        continue;
      }
      /*
       * The device is an ATAPI device.
       */
      outport_byte(port+IDE_REGISTER_COMMAND, 0xa1);
      data_ready = pc386_ide_status_data_ready (port,
                                                250,
                                                &status,
                                                pc386_ide_prestart_sleep);
    }

    if (!data_ready)
      continue;

    byte = 0;
    while (byte < 512)
    {
      uint16_t word;

      if (pc386_ide_show && ((byte % 16) == 0))
        printk("\n %04x : ", byte);

      inport_word(port+IDE_REGISTER_DATA, word);

      if (pc386_ide_show)
        printk ("%04x ", word);

      if (byte == 2)
        cylinders = word;
      if (byte == 6)
        heads = word;
      if (byte == 12)
        sectors = word;

      if (byte >= 54 && byte < (54 + 40))
      {
        *p = word >> 8;
        p++;
        *p = word;
        p++;
      }

      if (byte == (47 * 2))
        max_multiple_sectors = word & 0xff;

      if (byte == (49 * 2))
        capabilities = word;

      if (byte == (59 * 2))
      {
        if (word & (1 << 8))
          cur_multiple_sectors = word & 0xff;
      }

      if (byte == (60 * 2))
        lba_sectors = word;
      if (byte == (61 * 2))
        lba_sectors |= word << 16;

      byte += 2;
    }

    if (pc386_ide_show)
      printk("\nbytes read = %d\n", byte);

    if (p != &model_number[0])
    {
      uint32_t size;
      uint32_t left;
      uint32_t right;
      char     units;

      if (capabilities & (1 << 9))
        size = lba_sectors;
      else
        size = cylinders * heads * sectors;

      size /= 2;

      if (size > (1024 * 1024))
      {
        size = (size * 10) / (1000 * 1000);
        units = 'G';
      }
      else if (size > 1024)
      {
        size = (size * 10) / 1000;
        units = 'M';
      }
      else
      {
        size = size * 10;
        units = 'K';
      }

      left = size / 10;
      right = size % 10;

      p--;
      while (*p == ' ')
      {
        *p = '\0';
        p--;
      }

      printk("IDE%d:%s:%s, %u.%u%c (%u/%u/%u), max blk size:%d\n",
             minor, label, model_number, left, right, units,
             heads, cylinders, sectors, max_multiple_sectors * 512);
    }

#if IDE_CLEAR_MULTI_SECTOR_COUNT
    if (max_multiple_sectors)
    {
      outport_byte(port+IDE_REGISTER_SECTOR_COUNT, 0);
      outport_byte(port+IDE_REGISTER_COMMAND, 0xc6);

      if (!pc386_ide_status_busy (port, PC386_IDE_PRESTART_TIMEOUT,
                                  &status, pc386_ide_prestart_sleep))
      {
        if (pc386_ide_show)
          printk("IDE%d:%s: device busy: %02x\n", minor, label, status);
        continue;
      }

      inport_byte(port+IDE_REGISTER_STATUS, status);
      if (status & IDE_REGISTER_STATUS_ERR)
      {
        inport_byte(port+IDE_REGISTER_ERROR, error);
        if (error & IDE_REGISTER_ERROR_ABRT)
          printk("IDE%d:%s: disable multiple failed\n", minor, label);
        else
          printk("IDE%d:%s: unknown error on disable multiple: %02x\n",
                 minor, label, error);
      }
    }
#endif

    outport_byte(port+IDE_REGISTER_DEVICE_CONTROL,
                 IDE_REGISTER_DEVICE_CONTROL_nIEN);
    wait(10000);
  }

  pc386_ide_timeout = PC386_IDE_TASKING_TIMEOUT;

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
#if PC386_IDE_DEBUG_OUT
  pc386_ide_printk("pc386_ide_read_reg (0x%x)=0x%x\r\n",reg,*value & 0xff);
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

#if PC386_IDE_DEBUG_OUT
  pc386_ide_printk("pc386_ide_write_reg(0x%x,0x%x)\r\n",reg,value & 0xff);
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
 int                     minor,
 uint32_t                block_size,
 rtems_blkdev_sg_buffer *bufs,
 uint32_t               *cbuf,
 uint32_t               *pos
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t port = IDE_Controller_Table[minor].port1;
  uint32_t cnt = 0;
#if PC386_IDE_DEBUG_OUT
  int i32 = 0;
  pc386_ide_printk("pc386_ide_read_block(bs=%u,bn=%u,bl=%u,cb=%d,p=%d)\n",
                   block_size, bufs[(*cbuf)].block, llength, *cbuf, *pos);
#endif

  while (cnt < block_size)
  {
    uint16_t *lbuf;
    uint8_t  status_val;
    int      b;

    if (!pc386_ide_status_data_ready (port, pc386_ide_timeout,
                                      &status_val, pc386_ide_tasking_sleep))
    {
      printk ("pc386_ide_read_block: block=%u cbuf=%u status=%02x, cnt=%d bs=%d\n",
              bufs[*cbuf].block, *cbuf, status_val, cnt, block_size);
      /* FIXME: add an error here. */
      return;
    }

    if (status_val & IDE_REGISTER_STATUS_ERR)
    {
      inport_byte(port+IDE_REGISTER_ERROR, status_val);
      printk("pc386_ide_read_block: error: %02x\n", status_val);
      return;
    }

    lbuf = (uint16_t*)((uint8_t*)(bufs[(*cbuf)].buffer) + (*pos));

    for (b = 0; b < (ATA_SECTOR_SIZE / 2); b++)
    {
      inport_word(port+IDE_REGISTER_DATA,*lbuf);

#if PC386_IDE_DEBUG_OUT
      pc386_ide_printk("%04x ",*lbuf);
      i32++;
      if (i32 >= 16)
      {
        pc386_ide_printk("\n");
        i32 = 0;
      }
#endif
      lbuf++;
    }
    cnt    += ATA_SECTOR_SIZE;
    (*pos) += ATA_SECTOR_SIZE;
    if ((*pos) == bufs[(*cbuf)].length) {
      (*pos) = 0;
      (*cbuf)++;
      lbuf = bufs[(*cbuf)].buffer;
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
 uint32_t                block_size,
 rtems_blkdev_sg_buffer *bufs,
 uint32_t               *cbuf,
 uint32_t               *pos
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  uint32_t port = IDE_Controller_Table[minor].port1;
  uint32_t cnt = 0;
#if PC386_IDE_DEBUG_OUT
  int i32 = 0;
  pc386_ide_printk("pc386_ide_write_block(bs=%u,bn=%u,bl=%u,cb=%d,p=%d)\n",
                   block_size, bufs[(*cbuf)].block, llength, *cbuf, *pos);
#endif

  while (cnt < block_size)
  {
    uint16_t *lbuf;
    uint8_t  status_val;
    int      b;

    if (!pc386_ide_status_data_ready (port, pc386_ide_timeout,
                                      &status_val, pc386_ide_tasking_sleep))
    {
      printk ("pc386_ide_write_block: block=%u status=%02x, cnt=%d bs=%d\n",
              bufs[*cbuf].block, status_val, cnt, block_size);
      /* FIXME: add an error here. */
      return;
    }

    if (status_val & IDE_REGISTER_STATUS_ERR)
    {
      inport_byte(port+IDE_REGISTER_ERROR, status_val);
      printk ("pc386_ide_write_block: error: %02x\n", status_val);
      return;
    }

    lbuf = (uint16_t*)(((uint8_t*)bufs[*cbuf].buffer) + (*pos));

    for (b = 0; b < (ATA_SECTOR_SIZE / 2); b++)
    {
#if PC386_IDE_DEBUG_OUT
      pc386_ide_printk("%04x ",*lbuf);
      i32++;
      if (i32 >= 16)
      {
        pc386_ide_printk("\n");
        i32 = 0;
      }
#endif
      outport_word(port+IDE_REGISTER_DATA,*lbuf);
      lbuf++;
    }
    cnt    += ATA_SECTOR_SIZE;
    (*pos) += ATA_SECTOR_SIZE;
    if ((*pos) == bufs[(*cbuf)].length) {
      (*pos) = 0;
      (*cbuf)++;
      lbuf = bufs[(*cbuf)].buffer;
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
