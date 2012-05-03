/*
 * Copyright (c) 2010 embedded brains GmbH.
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Authors: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include "ata_internal.h"

#include <assert.h>
#include <string.h>

#include <libchip/ide_ctrl_io.h>
#include <libchip/ide_ctrl_cfg.h>

/* ata_process_request_on_init_phase --
 *     Process the ATA request during system initialization. Request
 *     processing is syncronous and doesn't use multiprocessing enviroment.
 *
 * PARAMETERS:
 *     ctrl_minor - controller identifier
 *     areq       - ATA request
 *
 * RETURNS:
 *     NONE
 */
void
ata_process_request_on_init_phase(rtems_device_minor_number  ctrl_minor,
                                  ata_req_t                 *areq)
{
    uint16_t           byte;/* emphasize that only 8 low bits is meaningful */
    uint8_t            i;
#if 0
    uint8_t            dev;
#endif
    uint16_t           val, val1;
    volatile unsigned  retries;

    assert(areq);

#if 0
    dev =  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] &
           IDE_REGISTER_DEVICE_HEAD_DEV;
#endif

    ide_controller_write_register(ctrl_minor, IDE_REGISTER_DEVICE_HEAD,
                                  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD]);

	retries = 0;
    do {
        ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, &byte);
        /* If device (on INIT, i.e. it should be idle) is neither
         * busy nor ready something's fishy, i.e., there is probably
         * no device present.
         * I'd like to do a proper timeout but don't know of a portable
         * timeout routine (w/o using multitasking / rtems_task_wake_after())
         */
        if ( ! (byte & (IDE_REGISTER_STATUS_BSY | IDE_REGISTER_STATUS_DRDY))) {
            retries++;
            if ( 10000 == retries ) {
              /* probably no drive connected */
              areq->breq->status = RTEMS_UNSATISFIED;
              return;
            }
        }
    } while ((byte & IDE_REGISTER_STATUS_BSY) ||
             (!(byte & IDE_REGISTER_STATUS_DRDY)));

    for (i=0; i< ATA_MAX_CMD_REG_OFFSET; i++)
    {
        uint32_t   reg = (1 << i);
        if (areq->regs.to_write & reg)
            ide_controller_write_register(ctrl_minor, i,
                                          areq->regs.regs[i]);
    }

    do {
        ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, &byte);
    } while (byte & IDE_REGISTER_STATUS_BSY);

    ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, &val);
    ide_controller_read_register(ctrl_minor, IDE_REGISTER_ERROR, &val1);

    if (val & IDE_REGISTER_STATUS_ERR)
    {
        areq->breq->status = RTEMS_IO_ERROR;
        return;
    }

    switch(areq->type)
    {
        case ATA_COMMAND_TYPE_PIO_IN:
            if (areq->cnt)
            {
              int ccbuf = areq->cbuf;
              ide_controller_read_data_block(ctrl_minor,
                                             areq->breq->bufs[0].length * areq->cnt,
                                             areq->breq->bufs, &areq->cbuf,
                                             &areq->pos);
              ccbuf = areq->cbuf - ccbuf;
              areq->cnt -= ccbuf;
            }
            if (areq->cnt == 0)
            {
                areq->breq->status = RTEMS_SUCCESSFUL;
            }
            else
            {
                /*
                 * this shouldn't happend on the initialization
                 * phase!
                 */
                rtems_fatal_error_occurred(RTEMS_INTERNAL_ERROR);
            }
            break;

        case ATA_COMMAND_TYPE_NON_DATA:
            areq->breq->status = RTEMS_SUCCESSFUL;
            areq->info = val1;
            break;

        default:
            areq->breq->status = RTEMS_IO_ERROR;
            break;
    }
}

void ata_breq_init(blkdev_request1 *breq, uint16_t *sector_buffer)
{
  memset(breq, 0, sizeof(*breq));

  breq->req.done_arg = breq;
  breq->req.bufnum = 1;
  breq->req.bufs [0].length = ATA_SECTOR_SIZE;
  breq->req.bufs [0].buffer = sector_buffer;
}

