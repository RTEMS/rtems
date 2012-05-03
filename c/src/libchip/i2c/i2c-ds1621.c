/* Trivial i2c driver for the maxim DS1621 temperature sensor;
 * just implements reading constant conversions with 8-bit
 * resolution.
 * Demonstrates the implementation of a i2c high-level driver.
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */
#include <rtems.h>
#include <rtems/libi2c.h>

#include <libchip/i2c-ds1621.h>

#include <rtems/libio.h>


static rtems_status_code
ds1621_init (rtems_device_major_number major, rtems_device_minor_number minor,
             void *arg)
{
  int sc;
  unsigned char csr[2] = { DS1621_CMD_CSR_ACCESS, 0 }, cmd;

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
  unsigned char cmd = DS1621_CMD_READ_TEMP;

  sc = rtems_libi2c_start_write_bytes (minor, &cmd, 1);
  if (sc < 0)
    return -sc;
  if (sc < 1)
    return RTEMS_IO_ERROR;
  sc = rtems_libi2c_start_read_bytes(minor, (unsigned char *)rwargs->buffer, 1);
  if (sc < 0) {
    rwargs->bytes_moved = 0;
    return -sc;
  }
  rwargs->bytes_moved = 1;
  return rtems_libi2c_send_stop (minor);
}

static rtems_driver_address_table myops = {
  .initialization_entry = ds1621_init,
  .read_entry =           ds1621_read,
};

static rtems_libi2c_drv_t my_drv_tbl = {
  .ops =                  &myops,
  .size =                 sizeof (my_drv_tbl),
};

rtems_libi2c_drv_t *i2c_ds1621_driver_descriptor = &my_drv_tbl;
