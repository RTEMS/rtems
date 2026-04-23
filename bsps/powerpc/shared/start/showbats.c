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
#include <rtems/powerpc/registers.h>
#include <libcpu/cpuIdent.h>
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

SPR_RO(DBAT0U);
SPR_RO(DBAT0L);
SPR_RO(DBAT1U);
SPR_RO(DBAT1L);
SPR_RO(DBAT2U);
SPR_RO(DBAT2L);
SPR_RO(DBAT3U);
SPR_RO(DBAT3L);
SPR_RO(DBAT4U);
SPR_RO(DBAT4L);
SPR_RO(DBAT5U);
SPR_RO(DBAT5L);
SPR_RO(DBAT6U);
SPR_RO(DBAT6L);
SPR_RO(DBAT7U);
SPR_RO(DBAT7L);

void ShowBATS(void){
  printBAT( 0, _read_DBAT0U(), _read_DBAT0L() );
  printBAT( 1, _read_DBAT1U(), _read_DBAT1L() );
  printBAT( 2, _read_DBAT2U(), _read_DBAT2L() );
  printBAT( 3, _read_DBAT3U(), _read_DBAT3L() );
  if ( ppc_cpu_has_8_bats() ) {
    printBAT( 4, _read_DBAT4U(), _read_DBAT4L() );
    printBAT( 5, _read_DBAT5U(), _read_DBAT5L() );
    printBAT( 6, _read_DBAT6U(), _read_DBAT6L() );
    printBAT( 7, _read_DBAT7U(), _read_DBAT6L() );
  }
}
