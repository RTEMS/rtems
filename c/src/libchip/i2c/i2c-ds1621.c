/* $Id$ */

/* Trivial i2c driver for the maxim DS1621 temperature sensor;
 * just implements reading constant conversions with 8-bit
 * resolution.
 * Demonstrates the implementation of a i2c high-level driver.
 */

/* Author: Till Straumann, 2005 */
#include <rtems.h>
#include <rtems/libi2c.h>

#include <libchip/i2c-ds1621.h>

#include <rtems/libio.h>


static rtems_status_code
ds1621_init (rtems_device_major_number major, rtems_device_minor_number minor,
             void *arg)
{
  int sc;
  char csr[2] = { DS1621_CMD_CSR_ACCESS, 0 }, cmd;

  /* First start command acquires a lock for the bus */

  /* Initialize; switch continuous conversion on */
  sc = rtems_libi2c_start_write_bytes (minor, csr, 1);
  if (sc < 0)
    return -sc;

  sc = rtems_libi2c_start_read_bytes (minor, csr + 1, 1);
  if (sc < 0)
    return -sc;

  csr[1] &= ~DS1621_CSR_1SHOT;

  sc = rtems_libi2c_start_write_bytes (minor, csr, 2);
  if (sc < 0)
    return -sc;

  /* Start conversion */
  cmd = DS1621_CMD_START_CONV;

  sc = rtems_libi2c_start_write_bytes (minor, &cmd, 1);
  if (sc < 0)
    return -sc;

  /* sending 'stop' relinquishes the bus mutex -- don't hold it
   * across system calls!
   */
  return rtems_libi2c_send_stop (minor);
}

static rtems_status_code
ds1621_read (rtems_device_major_number major, rtems_device_minor_number minor,
             void *arg)
{
  int sc;
  rtems_libio_rw_args_t *rwargs = arg;
  char cmd = DS1621_CMD_READ_TEMP;

  sc = rtems_libi2c_start_write_bytes (minor, &cmd, 1);
  if (sc < 0)
    return -sc;
  if (sc < 1)
    return RTEMS_IO_ERROR;
  sc = rtems_libi2c_start_read_bytes (minor, rwargs->buffer, 1);
  if (sc < 0) {
    rwargs->bytes_moved = 0;
    return -sc;
  }
  rwargs->bytes_moved = 1;
  return rtems_libi2c_send_stop (minor);
}

static rtems_driver_address_table myops = {
  initialization_entry: ds1621_init,
  read_entry:           ds1621_read,
};

static rtems_libi2c_drv_t my_drv_tbl = {
  ops:                  &myops,
  size:                 sizeof (my_drv_tbl),
};

rtems_libi2c_drv_t *i2c_ds1621_driver_descriptor = &my_drv_tbl;