rtems_status_code ata_identify_device(
  rtems_device_minor_number ctrl_minor,
  int dev,
  uint16_t *sector_buffer,
  ata_dev_t *device_entry
)
{
  ata_req_t areq;
  blkdev_request1 breq;

  ata_breq_init(&breq, sector_buffer);

  /*
   * Issue DEVICE IDENTIFY ATA command and get device
   * configuration
   */
  memset(&areq, 0, sizeof(ata_req_t));
  areq.type = ATA_COMMAND_TYPE_PIO_IN;
  areq.regs.to_write = ATA_REGISTERS_VALUE(IDE_REGISTER_COMMAND);
  areq.regs.regs [IDE_REGISTER_COMMAND] = ATA_COMMAND_IDENTIFY_DEVICE;
  areq.regs.to_read = ATA_REGISTERS_VALUE(IDE_REGISTER_STATUS);
  areq.breq = (rtems_blkdev_request *)&breq;
  areq.cnt = breq.req.bufnum;
  areq.regs.regs [IDE_REGISTER_DEVICE_HEAD] |=
    dev << IDE_REGISTER_DEVICE_HEAD_DEV_POS;

  /*
   * Process the request. Special processing of requests on
   * initialization phase is needed because at this moment there
   * is no multitasking enviroment
   */
  ata_process_request_on_init_phase(ctrl_minor, &areq);

  /* check status of I/O operation */
  if (breq.req.status != RTEMS_SUCCESSFUL) {
    return RTEMS_IO_ERROR;
  }

  /*
   * Parse returned device configuration and fill in ATA internal
   * device info structure
   */
  device_entry->cylinders =
      CF_LE_W(sector_buffer[ATA_IDENT_WORD_NUM_OF_CURR_LOG_CLNDS]);
  device_entry->heads =
      CF_LE_W(sector_buffer[ATA_IDENT_WORD_NUM_OF_CURR_LOG_HEADS]);
  device_entry->sectors =
      CF_LE_W(sector_buffer[ATA_IDENT_WORD_NUM_OF_CURR_LOG_SECS]);
  device_entry->lba_sectors =
      CF_LE_W(sector_buffer[ATA_IDENT_WORD_NUM_OF_USR_SECS1]);
  device_entry->lba_sectors <<= 16;
  device_entry->lba_sectors += CF_LE_W(sector_buffer[ATA_IDENT_WORD_NUM_OF_USR_SECS0]);
  device_entry->lba_avaible =
      (CF_LE_W(sector_buffer[ATA_IDENT_WORD_CAPABILITIES]) >> 9) & 0x1;

  if ((CF_LE_W(sector_buffer[ATA_IDENT_WORD_FIELD_VALIDITY]) &
       ATA_IDENT_BIT_VALID) == 0) {
    /* no "supported modes" info -> use default */
    device_entry->mode_active = ATA_MODES_PIO3;
  } else {
    device_entry->modes_available =
      ((CF_LE_W(sector_buffer[64]) & 0x1) ? ATA_MODES_PIO3 : 0) |
      ((CF_LE_W(sector_buffer[64]) & 0x2) ? ATA_MODES_PIO4 : 0) |
      ((CF_LE_W(sector_buffer[63]) & 0x1) ? ATA_MODES_DMA0 : 0) |
      ((CF_LE_W(sector_buffer[63]) & 0x2) ?
       ATA_MODES_DMA0 | ATA_MODES_DMA1 : 0) |
      ((CF_LE_W(sector_buffer[63]) & 0x4) ?
       ATA_MODES_DMA0 | ATA_MODES_DMA1 | ATA_MODES_DMA2 : 0);
    if (device_entry->modes_available == 0) {
      return RTEMS_IO_ERROR;
    }
  }

  return RTEMS_SUCCESSFUL;
}
