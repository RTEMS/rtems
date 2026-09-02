/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME5100
 *
 * @brief MVME5100 BSP Start
 *
 * This routine does the bulk of the system initialization.
 */

/*
 * Copyright (C) 2026 UChicago Argonne LLC,
 * as operator of Argonne National Laboratory
 * Author: Vijay Banerjee <vijay@rtems.org>
 *
 * Copyright (C) 1989-2007 On-Line Applications Research Corporation (OAR).
 *
 * Modified to support the MCP750.
 * Copyright (C) 1998 Eric Valette <eric.valette@free.fr>
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

#include <stdlib.h>
#include <string.h>

#include <bsp.h>
#include <bsp/VME.h>
#include <bsp/VMEConfig.h>
#include <bsp/bootcard.h>
#include <bsp/consoleIo.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/motorola.h>
#include <bsp/openpic.h>
#include <bsp/pci.h>
#include <bsp/ppcbug-nvram-set.h>
#include <bsp/vectors.h>
#include <bsp/vpd.h>
#include <libcpu/bat.h>
#include <libcpu/cpuIdent.h>
#include <libcpu/pte121.h>
#include <libcpu/spr.h>
#include <rtems/bspIo.h>
#include <rtems/counter.h>
#include <rtems/powerpc/powerpc.h>
#include <rtems/sysinit.h>

extern void _return_to_ppcbug(void);
extern unsigned long __rtems_end[];
extern void L1_caches_enables(void);
extern unsigned get_L2CR(void);
extern void set_L2CR(unsigned);
extern Triv121PgTbl BSP_pgtbl_setup(unsigned int *);
extern void BSP_pgtbl_activate(Triv121PgTbl);

#define PPC_MIN_BAT_SIZE (128 * 1024)
static char cc_memory[PPC_MIN_BAT_SIZE] RTEMS_ALIGNED(PPC_MIN_BAT_SIZE);

SPR_RW(SPRG1)

#if defined(DEBUG_BATS)
extern void ShowBATS(void);
#endif

/*
 *  Driver configuration parameters
 */
uint32_t bsp_clicks_per_usec;

/*
 * Copy Additional boot param passed by boot loader
 */
#define MAX_LOADER_ADD_PARM 80
char loaderParam[MAX_LOADER_ADD_PARM];

char *BSP_commandline_string = loaderParam;

unsigned int BSP_mem_size;

/*
 * PCI Bus Frequency
 */
unsigned int BSP_bus_frequency = BSP_VPD_NOT_FOUND;
unsigned int BSP_processor_frequency = BSP_VPD_NOT_FOUND;
unsigned int BSP_time_base_divisor = BSP_MVME5100_TIMEBASE_DIVISOR;

char BSP_productIdent[20] = {0};
char BSP_serialNumber[20] = {0};

static VpdBufRec vpdData[] = {{.key = ProductIdent,
                               .instance = 0,
                               .buf = BSP_productIdent,
                               .buflen = sizeof(BSP_productIdent) - 1},
                              {.key = SerialNumber,
                               .instance = 0,
                               .buf = BSP_serialNumber,
                               .buflen = sizeof(BSP_serialNumber) - 1},
                              {.key = CpuClockHz,
                               .instance = 0,
                               .buf = &BSP_processor_frequency,
                               .buflen = sizeof(BSP_processor_frequency)},
                              {.key = BusClockHz,
                               .instance = 0,
                               .buf = &BSP_bus_frequency,
                               .buflen = sizeof(BSP_bus_frequency)},
                              VPD_END};

/* block size in MB indexed by the 4-bit size code */
static const uint16_t hawk_sdram_block_mb[16] = {
    0, 32, 64, 64, 128, 128, 128, 256, 256, 512, 0, 0, 0, 0, 0, 0};

