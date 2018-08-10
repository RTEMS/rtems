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

#ifndef GEN5200_ATA_H
#define GEN5200_ATA_H

#include "bestcomm.h"

#include <assert.h>

#include <rtems.h>
#include <rtems/diskdevs.h>
#include <rtems/bdbuf.h>

#include <libchip/ata_internal.h>
#include <libchip/ide_ctrl_io.h>
#include <libchip/ide_ctrl_cfg.h>

#include <libcpu/powerpc-utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DCTRL_SRST BSP_BBIT8(5)
#define DCTRL_NIEN BSP_BBIT8(6)

#define DAST_BSY BSP_BBIT8(0)
#define DAST_DRDY BSP_BBIT8(1)
#define DAST_DRQ BSP_BBIT8(4)
#define DAST_ERR BSP_BBIT8(7)

#define DST_BSY BSP_BBIT16(0)
#define DST_DRDY BSP_BBIT16(1)
#define DST_DRQ BSP_BBIT16(4)
#define DST_ERR BSP_BBIT16(7)

#define DDMA_HUT BSP_BBIT8(1)
#define DDMA_FR BSP_BBIT8(2)
#define DDMA_FE BSP_BBIT8(3)
#define DDMA_IE BSP_BBIT8(4)
#define DDMA_UDMA BSP_BBIT8(5)
#define DDMA_READ BSP_BBIT8(6)
#define DDMA_WRITE BSP_BBIT8(7)

#define ATA_SECTOR_SHIFT 9

#define ATA_PER_TRANSFER_SECTOR_COUNT_MAX 256

typedef union {
  struct {
    uint8_t alternate_status;
    uint8_t reserved_0[3];
    uint16_t data;
    uint8_t reserved_1[2];
    uint8_t error;
    uint8_t reserved_2[3];
    uint8_t sector_count;
    uint8_t reserved_3[3];
    uint8_t sector;
    uint8_t reserved_4[3];
    uint8_t cylinder_low;
    uint8_t reserved_5[3];
    uint8_t cylinder_high;
    uint8_t reserved_6[3];
    uint8_t head;
    uint8_t reserved_7[3];
    uint16_t status;
    uint8_t reserved_8[2];
  } read;

  struct {
    uint8_t control;
    uint8_t reserved_0[3];
    uint16_t data;
    uint8_t reserved_1[2];
    uint8_t feature;
    uint8_t reserved_2[3];
    uint8_t sector_count;
    uint8_t reserved_3[3];
    uint8_t sector;
    uint8_t reserved_4[3];
    uint8_t cylinder_low;
    uint8_t reserved_5[3];
    uint8_t cylinder_high;
    uint8_t reserved_6[3];
    uint8_t head;
    uint8_t reserved_7[3];
    uint8_t command;
    uint8_t dma_control;
    uint8_t reserved_8[2];
  } write;
} ata_drive_registers;

#define ATA ((volatile ata_drive_registers *) 0xf0003a5c)

static inline bool ata_is_data_request(void)
{
  return (ATA->read.alternate_status & DAST_DRQ) != 0;
}

static inline bool ata_is_drive_ready_for_selection(void)
{
  return (ATA->read.alternate_status & (DAST_BSY | DAST_DRQ)) == 0;
}

static inline void ata_wait_400_nano_seconds(void)
{
  ATA->read.alternate_status;
}

static inline void ata_wait_for_drive_ready(void)
{
  while ((ATA->read.alternate_status & (DAST_BSY | DAST_DRQ | DAST_DRDY)) != DAST_DRDY) {
    /* Wait */
  }
}

static inline void ata_wait_for_not_busy(void)
{
  ata_wait_400_nano_seconds();

  while ((ATA->read.alternate_status & DAST_BSY) != 0) {
    /* Wait */
  }
}

static inline bool ata_wait_for_data_request(void)
{
  ata_wait_400_nano_seconds();

  uint8_t alternate_status;
  do {
    alternate_status = ATA->read.alternate_status;
  } while ((alternate_status & DAST_BSY) == DAST_BSY);

  return (alternate_status & (DAST_ERR | DAST_DRQ)) == DAST_DRQ;
}

static inline bool ata_check_status(void)
{
  return (ATA->read.status & (DST_BSY | DST_ERR)) == 0;
}

static inline void ata_clear_interrupts(void)
{
  ATA->read.status;
}

static inline uint8_t ata_read_or_write_sectors_command(bool read)
{
  return read ? 0x20 : 0x30;
}

static inline rtems_blkdev_bnum ata_max_transfer_count(rtems_blkdev_bnum sector_count)
{
  return sector_count > ATA_PER_TRANSFER_SECTOR_COUNT_MAX ?
    ATA_PER_TRANSFER_SECTOR_COUNT_MAX
      : sector_count;
}

static inline void ata_flush_sector(uint16_t *begin)
{
  /* XXX: The dcbi operation does not work properly */
  rtems_cache_flush_multiple_data_lines(begin, ATA_SECTOR_SIZE);
}

void ata_reset_device(void);

bool ata_set_transfer_mode(uint8_t mode);

