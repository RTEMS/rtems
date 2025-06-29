/* Driver for the Maxim 1375 i2c RTC (TOD only; very simple...) */

/*
 * Authorship
 * ----------
 * This software was created by
 *
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 *      Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 *      under Contract DE-AC03-76SFO0515 with the Department of Energy.
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

/**
 * Modified to use i2c-rtc driver by J. Lorelli <lorelli@slac.stanford.edu>
 */

#include <unistd.h> /* write, read, close */

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/rtc.h>
#include <rtems/score/sysstate.h>
#include <rtems/rtems/clockimpl.h>
#include <libchip/rtc.h>
#include <libchip/ds1375-rtc.h>

#include <sys/fcntl.h>
#include <stdint.h>

/*
 * Register Definitions and Access Macros
 *
 * The xxx_REG macros are offsets into the register files
 * The xxx_OFF macros are offsets into a in-memory buffer
 *             starting at the seconds (for the 1375 both,
 *             _REG and _OFF happen to be identical).
 */

/* CR Register and bit definitions       */
#define DS1375_CR_REG  0xe
#define DS1375_CR_ECLK    (1<<7)
#define DS1375_CR_CLKSEL1  (1<<6)
#define DS1375_CR_CLKSEL0  (1<<5)
#define DS1375_CR_RS2    (1<<4)
#define DS1375_CR_RS1    (1<<3)
#define DS1375_CR_INTCN    (1<<2)
#define DS1375_CR_A2IE    (1<<1)
#define DS1375_CR_A1IE    (1<<0)

#define DS1375_CSR_REG  0xf

/* Initialize the hardware */
int
rtc_ds1375_hw_init(struct i2c_rtc_base *base)
{
  uint8_t cr;
  int r = i2c_rtc_read(base, DS1375_CR_REG, &cr, 1);
  if (r != 0)
    return r;
  
  /* make sure clock is enabled */
  if (!(cr & DS1375_CR_ECLK)) {
    cr |= DS1375_CR_ECLK;
    r = i2c_rtc_write(base, DS1375_CR_REG, &cr, 1);
    if (r != 0)
      return r;
  }
  return 0;
}
