/* pci_interface.c
 *
 * Copyright 2004, Brookhaven National Laboratory and
 *                 Shuchen Kate Feng <feng1@bnl.gov>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 *
 */
#include <libcpu/io.h>
#include <rtems/bspIo.h>	    /* printk */

#include <bsp.h>
#include <bsp/pci.h>
#include <bsp/gtreg.h>
#include <bsp/gtpcireg.h> 

#define PCI_DEBUG     0

/* Please reference the GT64260B datasheet, for the PCI interface, 
 * Synchronization Barriers and PCI ordering.
 *
 * Some PCI devices require Synchronization Barriers or PCI ordering
 * for synchronization.  For example, the VME-OMS58 motor controller we
 * used at NSLS requires either enhanced CPU Synchronization Barrier
 * or PCI-ordering (only one mechanism allowed. See section 11.1.2).
 * To use the former mechanism(default), one needs to call 
 * CPU0_PciEnhanceSync() or CPU1_PciEnhanceSync() to perform software
 * synchronization between the CPU and PCI activities.
 * 
 * To use the PCI-ordering, one can call pciToCpuSync() to trigger
 * the PCI-to-CPU sync barrier after the out_xx(). In this mode,
 * PCI configuration reads suffer sync barrier latency. Please reference
 * the datasheet to explore other options.
 *
 * Note : If PCI_ORDERING is needed for the PCI0, while disabling the
 * deadlock  for the PCI0, one should keep the CommDLEn bit enabled
 * for the deadlock mechanism so that the 10/100 MB ethernet will
 * function correctly.
 *
 */
#define PCI_ORDERING

/*#define PCI_DEADLOCK*/

/*  So far, I do not see the need to disable the address pipelining.
#define DIS_ADDR_PIPELINE*/

#ifdef PCI_ORDERING
#define PCI_ACCCTLBASEL_VALUE          0x01009000
#else
#define PCI_ACCCTLBASEL_VALUE          0x01001000
#endif

#define ConfSBDis     0x10000000  /* 1: disable, 0: enable */
#define IOSBDis       0x20000000  /* 1: disable, 0: enable */
#define ConfIOSBDis   0x30000000
#define CpuPipeline   0x00002000  /* optional, 1:enable, 0:disable */

#define CPU0_SYNC_TRIGGER   0xD0  /* CPU0 Sync Barrier trigger */
#define CPU0_SYNC_VIRTUAL   0xC0  /* CPU0 Sync Barrier Virtual */

#define CPU1_SYNC_TRIGGER   0xD8  /* CPU1 Sync Barrier trigger */
#define CPU1_SYNC_VIRTUAL   0xC8  /* CPU1 Sync Barrier Virtual */


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

void  pciAccessInit();

void pci_interface()
{
  unsigned int data;

#if  (defined(PCI_ORDERING)||defined(DIS_ADDR_PIPELINE))
  data = inl(0); /* needed : read to flush */
  /* MOTLOad default disables Configuration and I/O Read Sync Barrier 
   * which is needed for enhanced CPU sync. barrier  */
#ifdef PCI_ORDERING
  /* enable Configuration Read Sync Barrier and IO read Sync Barrier*/
  data &= ~ConfIOSBDis;
#endif
#ifdef DIS_ADDR_PIPELINE
  data &= ~ADDR_PIPELINE;

#if PCI_DEBUG
  printk("data %x\n", data);
#endif
#endif
  outl(data, 0);
  /* read polling of the register until the new data is being read */
  while ( inl(0)!=data);
#endif

#ifdef PCI_DEADLOCK
  outl(0x07fff600, CNT_SYNC_REG);
#endif
#ifdef PCI_ORDERING
  outl(0xc0060002, DLOCK_ORDER_REG);
  outl(0x07fff600, CNT_SYNC_REG);
#else
  outl(inl(PCI_CMD_CNTL)|PCI_COMMAND_SB_DIS, PCI_CMD_CNTL);
#endif

  /* asserts SERR upon various detection */
  outl(0x3fffff, 0xc28);

  pciAccessInit();
}
/* Use MOTLoad default for Writeback Priority and Buffer Depth 
 */
void pciAccessInit()
{
  unsigned int PciLocal, data;

  for (PciLocal=0; PciLocal < 2; PciLocal++) {
    /* MOTLoad combines the two banks of SDRAM into
     * one PCI access control because the top = 0x1ff
     */
    data = inl(GT_SCS0_Low_Decode) & 0xfff; 
    data |= PCI_ACCCTLBASEL_VALUE;
    data &= ~0x300000;
    outl(data, PCI0_ACCESS_CNTL_BASE0_LOW+(PciLocal * 0x80));
#if PCI_DEBUG
    printk("PCI%d_ACCESS_CNTL_BASE0_LOW 0x%x\n",PciLocal,inl(PCI_ACCESS_CNTL_BASE0_LOW+(PciLocal * 0x80))); 
#endif
  }
}

/* Sync Barrier Trigger. A write to the CPU_SYNC_TRIGGER register triggers
 * the sync barrier process.  The three bits, define which buffers should
 * be flushed.
 * Bit 0 = PCI0 slave write buffer.
 * Bit 1 = PCI1 slave write buffer.
 * Bit 2 = SDRAM snoop queue.
 */
void CPU0_PciEnhanceSync(unsigned int syncVal)
{
  outl(syncVal,CPU0_SYNC_TRIGGER);
  while (inl(CPU0_SYNC_VIRTUAL));
}

void CPU1_PciEnhanceSync(unsigned int syncVal)
{
  outl(syncVal,CPU1_SYNC_TRIGGER);
  while (inl(CPU1_SYNC_VIRTUAL));
}

/* Currently, if PCI_ordering is used for synchronization, configuration
 * reads is programmed to be the PCI slave "synchronization barrier" 
 * cycles. 
 */
void pciToCpuSync(int pci_num)
{
  unsigned char data;
  unsigned char bus=0;

  if (pci_num) bus += BSP_MAX_PCI_BUS_ON_PCI0;
  pci_read_config_byte(bus,0,0,4, &data);
}
