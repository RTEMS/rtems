/*
 *  Display the PowerPC BAT Registers
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <string.h>

#include <bsp.h>
#include <rtems/bspIo.h>
#include <libcpu/spr.h>

void printBAT( int bat, uint32_t upper, uint32_t lower )
{
  uint32_t lowest_addr;
  uint32_t size;

  printk("BAT%d raw(upper=0x%08x, lower=0x%08x) ", bat, upper, lower );

  lowest_addr = (upper & 0xFFFE0000);
  size = (((upper & 0x00001FFC) >> 2) + 1) * (128 * 1024);
  printk(" range(0x%08x, 0x%08x) %s%s %s%s%s%s %s\n",
    lowest_addr,
    lowest_addr + (size - 1),
    (upper & 0x01) ? "P" : "p",
    (upper & 0x02) ? "S" : "s",
    (lower & 0x08) ? "G" : "g",
    (lower & 0x10) ? "M" : "m",
    (lower & 0x20) ? "I" : "i",
    (lower & 0x40) ? "W" : "w",
    (lower & 0x01) ? "Read Only" :
      ((lower & 0x02) ? "Read/Write" : "No Access")
  );
}

void ShowBATS(void){
  uint32_t lower;
  uint32_t upper;

  __MFSPR(536, upper); __MFSPR(537, lower); printBAT( 0, upper, lower );
  __MFSPR(538, upper); __MFSPR(539, lower); printBAT( 1, upper, lower );
  __MFSPR(540, upper); __MFSPR(541, lower); printBAT( 2, upper, lower );
  __MFSPR(542, upper); __MFSPR(543, lower); printBAT( 3, upper, lower );
}
