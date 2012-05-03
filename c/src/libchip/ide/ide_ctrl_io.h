/*
 * ide_ctrl_io.h
 *
 * LibChip library IDE controller header file - IO operations defined for
 * IDE controllers.
 *
 * Copyright (C) 2002 OKTET Ltd., St.-Petersburg, Russia
 * Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */
#ifndef __IDE_CTRL_IO_H__
#define __IDE_CTRL_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/blkdev.h>

/* Command Block Registers */
#define IDE_REGISTER_DATA               0
#define IDE_REGISTER_ERROR              1
#define IDE_REGISTER_FEATURES           IDE_REGISTER_ERROR
#define IDE_REGISTER_SECTOR_COUNT       2
#define IDE_REGISTER_SECTOR_NUMBER      3
#define IDE_REGISTER_LBA0               IDE_REGISTER_SECTOR_NUMBER
#define IDE_REGISTER_CYLINDER_LOW       4
#define IDE_REGISTER_LBA1               IDE_REGISTER_CYLINDER_LOW
#define IDE_REGISTER_CYLINDER_HIGH      5
#define IDE_REGISTER_LBA2               IDE_REGISTER_CYLINDER_HIGH
#define IDE_REGISTER_DEVICE_HEAD        6
#define IDE_REGISTER_LBA3               IDE_REGISTER_DEVICE_HEAD
#define IDE_REGISTER_STATUS             7
#define IDE_REGISTER_COMMAND            IDE_REGISTER_STATUS

/*  Control Block Registers */
#define IDE_REGISTER_ALTERNATE_STATUS   6
#define IDE_REGISTER_DEVICE_CONTROL     IDE_REGISTER_ALTERNATE_STATUS

/* offsets used to access  registers */
#define IDE_REGISTER_DEVICE_CONTROL_OFFSET   8
#define IDE_REGISTER_ALTERNATE_STATUS_OFFSET IDE_REGISTER_DEVICE_CONTROL_OFFSET
#define IDE_REGISTER_DATA_BYTE               9
#define IDE_REGISTER_DATA_WORD               10

/*
 * Registers bits
 */
#define IDE_REGISTER_STATUS_BSY          0x80  /* Busy bit */
#define IDE_REGISTER_STATUS_DRDY         0x40  /* Device ready */
#define IDE_REGISTER_STATUS_DF           0x20  /* Device fault */
#define IDE_REGISTER_STATUS_DSC          0x10  /* Device seek complete-- */
                                               /* obsolete */
#define IDE_REGISTER_STATUS_DRQ          0x08  /* Data request */
#define IDE_REGISTER_STATUS_CORR         0x04  /* Corrected data-- */
                                               /* vendor specific--obsolete */
#define IDE_REGISTER_STATUS_IDX          0x02  /* Index-- */
                                                /* vendor specific--obsolete */
#define IDE_REGISTER_STATUS_ERR          0x01  /* Error */

#define IDE_REGISTER_DEVICE_CONTROL_SRST 0x04  /* Host software reset bit */
#define IDE_REGISTER_DEVICE_CONTROL_nIEN 0x02  /* Negated interrupt enable */

#define IDE_REGISTER_DEVICE_HEAD_L       0x40  /* LBA mode bit */
#define IDE_REGISTER_DEVICE_HEAD_DEV     0x10  /* Device0/Device1 bit */
#define IDE_REGISTER_DEVICE_HEAD_DEV_POS 4     /* Dev0/Dev1 bit position */
#define IDE_REGISTER_DEVICE_HEAD_HS      0x0f  /* Head/LBA24_27 bits */
#define IDE_REGISTER_LBA3_L              0x40
#define IDE_REGISTER_LBA3_DEV            0x10
#define IDE_REGISTER_LBA3_LBA            0x0f

#define IDE_REGISTER_ERROR_ICRC          (1 << 7) /* Interface CRC error on */
                                                  /* UDMA data transfer */
#define IDE_REGISTER_ERROR_UNC           (1 << 6) /* Uncorrectable data error */
#if CCJ_COULD_NOT_FIND_THIS_ERROR
#define IDE_REGISTER_ERROR_WP            (1 << 6) /* Write protect */
#endif
#define IDE_REGISTER_ERROR_MC            (1 << 5) /* Media changed */
#define IDE_REGISTER_ERROR_IDNF          (1 << 4) /* Sector ID not found */
#define IDE_REGISTER_ERROR_MCR           (1 << 3) /* Media change requested */
                                                  /* obsolette */
#define IDE_REGISTER_ERROR_ABRT          (1 << 2) /* Aborted command */
#define IDE_REGISTER_ERROR_NM            (1 << 1) /* No media, End of Media. */
#define IDE_REGISTER_ERROR_AMNF          (1 << 0) /* Address mark not found */
                                                  /* --obsolette in ATA-4 */
#define IDE_REGISTER_ERROR_MED           (1 << 0) /* Media error is detected */

/*
 * ide_controller_read_data_block --
 *     Read data block via controller's data register
 *
 * PARAMETERS:
 *     minor      - minor number of controller
 *     block_size - number of bytes to read
 *     bufs       - set of buffers to store data
 *     cbuf       - number of current buffer from the set
 *     pos        - position inside current buffer 'cbuf'
 *
 * RETURNS:
 *     NONE
 */
void
ide_controller_read_data_block(rtems_device_minor_number  minor,
                               uint32_t                   block_size,
                               rtems_blkdev_sg_buffer    *bufs,
                               uint32_t                  *cbuf,
                               uint32_t                  *pos);

/*
 * ide_controller_write_data_block --
 *     Write data block via controller's data register
 *
 * PARAMETERS:
 *     minor      - minor number of controller
 *     block_size - number of bytes to write
 *     bufs       - set of buffers which store data
 *     cbuf       - number of current buffer from the set
 *     pos        - position inside current buffer 'cbuf'
 *
 * RETURNS:
 *     NONE
 */
void
ide_controller_write_data_block(rtems_device_minor_number  minor,
                                uint32_t                   block_size,
                                rtems_blkdev_sg_buffer    *bufs,
                                uint32_t                  *cbuf,
                                uint32_t                  *pos);

/*
 * ide_controller_read_register --
 *     Read controller's register
 *
 * PARAMETERS:
 *     minor - minor number of controller
 *     reg   - register to read
 *     value - placeholder for result
 *
 * RETURNS
 *     NONE
 */
void
ide_controller_read_register(rtems_device_minor_number  minor,
                             int                        reg,
                             uint16_t                  *value);

/*
 * ide_controller_write_register --
 *     Write controller's register
 *
 * PARAMETERS:
 *     minor - minor number of controller
 *     reg   - register to write
 *     value - value to write
 *
 * RETURNS:
 *     NONE
 */
void
ide_controller_write_register(rtems_device_minor_number minor,
                              int reg, uint16_t   value);

/*
 * ide_controller_config_io_speed --
 *     Set controller's speed of IO operations
 *
 * PARAMETERS:
 *     minor           - minor number of controller
 *     modes_available - speeds available
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL on success, or error code if
 *     error occured
 */
rtems_status_code
ide_controller_config_io_speed(int minor, uint16_t modes_available);

#ifdef __cplusplus
}
#endif


#endif /* __IDE_CTRL_IO_H__ */
