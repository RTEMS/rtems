/*
 * RTEMS for Nintendo DS flash driver.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <rtems.h>
#include <bsp.h>
#include <libchip/ide_ctrl.h>
#include <libchip/ide_ctrl_cfg.h>
#include <libchip/ide_ctrl_io.h>

#include <disc.h>

bool
nds_flash_probe (int minor)
{
  return true;
}

void
nds_flash_initialize (int minor)
{
  const IO_INTERFACE *flash;

  printk ("[+] flash started\n");

  flash = _FAT_disc_dsSlotFindInterface ();
  if (flash == NULL) {
    printk ("[!] error getting device\n");
    rtems_fatal_error_occurred (0);
  }

  if (_FAT_disc_isInserted (flash)) {
    printk ("[#] flash inserted\n");
  } else {
    printk ("[!] flash not inserted\n");
  }
}

void
nds_flash_read_reg (int minor, int reg, uint16_t * value)
{
  printk ("nds_flash_read_reg\n");
}

void
nds_flash_write_reg (int minor, int reg, uint16_t value)
{
  printk ("nds_flash_write_reg\n");
}

void
nds_flash_read_block (int minor, uint32_t block_size,
                      rtems_blkdev_sg_buffer * bufs,
                      uint32_t * cbuf, uint32_t * pos)
{
  printk ("nds_flash_read_block\n");
}

void
nds_flash_write_block (int minor, uint32_t block_size,
                       rtems_blkdev_sg_buffer * bufs,
                       uint32_t * cbuf, uint32_t * pos)
{
  printk ("nds_flash_write_block\n");
}

int
nds_flash_control (int minor, uint32_t cmd, void *arg)
{
  printk ("nds_flash_control\n");
  return 0;
}

rtems_status_code
nds_flash_io_speed (int minor, uint16_t mode)
{
  return RTEMS_SUCCESSFUL;
}

ide_ctrl_fns_t nds_flash_ctrl_fns = {
  nds_flash_probe,
  nds_flash_initialize,
  nds_flash_control,
  nds_flash_read_reg,
  nds_flash_write_reg,
  nds_flash_read_block,
  nds_flash_write_block,
  nds_flash_io_speed
};

/* IDE controllers Table */
ide_controller_bsp_table_t IDE_Controller_Table[] = {
  {
   "/dev/flash",
   IDE_CUSTOM,                  /* standard IDE controller */
   &nds_flash_ctrl_fns,
   NULL,                        /* probe for IDE standard registers */
   FALSE,                       /* not (yet) initialized */
   0x0,                         /* base I/O address for first IDE controller */
   FALSE, 0,                    /* not (yet) interrupt driven */
   NULL
  }
};

/* Number of rows in IDE_Controller_Table */
unsigned long IDE_Controller_Count = 1;
