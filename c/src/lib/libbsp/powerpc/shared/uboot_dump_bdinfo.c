/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#include <bsp.h>
#include <rtems/bspIo.h>

/*
 *  This file should only be compiled if the BSP has U-Boot.
 *  In addition, this function does not support every PowerPC
 *  CPU model listed in the bd_t structure.  Users are encouraged
 *  to augment this code.  The following #error should be fixed
 *  as more CPU models are supported.
 */
#if defined(HAS_UBOOT)
  #if !defined(CONFIG_MPC5xxx)
    #error "dumpUBootDBInfo: unsupported configuration!!"
  #endif

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
    "Start/Size of DRAM memory  = %p for %lx\n"
    "Start/Size of Flash memory = %p for %lx\n"
    "Reserved area for startup monitor = %ld\n"
    "Start/Size of SRAM memory  = %p for %ld\n"
    "Boot/Reboot flag = %ld\n"
    "IP Address = %d:%d:%d:%d\n"
    "Ethernet address = %02x:%02x:%02x:%02x:%02x:%02x\n"
    "Ethernet speed in Mbps = %d\n"
    "Internal Freq, in MHz = %ld\n"
    "Bus Freq, in MHz = %ld\n"
    "Console Baud Rate = %ld\n"
    #if defined(CONFIG_MPC5xxx)
      "MBAR                       = %p\n"
      "IPB Bus Freq, in MHz       = %ld\n"
      "PCI Bus Freq, in MHz       = %ld\n"
    #endif
    ,
    u->bi_memstart,   u->bi_memsize,
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
    u->bi_busfreq,
    u->bi_baudrate
    #if defined(CONFIG_MPC5xxx)
      ,
      u->bi_mbar_base,
      u->bi_ipbfreq,
      u->bi_pcifreq
    #endif
  );
 
}
#endif

