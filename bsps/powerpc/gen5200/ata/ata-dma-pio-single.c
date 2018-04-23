/*
 * Copyright (c) 2011-2013 embedded brains GmbH.  All rights reserved.
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

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>

typedef enum {
  DATA_CURRENT = 0,
  DATA_END,
  DATA_REG
} variables;

typedef enum {
  INC_0_NE = 0,
  INC_2_NE
} increments;

/*
 * for
 *   idx0 = DATA_CURRENT, idx1 = DATA_REG
 *   idx0 != DATA_END
 *   idx0 += 2, idx1 += 0
 * do
 *   *idx0 = *idx1 [INT, 16 bit] OR *idx1 = *idx0 [INT, 16 bit]
 */
static const uint32_t ops[] = {
  LCD(0, VAR(DATA_CURRENT), 0, VAR(DATA_REG), TERM_FIRST, VAR(DATA_END), INC_2_NE, INC_0_NE),
    0, /* Transfer opcode, see transfer() */
};

static bool is_last_transfer(const ata_driver_dma_pio_single *self)
{
  return self->transfer_current + 1 == self->transfer_end;
}

static void start_sector_transfer(ata_driver_dma_pio_single *self)
{
  uint16_t *current = ata_sg_get_sector_data_begin(&self->sg_context, self->transfer_current);
  bestcomm_task_set_variable(&self->task, DATA_CURRENT, (uint32_t) current);
  bestcomm_task_set_variable(&self->task, DATA_END, (uint32_t) ata_sg_get_sector_data_end(&self->sg_context, current));
  bestcomm_task_start(&self->task);

  bool last = is_last_transfer(self);
  ++self->transfer_current;

  if (!last) {
    ata_flush_sector(ata_sg_get_sector_data_begin(&self->sg_context, self->transfer_current));
  }
}

static void dma_pio_single_interrupt_handler(void *arg)
{
  ata_driver_dma_pio_single *self = arg;
  bool ok = ata_check_status();
  bool send_event = false;
  if (ok && self->transfer_current != self->transfer_end) {
    bool enable_dma_interrupt = self->read && is_last_transfer(self);
    if (enable_dma_interrupt) {
      bestcomm_task_irq_clear(&self->task);
      bestcomm_task_irq_enable(&self->task);
    }

    start_sector_transfer(self);
  } else {
    send_event = true;
  }

  if (send_event) {
    bestcomm_task_wakeup_event_task(&self->task);
  }
}

static bool transfer_dma_pio_single(ata_driver *super, bool read, rtems_blkdev_sg_buffer *sg, size_t sg_count)
{
  bool ok = true;
  ata_driver_dma_pio_single *self = (ata_driver_dma_pio_single *) super;

  self->read = read;
  ata_sg_reset(&self->sg_context, sg, sg_count);
  rtems_blkdev_bnum start_sector = ata_sg_get_start_sector(&self->sg_context);
  rtems_blkdev_bnum sector_count = ata_sg_get_sector_count(&self->sg_context);
  rtems_blkdev_bnum relative_sector = 0;

  ata_flush_sector(ata_sg_get_sector_data_begin(&self->sg_context, relative_sector));

  uint8_t command = ata_read_or_write_sectors_command(read);

  uint32_t opcode;
  if (read) {
    opcode = DRD1A(INT, INIT_ALWAYS, DEST_DEREF_IDX(0), SZ_16, SRC_DEREF_IDX(1), SZ_16);
  } else {
    opcode = DRD1A(INT, INIT_ALWAYS, DEST_DEREF_IDX(1), SZ_16, SRC_DEREF_IDX(0), SZ_16);
  }

  bestcomm_task_irq_disable(&self->task);
  bestcomm_task_associate_with_current_task(&self->task);

  size_t transfer_opcode_index = 1; /* See ops */
  bestcomm_task_set_opcode(&self->task, transfer_opcode_index, opcode);

  while (ok && relative_sector < sector_count) {
    rtems_blkdev_bnum remaining_sectors = sector_count - relative_sector;
    rtems_blkdev_bnum transfer_count = ata_max_transfer_count(remaining_sectors);

    self->transfer_current = relative_sector;
    self->transfer_end = relative_sector + transfer_count;

    ok = ata_execute_io_command(command, start_sector + relative_sector, transfer_count);
    if (ok) {
      if (!read) {
        ok = ata_wait_for_data_request();
        assert(ok);

        rtems_interrupt_level level;
        rtems_interrupt_disable(level);
        start_sector_transfer(self);
        rtems_interrupt_enable(level);
      }

      bestcomm_task_wait(&self->task);

      ok = ata_check_status();

      relative_sector += ATA_PER_TRANSFER_SECTOR_COUNT_MAX;
    }
  }

  return ok;
}

static int io_control_dma_pio_single(
  rtems_disk_device *dd,
  uint32_t cmd,
  void *arg
)
{
  return ata_driver_io_control(dd, cmd, arg, transfer_dma_pio_single);
}

void ata_driver_dma_pio_single_create(ata_driver_dma_pio_single *self, const char *device_file_path, TaskId task_index)
{
  ata_driver_create(&self->super, device_file_path, io_control_dma_pio_single);

  self->read = false;

  if (ata_driver_is_card_present(&self->super)) {
    bestcomm_task_create_and_load(&self->task, task_index, ops, sizeof(ops));

    bestcomm_task_set_variable(&self->task, DATA_REG, (uint32_t) &ATA->write.data);

    bestcomm_task_set_increment_and_condition(&self->task, INC_0_NE, 0, COND_NE);
    bestcomm_task_set_increment_and_condition(&self->task, INC_2_NE, 2, COND_NE);

    bestcomm_task_enable_combined_write(&self->task, true);
    bestcomm_task_enable_read_buffer(&self->task, true);
    bestcomm_task_enable_speculative_read(&self->task, true);

    ata_clear_interrupts();

    rtems_status_code sc = rtems_interrupt_handler_install(
      BSP_SIU_IRQ_ATA,
      "ATA",
      RTEMS_INTERRUPT_UNIQUE,
      dma_pio_single_interrupt_handler,
      self
    );
    if (sc != RTEMS_SUCCESSFUL) {
      bsp_fatal(MPC5200_FATAL_ATA_DMA_SINGLE_IRQ_INSTALL);
    }
  }
}
