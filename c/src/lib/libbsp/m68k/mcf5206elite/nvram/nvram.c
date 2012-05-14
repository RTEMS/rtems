/*
 * DS1307-based Non-Volatile memory device driver
 *
 * DS1307 chip is a I2C Real-Time Clock. It contains 56 bytes of
 * non-volatile RAM storage. This driver provide file-like interface to
 * this memory.
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <errno.h>
#include <string.h>
#include <bsp.h>
#include <nvram.h>
#include <i2c.h>
#include <ds1307.h>

/* nvram_driver_initialize --
 *     Non-volatile memory device driver initialization.
 */
rtems_device_driver
nvram_driver_initialize(rtems_device_major_number major,
                        rtems_device_minor_number minor,
                        void *arg)
{
    rtems_status_code sc;
    i2c_message_status status;
    i2c_bus_number bus = DS1307_I2C_BUS_NUMBER;
    i2c_address    addr = DS1307_I2C_ADDRESS;
    int try = 0;
    do {
        status = i2c_wrbyte(bus, addr, 0);
        if (status == I2C_NO_DEVICE)
            break;
        try++;
    } while ((try < 15) && (status != I2C_SUCCESSFUL));

    if (status == I2C_SUCCESSFUL)
    {
        sc = rtems_io_register_name("/dev/nvram", major, 0);
        if (sc != RTEMS_SUCCESSFUL)
        {
            errno = EIO;
            return RTEMS_UNSATISFIED;
        }
        else
            return RTEMS_SUCCESSFUL;
    }
    else
    {
        errno = ENODEV;
        return RTEMS_UNSATISFIED;
    }
}

/* nvram_driver_open --
 *     Non-volatile memory device driver open primitive.
 */
rtems_device_driver
nvram_driver_open(rtems_device_major_number major,
                  rtems_device_minor_number minor,
                  void *arg)
{
    return RTEMS_SUCCESSFUL;
}

/* nvram_driver_close --
 *     Non-volatile memory device driver close primitive.
 */
rtems_device_driver
nvram_driver_close(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void *arg)
{
    return RTEMS_SUCCESSFUL;
}

/* nvram_driver_read --
 *     Non-volatile memory device driver read primitive.
 */
rtems_device_driver
nvram_driver_read(rtems_device_major_number major,
                  rtems_device_minor_number minor,
                  void *arg)
{
    rtems_libio_rw_args_t *args = arg;
    uint32_t   count;
    i2c_bus_number bus = DS1307_I2C_BUS_NUMBER;
    i2c_address    addr = DS1307_I2C_ADDRESS;
    i2c_message_status status;
    if (args->offset >= DS1307_NVRAM_SIZE)
    {
        count = 0;
    }
    else if (args->offset + args->count >= DS1307_NVRAM_SIZE)
    {
        count = DS1307_NVRAM_SIZE - args->offset;
    }
    else
    {
        count = args->count;
    }
    if (count > 0)
    {
        int try = 0;
        do {
            status = i2c_wbrd(bus, addr, DS1307_NVRAM_START + args->offset,
                              args->buffer, count);
            try++;
        } while ((try < 15) && (status != I2C_SUCCESSFUL));
        if (status != I2C_SUCCESSFUL)
        {
            errno = EIO;
            return RTEMS_UNSATISFIED;
        }
    }
    args->bytes_moved = count;
    return RTEMS_SUCCESSFUL;
}

/* nvram_driver_write --
 *     Non-volatile memory device driver write primitive.
 */
rtems_device_driver
nvram_driver_write(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void *arg)
{
    rtems_libio_rw_args_t *args = arg;
    uint32_t   count;
    i2c_bus_number bus = DS1307_I2C_BUS_NUMBER;
    i2c_address    addr = DS1307_I2C_ADDRESS;
    i2c_message_status status;

    if (args->offset >= DS1307_NVRAM_SIZE)
    {
        count = 0;
    }
    else if (args->offset + args->count >= DS1307_NVRAM_SIZE)
    {
        count = DS1307_NVRAM_SIZE - args->offset;
    }
    else
    {
        count = args->count;
    }
    if (count > 0)
    {
        int try = 0;
        do {
            uint8_t         buf[DS1307_NVRAM_SIZE + 1];
            buf[0] = DS1307_NVRAM_START + args->offset;
            memcpy(buf+1, args->buffer, count);
            status = i2c_write(bus, addr, buf, count+1);
            try++;
        } while ((try < 15) && (status != I2C_SUCCESSFUL));
        if (status != I2C_SUCCESSFUL)
        {
            errno = EIO;
            return RTEMS_UNSATISFIED;
        }
    }
    args->bytes_moved = count;
    return RTEMS_SUCCESSFUL;
}
