/* SPDX-License-Identifier: BSD-2-Clause */

/*
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

#include <bsp.h>

#if defined(HAS_UBOOT)

#include <bsp/u-boot.h>
#include <rtems/bspIo.h>

/*
 *  Dump U-Boot Board Information Structure
 *
 *  u - pointer to information structure
 */
void dumpUBootBDInfo(
  bd_t *u
)
{
  if ( u == (bd_t *)1 ) {
    printk( "UBoot BD Info Ptr NOT Set\n" );
    return;
  }

  printk(
    "*** U-Boot Information ***\n"
    "Start/Size of DRAM memory  = %lu for %llx\n"
    "Start/Size of Flash memory = %lu for %lx\n"
    "Reserved area for startup monitor = %ld\n"
    "Start/Size of SRAM memory  = %lu for %ld\n"
    "Boot/Reboot flag = %ld\n"
    "IP Address = %ld:%ld:%ld:%ld\n"
    "Ethernet address = %02x:%02x:%02x:%02x:%02x:%02x\n"
    "Ethernet speed in Mbps = %d\n"
    "Internal Freq, in MHz = %ld\n"
    "Bus Freq, in MHz = %ld\n"
    #if !defined(U_BOOT_GENERIC_BOARD_INFO)
      "Console Baud Rate = %ld\n"
    #endif
    #if defined(CONFIG_MPC5xxx)
      "MBAR                       = %lx\n"
      "IPB Bus Freq, in MHz       = %ld\n"
      "PCI Bus Freq, in MHz       = %ld\n"
    #endif
    ,
    u->bi_memstart, (unsigned long long) u->bi_memsize,
    u->bi_flashstart, u->bi_flashsize,
    u->bi_flashoffset,
    u->bi_sramstart, u->bi_sramsize,
    u->bi_bootflags,
    ((u->bi_ip_addr >> 24) & 0xff), ((u->bi_ip_addr >> 16) & 0xff),
    ((u->bi_ip_addr >> 8) & 0xff),  (u->bi_ip_addr & 0xff),
    u->bi_enetaddr[0], u->bi_enetaddr[1], u->bi_enetaddr[2],
    u->bi_enetaddr[3], u->bi_enetaddr[4], u->bi_enetaddr[5],
    u->bi_ethspeed,
    u->bi_intfreq,
    u->bi_busfreq
    #if !defined(U_BOOT_GENERIC_BOARD_INFO)
      ,
      u->bi_baudrate
    #endif
    #if defined(CONFIG_MPC5xxx)
      ,
      u->bi_mbar_base,
      u->bi_ipbfreq,
      u->bi_pcifreq
    #endif
  );
}

#endif