static unsigned int BSP_get_mem_size(void) {
  unsigned int mb = 0;
  unsigned char off[2];
  unsigned i, b;

  off[0] = HAWK_SMC_SDRAM_SIZE_AD;
  off[1] = HAWK_SMC_SDRAM_SIZE_EH;

  for (i = 0; i < 2; i++) {
    uint32_t reg = in_be32((volatile uint32_t *)(BSP_HAWK_SMC_BASE + off[i]));

    for (b = 0; b < 4; b++) {
      uint32_t blk = (reg >> (8 * b)) & 0xff;

      if (blk & HAWK_SMC_RAM_EN) {
        mb += hawk_sdram_block_mb[blk & HAWK_SMC_RAM_SIZ];
      }
    }
  }
  return mb << 20;
}

/*
 *  Use the shared implementations of the following routines
 */

char *save_boot_params(void *r3, void *r4, void *r5, char *cmdline_start,
                       char *cmdline_end) {
  (void)r4;
  (void)r5;
  (void)cmdline_end;

  (void)r3;
  strncpy(loaderParam, cmdline_start, MAX_LOADER_ADD_PARM);
  loaderParam[MAX_LOADER_ADD_PARM - 1] = '\0';
  return loaderParam;
}

uint32_t _CPU_Counter_frequency(void) {
  return BSP_bus_frequency / (BSP_time_base_divisor / 1000);
}

