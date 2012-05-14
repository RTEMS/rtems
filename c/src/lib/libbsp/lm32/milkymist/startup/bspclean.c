/*  bspclean.c
 *
 *  Milkymist shutdown routine
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include "../include/system_conf.h"

static void reboot(void)
{
  /* Writing to CSR_SYSTEM_ID causes a system reset */
  MM_WRITE(MM_SYSTEM_ID, 1);
}

static void icap_write(int val, unsigned int w)
{
  while (!(MM_READ(MM_ICAP) & ICAP_READY));
  if (!val)
    w |= ICAP_CE|ICAP_WRITE;
  MM_WRITE(MM_ICAP, w);
}
static void reconf(void)
{
  icap_write(0, 0xffff); /* dummy word */
  icap_write(0, 0xffff); /* dummy word */
  icap_write(0, 0xffff); /* dummy word */
  icap_write(0, 0xffff); /* dummy word */
  icap_write(1, 0xaa99); /* sync word part 1 */
  icap_write(1, 0x5566); /* sync word part 2 */
  icap_write(1, 0x30a1); /* write to command register */
  icap_write(1, 0x0000); /* null command */
  icap_write(1, 0x30a1); /* write to command register */
  icap_write(1, 0x000e); /* reboot command */
  icap_write(1, 0x2000); /* NOP */
  icap_write(1, 0x2000); /* NOP */
  icap_write(1, 0x2000); /* NOP */
  icap_write(1, 0x2000); /* NOP */
  icap_write(0, 0x1111); /* NULL */
  icap_write(0, 0xffff); /* dummy word */
}

void bsp_cleanup(uint32_t status)
{
  if (status)
    reconf();
  else
    reboot();
}