bool ata_execute_io_command(uint8_t command, uint32_t lba, uint32_t sector_count);

static inline bool ata_execute_io_command_with_sg(uint8_t command, const rtems_blkdev_sg_buffer *sg)
{
  uint32_t lba = sg->block;
  uint32_t sector_count = sg->length / ATA_SECTOR_SIZE;
  return ata_execute_io_command(command, lba, sector_count);
}

typedef struct {
  const rtems_blkdev_sg_buffer *sg;

  size_t sg_count;

  rtems_blkdev_bnum sg_buffer_offset_mask;

  int sg_index_shift;
} ata_sg_context;

static inline void ata_sg_reset(ata_sg_context *self, const rtems_blkdev_sg_buffer *sg, size_t sg_count)
{
  self->sg = sg;
  self->sg_count = sg_count;
  uint32_t sectors_per_buffer = self->sg[0].length >> ATA_SECTOR_SHIFT;
  self->sg_buffer_offset_mask = sectors_per_buffer - 1;
  self->sg_index_shift = __builtin_ffs((int) sectors_per_buffer) - 1;
}

static inline void ata_sg_create_default(ata_sg_context *self)
{
  ata_sg_reset(self, NULL, 0);
}

static inline void ata_sg_create(ata_sg_context *self, const rtems_blkdev_sg_buffer *sg, size_t sg_count)
{
  ata_sg_reset(self, sg, sg_count);
}

static inline rtems_blkdev_bnum ata_sg_get_start_sector(const ata_sg_context *self)
{
  return self->sg[0].block;
}

static inline rtems_blkdev_bnum ata_sg_get_sector_count(const ata_sg_context *self)
{
  return (self->sg_buffer_offset_mask + 1) * self->sg_count;
}

static inline uint16_t *ata_sg_get_sector_data_begin(const ata_sg_context *self, rtems_blkdev_bnum relative_sector)
{
  uint16_t *begin = (uint16_t *)(self->sg[relative_sector >> self->sg_index_shift].buffer);

  return begin + ((relative_sector & self->sg_buffer_offset_mask) << (ATA_SECTOR_SHIFT - 1));
}

static inline uint16_t *ata_sg_get_sector_data_end(const ata_sg_context *self, uint16_t *begin)
{
  return begin + ATA_SECTOR_SIZE / 2;
}

typedef struct {
  rtems_id lock;

  bool card_present;
} ata_driver;

void ata_driver_create(ata_driver *self, const char *device_file_path, rtems_block_device_ioctl io_control);

void ata_driver_destroy(ata_driver *self);

static inline void ata_driver_lock(const ata_driver *self)
{
  rtems_status_code sc = rtems_semaphore_obtain(self->lock, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;
}

static inline void ata_driver_unlock(const ata_driver *self)
{
  rtems_status_code sc = rtems_semaphore_release(self->lock);
  assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;
}

static inline bool ata_driver_is_card_present(const ata_driver *self)
{
  return self->card_present;
}

static inline void ata_driver_io_request(
  ata_driver *self,
  rtems_blkdev_request *request,
  bool (*transfer)(ata_driver *, bool, rtems_blkdev_sg_buffer *, size_t)
)
{
  assert(request->req == RTEMS_BLKDEV_REQ_READ || request->req == RTEMS_BLKDEV_REQ_WRITE);
  bool read = request->req != RTEMS_BLKDEV_REQ_WRITE;
  rtems_blkdev_sg_buffer *sg = &request->bufs[0];
  uint32_t sg_count = request->bufnum;
  ata_driver_lock(self);
  bool ok = (*transfer)(self, read, sg, sg_count);
  ata_driver_unlock(self);
  rtems_status_code sc = ok ? RTEMS_SUCCESSFUL : RTEMS_IO_ERROR;
  rtems_blkdev_request_done(request, sc);
}

static inline int ata_driver_io_control(
  rtems_disk_device *dd,
  uint32_t cmd,
  void *arg,
  bool (*transfer)(ata_driver *, bool, rtems_blkdev_sg_buffer *, size_t)
)
{
  ata_driver *self = (ata_driver *) rtems_disk_get_driver_data(dd);

  switch (cmd) {
    case RTEMS_BLKIO_REQUEST:
      ata_driver_io_request(self, (rtems_blkdev_request *) arg, transfer);
      return 0;
    case RTEMS_BLKIO_CAPABILITIES:
      *(uint32_t *) arg = RTEMS_BLKDEV_CAP_MULTISECTOR_CONT;
      return 0;
    default:
      return rtems_blkdev_ioctl(dd, cmd, arg);
  }
}

int ata_driver_io_control_pio_polled(
  rtems_disk_device *dd,
  uint32_t cmd,
  void *arg
);

typedef struct {
  ata_driver super;

  bestcomm_task task;

  bool read;

  ata_sg_context sg_context;

  rtems_blkdev_bnum transfer_current;

  rtems_blkdev_bnum transfer_end;
} ata_driver_dma_pio_single;

void ata_driver_dma_pio_single_create(
  ata_driver_dma_pio_single *self,
  const char *device_file_path,
  TaskId task_index
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GEN5200_ATA_H */
