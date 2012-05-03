/* Trivial i2c driver for reading "2-byte eeproms".
 * On 'open' the read-pointer is reset to 0, subsequent
 * read operations slurp data from there...
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

#include <libchip/i2c-2b-eeprom.h>
#include <rtems/libio.h>

#define EEPROM_PG_SZ	32
#define ALGN(x)	(((uint32_t)(x) + EEPROM_PG_SZ) & ~(EEPROM_PG_SZ-1))

static rtems_status_code
send_file_ptr (rtems_device_minor_number minor, unsigned pos, int tout)
{
  int sc;
  unsigned char bytes[2];

  bytes[0] = (pos >> 8) & 0xff;
  bytes[1] = (pos) & 0xff;

  /* poll addressing the next page; if 'tout' is <=0 we only try once
   * and return the status. If 'tout' is positive, we try 'tout' times
   * and return RTEMS_TIMEOUT if it didnt work
   */
  while ((sc = rtems_libi2c_start_write_bytes (minor, bytes, 2)) < 0) {
    if (--tout <= 0)
      return tout ? -sc : RTEMS_TIMEOUT;
    rtems_task_wake_after (1);
  }
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code
i2c_2b_eeprom_write (rtems_device_major_number major,
                     rtems_device_minor_number minor, void *arg)
{
  rtems_libio_rw_args_t *rwargs = arg;
  unsigned off = rwargs->offset;
  int cnt = rwargs->count;
  unsigned char *buf = (unsigned char *)rwargs->buffer;
  int sc;
  unsigned end;
  int l;

  if (cnt <= 0)
    return RTEMS_SUCCESSFUL;

  if ((sc = send_file_ptr (minor, off, 0)))
    return sc;

  do {
    /* write up to next page boundary */
    end = ALGN (off);
    l = end - off;
    if (l > cnt)
      l = cnt;

    sc = rtems_libi2c_write_bytes (minor, buf, l);
    if (sc < 0)
      return -sc;

    sc = rtems_libi2c_send_stop (minor);
    if (sc)
      return sc;

    rwargs->bytes_moved += l;

    buf += l;
    cnt -= l;
    off += l;

    /* poll addressing the next page */
    if ((sc = send_file_ptr (minor, off, 100)))
      return sc;

  } while (cnt > 0);

  return rtems_libi2c_send_stop (minor);
}

static rtems_status_code
i2c_2b_eeprom_read (rtems_device_major_number major,
                    rtems_device_minor_number minor, void *arg)
{
  int sc;
  rtems_libio_rw_args_t *rwargs = arg;

  if (RTEMS_SUCCESSFUL != (sc = send_file_ptr (minor, rwargs->offset, 0)))
    return -sc;

  sc = rtems_libi2c_start_read_bytes(
    minor,
    (unsigned char *)rwargs->buffer,
    rwargs->count
  );

  if (sc < 0) {
    rwargs->bytes_moved = 0;
    return -sc;
  }
  rwargs->bytes_moved = sc;

  return rtems_libi2c_send_stop (minor);
}

static rtems_driver_address_table myops = {
  .read_entry =  i2c_2b_eeprom_read,
  .write_entry = i2c_2b_eeprom_write,
};

static rtems_libi2c_drv_t my_drv_tbl = {
  .ops =         &myops,
  .size =        sizeof (my_drv_tbl),
};

/* provide a second table for R/O access */
static rtems_driver_address_table my_ro_ops = {
  .read_entry =  i2c_2b_eeprom_read,
};

static rtems_libi2c_drv_t my_ro_drv_tbl = {
  .ops =         &my_ro_ops,
  .size =        sizeof (my_ro_drv_tbl),
};


rtems_libi2c_drv_t *i2c_2b_eeprom_driver_descriptor = &my_drv_tbl;
rtems_libi2c_drv_t *i2c_2b_eeprom_ro_driver_descriptor = &my_ro_drv_tbl;
