/*
 * ide_controller.c
 * 
 * This is generic rtems driver for IDE controllers.
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Authors: Alexandra Kossovsky <sasha@oktet.ru>
 *          Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 * 
 * $Id$
 *
 */
#include <chain.h>
#include <assert.h>
#include <rtems/blkdev.h>

#include <libchip/ide_ctrl_cfg.h>
#include <libchip/ide_ctrl_io.h>

/*
 * ide_controller_initialize --
 *     Initializes all configured IDE controllers. Controllers configuration 
 *     table is provided by BSP
 *
 * PARAMETERS:
 *     major     - device major number
 *     minor_arg - device minor number
 *     args      - arguments
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL on success, or error code if
 *     error occured
 */
rtems_device_driver 
ide_controller_initialize(rtems_device_major_number  major,
                          rtems_device_minor_number  minor_arg,
                          void                      *args)
{
    unsigned long       minor;
    rtems_status_code   status;

    /* FIXME: may be it should be done on compilation phase */
    if (IDE_Controller_Count > IDE_CTRL_MAX_MINOR_NUMBER)
        rtems_fatal_error_occurred(RTEMS_TOO_MANY);
        
    for (minor=0; minor < IDE_Controller_Count; minor++)
    {
        IDE_Controller_Table[minor].status = IDE_CTRL_NON_INITIALIZED;
        
        if ((IDE_Controller_Table[minor].probe != NULL && 
             IDE_Controller_Table[minor].probe(minor)) ||
            IDE_Controller_Table[minor].fns->ctrl_probe(minor))
        {
            status = rtems_io_register_name(IDE_Controller_Table[minor].name, 
                                            major, minor);
            if (status != RTEMS_SUCCESSFUL) 
                rtems_fatal_error_occurred(status);

            IDE_Controller_Table[minor].fns->ctrl_initialize(minor);
            
            IDE_Controller_Table[minor].status = IDE_CTRL_INITIALIZED;
        }
    }
    return RTEMS_SUCCESSFUL;
}

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
                               unsigned16                 block_size,
                               blkdev_sg_buffer          *bufs,
                               rtems_unsigned32          *cbuf,
                               rtems_unsigned32          *pos)
{
    IDE_Controller_Table[minor].fns->ctrl_read_block(minor, block_size, bufs,
                                                     cbuf, pos);
}

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
                                unsigned16                 block_size,
                                blkdev_sg_buffer          *bufs,
                                rtems_unsigned32          *cbuf,
                                rtems_unsigned32          *pos)

{
    IDE_Controller_Table[minor].fns->ctrl_write_block(minor, block_size, bufs,
                                                      cbuf, pos);
}

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
                             unsigned16                *value)
{
    IDE_Controller_Table[minor].fns->ctrl_reg_read(minor, reg, value);
}

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
ide_controller_write_register(rtems_device_minor_number minor, int reg, 
                              unsigned16 value)
{
    IDE_Controller_Table[minor].fns->ctrl_reg_write(minor, reg, value);
}

/*
 * ide_controller_config_io_speed --
 *     Set controller's speed of IO operations
 *
 * PARAMETERS:
 *     minor         - minor number of controller
 *     modes_avaible - speeds available
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL on success, or error code if
 *     error occured
 */
rtems_status_code 
ide_controller_config_io_speed(int minor, unsigned8 modes_avaible)
{
    return IDE_Controller_Table[minor].fns->ctrl_config_io_speed(
               minor, 
               modes_avaible);
}
