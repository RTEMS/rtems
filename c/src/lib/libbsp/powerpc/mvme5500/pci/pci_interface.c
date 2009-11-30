/* pci_interface.c
 *
 * Copyright 2004, 2006, 2007 All rights reserved. (NDA items)
 *      Brookhaven National Laboratory and Shuchen Kate Feng <feng1@bnl.gov>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 *
 * 8/17/2006 : S. Kate Feng
 *             uses in_le32()/out_le32(), instead of inl()/outl() for compatibility.
 *
 * 11/2008 : Enable "PCI Read Agressive Prefetch",
 *           "PCI Read Line Agressive Prefetch", and
 *           "PCI Read Multiple Agressive Prefetch" to improve the
 *           performance of the PCI based applications (e.g. 1GHz NIC).
 */

#include <libcpu/io.h>
#include <rtems/bspIo.h>	    /* printk */

#include <bsp.h>
#include <bsp/pci.h>
#include <bsp/gtreg.h>
#include <bsp/gtpcireg.h>

#define PCI_DEBUG     0

#if 0
#define CPU2PCI_ORDER
#define PCI2CPU_ORDER
#endif

/* PCI Read Agressive Prefetch Enable (1<<16 ),
 * PCI Read Line Agressive Prefetch Enable( 1<<17),
 * PCI Read Multiple Agressive Prefetch Enable (1<<18).
 */
#ifdef PCI2CPU_ORDER
#define PCI_ACCCTLBASEL_VALUE          0x01079000
#else
#define PCI_ACCCTLBASEL_VALUE          0x01071000
#endif


#define ConfSBDis     0x10000000  /* 1: disable, 0: enable */
#define IOSBDis       0x20000000  /* 1: disable, 0: enable */
#define ConfIOSBDis   0x30000000
#define CpuPipeline   0x00002000  /* optional, 1:enable, 0:disable */

/* CPU to PCI ordering register */
#define DLOCK_ORDER_REG    0x2D0  /* Deadlock and Ordering register */
#define PCI0OrEn      0x00000001
#define PCI1OrEn      0x00000020
#define PCIOrEn       0x40000000
#define PCIOrEnMASK   0x40000021

#define CNT_SYNC_REG       0x2E0  /* Counters and Sync Barrier register */
#define L0SyncBar     0x00001000
#define L1SyncBar     0x00002000
#define DSyncBar      0x00004000
#define SyncBarMode   0x00008000
#define SyncBarMASK   0x0000f000

#define WRTBK_PRIO_BUFFER  0x2d8  /* writback priority and buffer depth */

#define ADDR_PIPELINE 0x00020000

void  pciAccessInit(void);

void pci_interface(void)
{

#ifdef CPU2PCI_ORDER
    /* MOTLOad deafult : 0x07ff8600 */
    out_le32((volatile unsigned int *)(GT64x60_REG_BASE+CNT_SYNC_REG), 0x07fff600);
#endif
    /* asserts SERR upon various detection */
    out_le32((volatile unsigned int *)(GT64x60_REG_BASE+0xc28), 0x3fffff);
    pciAccessInit();
}

void pciAccessInit(void)
{
  unsigned int PciLocal, data;

  for (PciLocal=0; PciLocal < 2; PciLocal++) {
    data = in_le32((volatile unsigned int *)(GT64x60_REG_BASE+PCI0_ACCESS_CNTL_BASE0_LOW+(PciLocal * 0x80)));
#if 0
    printk("PCI%d_ACCESS_CNTL_BASE0_LOW was 0x%x\n",PciLocal,data);
#endif
    data |= PCI_ACCCTLBASEL_VALUE;
    data &= ~0x300000;
    out_le32((volatile unsigned int *)(GT64x60_REG_BASE+PCI0_ACCESS_CNTL_BASE0_LOW+(PciLocal * 0x80)), data);
#if 0
      printf("PCI%d_ACCESS_CNTL_BASE0_LOW now 0x%x\n",PciLocal,in_le32((volatile unsigned int *)(GT64x60_REG_BASE+PCI0_ACCESS_CNTL_BASE0_LOW+(PciLocal * 0x80))));
#endif
  }
}

