/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME2100
 *
 * @brief MVME2100 BSP Start
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
#include <libcpu/spr.h>
#include <rtems/bspIo.h>
#include <rtems/counter.h>
#include <rtems/powerpc/powerpc.h>
#include <rtems/sysinit.h>

extern void _return_to_ppcbug(void);
extern unsigned long __rtems_end[];
extern void L1_caches_enables(void);

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
/*
 * processor clock frequency
 */
unsigned int BSP_processor_frequency = BSP_VPD_NOT_FOUND;
/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor = BSP_MVME2100_TIMEBASE_DIVISOR;

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

static unsigned int BSP_get_mem_size(void) {
  uint32_t start[2], estart[2], end[2], eend[2];
  uint8_t mben;
  unsigned int size = 0;
  unsigned i, b;

  pci_read_config_dword(0, 0, 0, MPC8240_MSAR1, &start[0]);
  pci_read_config_dword(0, 0, 0, MPC8240_MSAR2, &start[1]);
  pci_read_config_dword(0, 0, 0, MPC8240_MESAR1, &estart[0]);
  pci_read_config_dword(0, 0, 0, MPC8240_MESAR2, &estart[1]);
  pci_read_config_dword(0, 0, 0, MPC8240_MEAR1, &end[0]);
  pci_read_config_dword(0, 0, 0, MPC8240_MEAR2, &end[1]);
  pci_read_config_dword(0, 0, 0, MPC8240_MEEAR1, &eend[0]);
  pci_read_config_dword(0, 0, 0, MPC8240_MEEAR2, &eend[1]);
  pci_read_config_byte(0, 0, 0, MPC8240_MBEN, &mben);

  for (i = 0; i < 2; i++) {
    for (b = 0; b < 4; b++) {
      unsigned s = 8 * b;
      uint32_t lo, hi;

      if (0 == (mben & (1 << (4 * i + b)))) {
        continue;
      }

      lo = ((((estart[i] >> s) & 0x3) << 8) | ((start[i] >> s) & 0xff)) << 20;
      hi = (((((eend[i] >> s) & 0x3) << 8) | ((end[i] >> s) & 0xff)) << 20) |
           0xfffff;

      if (lo == size) {
        size = hi + 1; /* extend only a run that is contiguous from zero */
      }
    }
  }
  return size;
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

unsigned int EUMBBAR;

/*
 * Return the current value of the Embedded Utilities Memory Block Base Address
 * Register (EUMBBAR) as read from the processor configuration register using
 * Processor Address Map B (CHRP).
 */
static unsigned int get_eumbbar(void) {
  out_le32((volatile uint32_t *)0xfec00000, 0x80000078);
  return in_le32((volatile uint32_t *)0xfee00000);
}

uint32_t _CPU_Counter_frequency(void) {
  return BSP_bus_frequency / (BSP_time_base_divisor / 1000);
}

static void bsp_early(void) {
  motorolaBoard myBoard;

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

  setdbat(1, 0, 0, 0, 0);
  setdbat(2, 0, 0, 0, 0);
  setdbat(3, 0, 0, 0, 0);

  /*
   * PCI hostbridge memory area
   */
  setdbat(3, 0xf0000000, 0xf0000000, 0x10000000, IO_PAGE);

  /*
   * PCI devices memory area. Needed to access OpenPIC features
   * provided by the Raven
   *
   * T. Straumann: give more PCI address space
   */
  setdbat(2, PCI_MEM_BASE + PCI_MEM_WIN0, PCI_MEM_BASE + PCI_MEM_WIN0,
          0x10000000, IO_PAGE);

  /* Need 0xfec00000 mapped for this */
  EUMBBAR = get_eumbbar();

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
    BSP_bus_frequency = BSP_MVME2100_BUS_HZ;
    printk("Bus Clock NOT FOUND in VPD; using %u Hz\n", BSP_bus_frequency);
  }

  if (BSP_VPD_NOT_FOUND == BSP_processor_frequency) {
    BSP_processor_frequency = BSP_MVME2100_PROCESSOR_HZ;
    printk("CPU Clock NOT FOUND in VPD; using %u Hz\n",
           BSP_processor_frequency);
  }

  BSP_mem_size = BSP_get_mem_size();
  if (0 == BSP_mem_size) {
    BSP_mem_size = BSP_MVME2100_MEM_SIZE;
    printk("No SDRAM banks enabled in the MPC8240; using 0x%x\n", BSP_mem_size);
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

  /*
   *  initialize the device driver parameters
   */
  bsp_clicks_per_usec = BSP_bus_frequency / (BSP_time_base_divisor * 1000);

  /* Initialize interrupt support */
  bsp_interrupt_initialize();

#if defined(DEBUG_BATS)
  ShowBATS();
#endif

  uintptr_t nvram_base;
  bool nvram_indirect;
  nvram_base = 0xFFE81000;
  nvram_indirect = false;

  ppcbug_nvram_set_nvbase(nvram_base, nvram_indirect);
  ppcbug_nvram_set_net_unit(1);
  ppcbug_nvram_set_net_label(1, "dc0");

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif
}

RTEMS_SYSINIT_ITEM(bsp_early, RTEMS_SYSINIT_BSP_EARLY,
                   RTEMS_SYSINIT_ORDER_MIDDLE);

void bsp_start(void) { /* Initialization was done by bsp_early() */ }

RTEMS_SYSINIT_ITEM(BSP_vme_config, RTEMS_SYSINIT_BSP_PRE_DRIVERS,
                   RTEMS_SYSINIT_ORDER_MIDDLE);

static void mvme2100_i2c_initialize(void) { BSP_i2c_initialize(); }

RTEMS_SYSINIT_ITEM(mvme2100_i2c_initialize, RTEMS_SYSINIT_DEVICE_DRIVERS,
                   RTEMS_SYSINIT_ORDER_MIDDLE);