static void bsp_early(void) {
  unsigned l2cr;
  motorolaBoard myBoard;
  Triv121PgTbl pt = 0;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables so that it can be used
   * later...
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  /*
   * Init MMU block address translation to enable hardware access
   */

  /*
   * Map the DBATs needed for initialization
   */

  /*
   * Must have access to open pic PCI ACK registers provided by the RAVEN
   */

  /*
   * PCI hostbridge memory area
   */
  setdbat(1, 0, 0, 0, 0);
  setdbat(2, 0, 0, 0, 0);
  setdbat(3, 0, 0, 0, 0);
  setdbat(2, 0xf0000000, 0xf0000000, 0x10000000, IO_PAGE);

  /*
   * PC legacy IO space used for inb/outb and all PC compatible hardware
   */
  setdbat(1, _IO_BASE, _IO_BASE, 0x10000000, IO_PAGE);

  /* Setting PCI I/O Base address to default memory map
   * Refer to MVME5100 Programmer's reference guide (Sep 2001)
   *
   * Initializing the PCI registers from here removes
   * dependance on debug monitor for initializing the registers
   * correctly
   */
  out_be32((volatile uint32_t *)0xfeff0040, 0xf000f3ff);
  out_be32((volatile uint32_t *)0xfeff0044, 0x000000d2);
  out_be32((volatile uint32_t *)0xfeff0048, 0xfc00fdff);
  out_be32((volatile uint32_t *)0xfeff004c, 0x000000c2);
  out_be32((volatile uint32_t *)0xfeff0050, 0xa000bfff);
  out_be32((volatile uint32_t *)0xfeff0054, 0x000000c2);
  out_be32((volatile uint32_t *)0xfeff0058, 0x80008080);
  out_be32((volatile uint32_t *)0xfeff005c, 0x800000c0);

  /*
   * enables L1 Cache. Note that the L1_caches_enables() codes checks for
   * relevant CPU type so that the reason why there is no use of myCpu...
   */
  L1_caches_enables();

  select_console(CONSOLE_LOG);

  /*
   * We check that the keyboard is present and immediately
   * select the serial console if not.
   */
#if defined(BSP_KBD_IOBASE)
  {
    int err;
    err = kbdreset();
    if (err) {
      select_console(CONSOLE_SERIAL);
    }
  }
#else
  select_console(CONSOLE_SERIAL);
#endif

  /*
   * Enable L2 Cache. Note that the set_L2CR(L2CR) codes checks for
   * relevant CPU type (mpc750)...
   */
  l2cr = get_L2CR();
#ifdef SHOW_LCR2_REGISTER
  printk("Initial L2CR value = %x\n", l2cr);
#endif
  if ((!(l2cr & 0x80000000)) && ((int)l2cr == -1)) {
    set_L2CR(0xb9A14000);
  }

  ppc_exc_initialize();

  myBoard = getMotorolaBoard();

  printk("-----------------------------------------\n");
  printk("Welcome to %s on %s\n", rtems_get_version_string(),
         motorolaBoardToString(myBoard));
  printk("-----------------------------------------\n");
#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Additional boot options are %s\n", loaderParam);
  printk("-----------------------------------------\n");
#endif

#ifdef TEST_RETURN_TO_PPCBUG
  printk("Hit <Enter> to return to PPCBUG monitor\n");
  printk("When Finished hit GO. It should print <Back from monitor>\n");
  debug_getc();
  _return_to_ppcbug();
  printk("Back from monitor\n");
  _return_to_ppcbug();
#endif /* TEST_RETURN_TO_PPCBUG  */

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Going to start PCI buses scanning and initialization\n");
#endif

  pci_initialize();
  {
    const struct _int_map *bspmap = motorolaIntMap(currentBoard);
    if (bspmap) {
      printk("pci : Configuring interrupt routing for '%s'\n",
             motorolaBoardToString(currentBoard));
      FixupPCI(bspmap, motorolaIntSwizzle(currentBoard));
    } else {
      printk("pci : Interrupt routing not available for this bsp\n");
    }
  }

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Number of PCI buses found is : %d\n", pci_bus_count());
#endif
#ifdef TEST_RAW_EXCEPTION_CODE
  printk("Testing exception handling Part 1\n");

  /*
   * Cause a software exception
   */
  __asm__ __volatile("sc");

  /*
   * Check we can still catch exceptions and return coorectly.
   */
  printk("Testing exception handling Part 2\n");
  __asm__ __volatile("sc");

  /*
   * Somehow doing the above seems to clobber SPRG0 on the mvme2100.  The
   * interrupt disable mask is stored in SPRG0. Is this a problem?
   */
  ppc_interrupt_set_disable_mask(PPC_INTERRUPT_DISABLE_MASK_DEFAULT);

#endif

  /* See above */

  BSP_vpdRetrieveFields(vpdData);

  printk("Board Type: %s (S/N %s)\n",
         BSP_productIdent[0] ? BSP_productIdent : "n/a",
         BSP_serialNumber[0] ? BSP_serialNumber : "n/a");

  if (BSP_VPD_NOT_FOUND == BSP_bus_frequency) {
    BSP_bus_frequency = BSP_MVME5100_BUS_HZ;
    printk("Bus Clock NOT FOUND in VPD; using %u Hz\n", BSP_bus_frequency);
  }

  if (BSP_VPD_NOT_FOUND == BSP_processor_frequency) {
    BSP_processor_frequency = BSP_MVME5100_PROCESSOR_HZ;
    printk("CPU Clock NOT FOUND in VPD; using %u Hz\n",
           BSP_processor_frequency);
  }

  BSP_mem_size = BSP_get_mem_size();
  if (0 == BSP_mem_size) {
    BSP_mem_size = BSP_MVME5100_MEM_SIZE;
    printk("No SDRAM blocks enabled in the Hawk; using 0x%x\n", BSP_mem_size);
  }

  {
    const char *chpt = strstr(BSP_commandline_string, "MEMSZ=");

    if (chpt) {
      char *endp;
      uint32_t sz;

      chpt += 6;
      sz = strtoul(chpt, &endp, 0);
      if (endp != chpt) {
        BSP_mem_size = sz;
      }
    }
  }

  /* clear hostbridge errors but leave MCP disabled -
   * PCI config space scanning code will trip otherwise :-(
   */
  _BSP_clear_hostbridge_errors(0 /* enableMCP */, 0 /*quiet*/);

  if (BSP_mem_size > 0x10000000) {

    /* Support cases of system memory size larger than 256Mb.
     *
     * We use BAT3 in order to obtain access to the top section of the RAM.
     * We also need to do this just before setting up the page table because
     * this is where the page table will be located.
     */
    const unsigned int mem256Count = (BSP_mem_size / 0x10000000);
    const unsigned int BAT3Addr =
        ((BSP_mem_size % 0x10000000) ? (mem256Count * 0x10000000)
                                     : ((mem256Count - 1) * 0x10000000));
    setdbat(3, BAT3Addr, BAT3Addr, 0x10000000, IO_PAGE);
#ifdef SHOW_MORE_INIT_SETTINGS
    printk("Setting up BAT3 for large memory support. (BAT3 --> 0x%x)\n",
           BAT3Addr);
#endif
  }

  /* Allocate and set up the page table mappings
   * This is only available on >604 CPUs.
   *
   * NOTE: This setup routine may modify the available memory
   *       size. It is essential to call it before
   *       calculating the workspace etc.
   */
  pt = BSP_pgtbl_setup(&BSP_mem_size);

  if (!pt || TRIV121_MAP_SUCCESS != triv121PgTblMap(pt, TRIV121_121_VSID,
#ifndef qemu
                                                    0xfeff0000,
#else
                                                    0xbffff000,
#endif
                                                    1, TRIV121_ATTR_IO_PAGE,
                                                    TRIV121_PP_RW_PAGE)) {
    printk("WARNING: unable to setup page tables VME "
           "bridge must share PCI space\n");
  }

  /* Use page tables to map the VME windows instead of DBATS. */
  if (!pt || TRIV121_MAP_SUCCESS !=
                 triv121PgTblMap(pt, TRIV121_121_VSID,
                                 PCI_MEM_BASE + _VME_A32_WIN0_ON_PCI,
                                 BSP_VME_APERTURE_SIZE >> 12,
                                 TRIV121_ATTR_IO_PAGE, TRIV121_PP_RW_PAGE)) {
    printk("WARNING: unable to map VME aperture; "
           "VME will be inaccessible\n");
  }

  /*
   *  initialize the device driver parameters
   */
  bsp_clicks_per_usec = BSP_bus_frequency / (BSP_time_base_divisor * 1000);

  /* Initialize interrupt support */
  bsp_interrupt_initialize();

  /* Activate the page table mappings only after
   * initializing interrupts because the irq_mng_init()
   * routine needs to modify the text
   */
  if (pt) {
#ifdef SHOW_MORE_INIT_SETTINGS
    printk("Page table setup finished; will activate it NOW...\n");
#endif
    BSP_pgtbl_activate(pt);
    /* finally, switch off DBAT3 */
    setdbat(3, 0, 0, 0, 0);
  }

  setdbat(3, (intptr_t)&cc_memory[0], (intptr_t)&cc_memory[0], PPC_MIN_BAT_SIZE,
          IO_PAGE);
  rtems_cache_coherent_add_area(&cc_memory[0], PPC_MIN_BAT_SIZE);

#if defined(DEBUG_BATS)
  ShowBATS();
#endif

  uintptr_t nvram_base;
  bool nvram_indirect;
  nvram_base = 0xfef880C8;
  nvram_indirect = true;

  ppcbug_nvram_set_nvbase(nvram_base, nvram_indirect);
  ppcbug_nvram_set_net_unit(1);
  ppcbug_nvram_set_net_label(1, "fxp0");

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif
}

RTEMS_SYSINIT_ITEM(bsp_early, RTEMS_SYSINIT_BSP_EARLY,
                   RTEMS_SYSINIT_ORDER_MIDDLE);

void bsp_start(void) { /* Initialization was done by bsp_early() */ }

RTEMS_SYSINIT_ITEM(BSP_vme_config, RTEMS_SYSINIT_BSP_PRE_DRIVERS,
                   RTEMS_SYSINIT_ORDER_MIDDLE);

static void mvme5100_i2c_initialize(void) { BSP_i2c_initialize(); }

RTEMS_SYSINIT_ITEM(mvme5100_i2c_initialize, RTEMS_SYSINIT_DEVICE_DRIVERS,
                   RTEMS_SYSINIT_ORDER_MIDDLE);
