/*
 * Copyright (c) 2010-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define NDEBUG

#include <bsp/ata.h>

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/mpc5200.h>

#include <libcpu/powerpc-utility.h>

bool ata_execute_io_command(uint8_t command, uint32_t lba, uint32_t sector_count_32)
{
  assert(sector_count_32 >= 1);
  assert(sector_count_32 <= 256);

  assert(ata_is_drive_ready_for_selection());

  ATA->write.head = (uint8_t) (0xe0 | ((lba >> 24) & 0x0f));

  ata_wait_400_nano_seconds();
  ata_wait_for_drive_ready();

  /*
   * It seems that the write to the ATA device registers is sometimes not
   * successful.  The write is retried until the read back values are all right
   * or a timeout is reached.
   */
  bool ok = false;
  uint8_t sector = (uint8_t) lba;
  uint8_t cylinder_low = (uint8_t) (lba >> 8);
  uint8_t cylinder_high = (uint8_t) (lba >> 16);
  uint8_t sector_count = (uint8_t) sector_count_32;
  int i;
  for (i = 0; !ok && i < 100; ++i) {
    ATA->write.sector = sector;
    ATA->write.cylinder_low = cylinder_low;
    ATA->write.cylinder_high = cylinder_high;
    ATA->write.sector_count = sector_count;

    uint8_t actual_sector = ATA->read.sector;
    uint8_t actual_cylinder_low = ATA->read.cylinder_low;
    uint8_t actual_cylinder_high = ATA->read.cylinder_high;
    uint8_t actual_sector_count = ATA->read.sector_count;

    ok = actual_cylinder_high == cylinder_high
      && actual_cylinder_low == cylinder_low
      && actual_sector == sector
      && actual_sector_count == sector_count;
  }

  if (ok) {
    ATA->write.command = command;
  }

  return ok;
}

void ata_reset_device(void)
{
  /* ATA/ATAPI-7 V2, 11.2 Software reset protocol */
  ATA->write.control = DCTRL_SRST;
  rtems_bsp_delay(5);
  ATA->write.control = 0;
  rtems_bsp_delay(2000);
  ata_wait_for_not_busy();
}

bool ata_set_transfer_mode(uint8_t mode)
{
  assert(ata_is_drive_ready_for_selection());

  ATA->write.head = 0xe0;

  ata_wait_400_nano_seconds();
  ata_wait_for_drive_ready();

  ATA->write.feature = 0x3;
  ATA->write.sector_count = mode;
  ATA->write.command = 0xef;

  ata_wait_for_not_busy();

  return ata_check_status();
}

static bool probe(void)
{
  bool card_present = true;

#ifdef MPC5200_BOARD_BRS5L
  volatile struct mpc5200_gpt *gpt = &mpc5200.gpt[GPT2];

  /* Enable card detection on GPT2 */
  gpt->emsel = (GPT_EMSEL_GPIO_IN | GPT_EMSEL_TIMER_MS_GPIO);

  /* Check for card detection (-CD0) */
  if ((gpt->status & GPT_STATUS_PIN) != 0) {
    card_present = false;
  }
#endif

  return card_present;
}

static void create_lock(ata_driver *self)
{
  rtems_status_code sc = rtems_semaphore_create(
    rtems_build_name('A', 'T', 'A', ' '),
    1,
    RTEMS_LOCAL | RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &self->lock
  );
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(MPC5200_FATAL_ATA_LOCK_CREATE);
  }
}

static void destroy_lock(const ata_driver *self)
{
  rtems_status_code sc = rtems_semaphore_delete(self->lock);
  if (sc != RTEMS_SUCCESSFUL) {
    bsp_fatal(MPC5200_FATAL_ATA_LOCK_DESTROY);
  }
}

void ata_driver_create(ata_driver *self, const char *device_file_path, rtems_block_device_ioctl io_control)
{
  self->card_present = probe();

  if (ata_driver_is_card_present(self)) {
    create_lock(self);
    ata_reset_device();

    uint16_t sector_buffer[256];
    ata_dev_t ata_device;
    rtems_status_code sc = ata_identify_device(0, 0, sector_buffer, &ata_device);

    if (sc == RTEMS_SUCCESSFUL && ata_device.lba_avaible) {
      sc = ide_controller_config_io_speed(0, ata_device.modes_available);

      if (sc == RTEMS_SUCCESSFUL) {
        sc = rtems_blkdev_create(
          device_file_path,
          ATA_SECTOR_SIZE,
          ata_device.lba_sectors,
          io_control,
          self
        );

        if (sc != RTEMS_SUCCESSFUL) {
          bsp_fatal(MPC5200_FATAL_ATA_DISK_CREATE);
        }
      }
    }
  }
}

void ata_driver_destroy(ata_driver *self)
{
  destroy_lock(self);
  /* TODO */
  assert(0);
}

static bool transfer_pio_polled(ata_driver *self, bool read, rtems_blkdev_sg_buffer *sg, size_t sg_count)
{
  bool ok = true;

  ata_sg_context sg_context;
  ata_sg_create(&sg_context, sg, sg_count);
  rtems_blkdev_bnum start_sector = ata_sg_get_start_sector(&sg_context);
  rtems_blkdev_bnum sector_count = ata_sg_get_sector_count(&sg_context);
  rtems_blkdev_bnum relative_sector = 0;

  uint8_t command = ata_read_or_write_sectors_command(read);

  while (ok && relative_sector < sector_count) {
    rtems_blkdev_bnum remaining_sectors = sector_count - relative_sector;
    rtems_blkdev_bnum transfer_count = ata_max_transfer_count(remaining_sectors);
    rtems_blkdev_bnum transfer_end = relative_sector + transfer_count;

    ok = ata_execute_io_command(command, start_sector + relative_sector, transfer_count);

    rtems_blkdev_bnum transfer;
    for (transfer = relative_sector; ok && transfer < transfer_end; ++transfer) {
      uint16_t *current = ata_sg_get_sector_data_begin(&sg_context, transfer);
      uint16_t *end = ata_sg_get_sector_data_end(&sg_context, current);

      ok = ata_wait_for_data_request();

      if (ok) {
        if (read) {
          while (current != end) {
            *current = ATA->read.data;
            ++current;
          }
        } else {
          while (current != end) {
            ATA->write.data = *current;
            ++current;
          }
        }
      }
    }

    if (ok) {
      ata_wait_for_not_busy();
      ok = ata_check_status();
    }

    relative_sector += ATA_PER_TRANSFER_SECTOR_COUNT_MAX;
  }

  return ok;
}

int ata_driver_io_control_pio_polled(
  rtems_disk_device *dd,
  uint32_t cmd,
  void *arg
)
{
  return ata_driver_io_control(dd, cmd, arg, transfer_pio_polled);
}
