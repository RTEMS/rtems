/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Display the PowerPC BAT Registers
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <inttypes.h>
#include <string.h>

#include <bsp.h>
#include <rtems/bspIo.h>
#include <libcpu/spr.h>
#include <libcpu/powerpc-utility.h>

void printBAT( int bat, uint32_t upper, uint32_t lower )
{
  uint32_t lowest_addr;
  uint32_t size;

  printk("BAT%d raw(upper=0x%08" PRIx32 ", lower=0x%08" PRIx32 ") ", bat, upper, lower );

  lowest_addr = (upper & 0xFFFE0000);
  size = (((upper & 0x00001FFC) >> 2) + 1) * (128 * 1024);
  printk(" range(0x%08" PRIx32 ", 0x%08" PRIx32 ") %s%s %s%s%s%s %s\n",
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
