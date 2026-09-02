/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME5100
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (C) 2026 UChicago Argonne LLC,
 * as operator of Argonne National Laboratory
 * Author: Vijay Banerjee <vijay@rtems.org>
 *
 * Copyright (C) 2002 Till Straumann <strauman@slac.stanford.edu>
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

#ifndef LIBBSP_POWERPC_MVME5100_BSP_H
#define LIBBSP_POWERPC_MVME5100_BSP_H

/**
 * @defgroup RTEMSBSPsPowerPCMVME5100 MVME5100
 *
 * @ingroup RTEMSBSPsPowerPC
 *
 * @brief MVME5100 Board Support Package.
 *
 * @{
 */

#include <bsp/default-initial-extension.h>
#include <bspopts.h>

#include <bsp/vectors.h>
#include <libcpu/io.h>
#include <rtems.h>

#ifdef qemu
#include <rtems/bspcmdline.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* fundamental addresses for BSP (CHRPxxx and PREPxxx are from libcpu/io.h) */
#define _IO_BASE PREP_ISA_IO_BASE
#define _ISA_MEM_BASE PREP_ISA_MEM_BASE

/* address of our ram on the PCI bus   */
#define PCI_DRAM_OFFSET PREP_PCI_DRAM_OFFSET

/* offset of pci memory as seen from the CPU */
#define PCI_MEM_BASE PREP_ISA_MEM_BASE

#define _REG_STRIDE 4
#define PCI_MEM_WIN0 0x80000000
#define BSP_VME_APERTURE_SIZE 0x20000000

#if defined(_REG_STRIDE)
#define BSP_REG_OFF(val) ((val) << _REG_STRIDE)
#else
#define BSP_REG_OFF(val) (val)
#endif

/*
 * Extra page table space for VME windows
 */
#define BSP_PGTBL_EXTRA_SIZE BSP_VME_APERTURE_SIZE

#define BSP_MVME5100_MEM_SIZE 0x20000000
#define BSP_MVME5100_PROCESSOR_HZ 450000000
#define BSP_MVME5100_BUS_HZ 100000000
#define BSP_MVME5100_TIMEBASE_DIVISOR 4000
#define BSP_VPD_NOT_FOUND 0xdeadbeef /* sentinel before the read */

#define BSP_HAWK_SMC_BASE 0xfef80000 /* system memory controller */
#define HAWK_SMC_SDRAM_SIZE_AD 0x10  /* blocks A-D, one byte each */
#define HAWK_SMC_SDRAM_SIZE_EH 0xc0  /* blocks E-H, one byte each */
#define HAWK_SMC_RAM_EN 0x80         /* per-byte enable bit */
#define HAWK_SMC_RAM_SIZ 0x0f        /* per-byte size code */

#define BSP_VPD_I2C_ADDR (0xA0 >> 1) /* onboard configuration VPD */
#define BSP_SPD_I2C_ADDR (0xA8 >> 1) /* memory bank A/B presence detect */
#define BSP_VPD_I2C_ADDR_BYTES 1     /* 256x8 device, one word address */
#define BSP_I2C_BUS_DESCRIPTOR hawk_i2c_bus_descriptor

#define BSP_I2C_BUS0_NAME "/dev/i2c0"
#define BSP_I2C_VPD_EEPROM_NAME "vpd-eeprom"
#define BSP_I2C_VPD_EEPROM_DEV_NAME                                            \
  (BSP_I2C_BUS0_NAME "." BSP_I2C_VPD_EEPROM_NAME)

int BSP_i2c_initialize(void);

/*
 * The BSP has PCI devices. Enable support in LibBSD.
 */
#define BSP_HAS_PC_PCI

/*
 * Remap the PCI address space for LibBSD
 */
#define RTEMS_BSP_PCI_IO_REGION_BASE 0
#define RTEMS_BSP_PCI_MEM_REGION_BASE PCI_DRAM_OFFSET
#define BSP_LOCAL2PCI_ADDR(a) ((uint32_t)(a))
#define RTEMS_BSP_PCI_DMA_REGION_BASE 0

/*
 * Remap the PCI address space for LibBSD
 */
#define RTEMS_BSP_ADDR_PTR(_type) uint##_type##_t __volatile *
#define RTEMS_BSP_ADDR_CPTR(_type) const RTEMS_BSP_ADDR_PTR(_type)
#define RTEMS_BSP_ADDRESS_READ(_addr, _type)                                   \
  *((RTEMS_BSP_ADDR_CPTR(_type))(((RTEMS_BSP_ADDR_CPTR(8))_addr) +             \
                                 PCI_DRAM_OFFSET))
#define RTEMS_BSP_ADDRESS_WRITE(_addr, _val, _type)                            \
  *((RTEMS_BSP_ADDR_PTR(_type))(((RTEMS_BSP_ADDR_PTR(8))_addr) +               \
                                PCI_DRAM_OFFSET)) = (_val)

#define RTEMS_BSP_READ_1(_addr) RTEMS_BSP_ADDRESS_READ(_addr, 8)
#define RTEMS_BSP_READ_2(_addr) RTEMS_BSP_ADDRESS_READ(_addr, 16)
#define RTEMS_BSP_READ_4(_addr) RTEMS_BSP_ADDRESS_READ(_addr, 32)
#define RTEMS_BSP_READ_8(_addr) RTEMS_BSP_ADDRESS_READ(_addr, 64)

#define RTEMS_BSP_WRITE_1(_addr, _val) RTEMS_BSP_ADDRESS_WRITE(_addr, _val, 8)
#define RTEMS_BSP_WRITE_2(_addr, _val) RTEMS_BSP_ADDRESS_WRITE(_addr, _val, 16)
#define RTEMS_BSP_WRITE_4(_addr, _val) RTEMS_BSP_ADDRESS_WRITE(_addr, _val, 32)
#define RTEMS_BSP_WRITE_8(_addr, _val) RTEMS_BSP_ADDRESS_WRITE(_addr, _val, 64)

/*
 *  Base address definitions for several devices
 *
 *  MVME2100 is very similar but has fewer devices and uses on-CPU EPIC
 *  implementation of OpenPIC controller.  It also cannot be probed to
 *  find out what it is which is VERY different from other Motorola boards.
 */

#define BSP_UART_IOBASE_COM1 (0xfef88000)
#define BSP_UART_IOBASE_COM2 (0xfef88200)

#define BSP_KBD_IOBASE ((_IO_BASE) + 0x60)
#define BSP_VGA_IOBASE ((_IO_BASE) + 0x3c0)

#define BSP_CONSOLE_PORT BSP_UART_COM1
#define BSP_UART_BAUD_BASE 115200

struct rtems_bsdnet_ifconfig;

#include <bsp/openpic.h>

/* BSP_PIC_DO_EOI is optionally used by the 'vmeUniverse' driver
 * to implement VME IRQ priorities in software.
 * Note that this requires support by the interrupt controller
 * driver (cf. bsps/powerpc/shared/irq/openpic_i8259_irq.c)
 * and the BSP-specific universe initialization/configuration
 * (cf. bsps/powerpc/motorola_powerpc/include/bsp/VMEConfig.h
 * bsps/powerpc/shared/vme/vme_universe.c)
 *
 * ********* IMPORTANT NOTE ********
 * When deriving from this file (new BSPs)
 * DO NOT define "BSP_PIC_DO_EOI" if you don't know what
 * you are doing i.e., w/o implementing the required pieces
 * mentioned above.
 * ********* IMPORTANT NOTE ********
 */
#define BSP_PIC_DO_EOI openpic_eoi(0)

#ifndef ASM
#define outport_byte(port, value) outb(value, port)
#define outport_word(port, value) outw(value, port)
#define outport_long(port, value) outl(value, port)

#define inport_byte(port, value) (value = inb(port))
#define inport_word(port, value) (value = inw(port))
#define inport_long(port, value) (value = inl(port))

extern unsigned int BSP_mem_size;

/*
 * Start of the heap
 */
extern unsigned int BSP_heap_start;

/*
 * PCI Bus Frequency
 */
extern unsigned int BSP_bus_frequency;

/*
 * processor clock frequency
 */
extern unsigned int BSP_processor_frequency;

/*
 * Time base divisior (how many tick for 1 second).
 */
extern unsigned int BSP_time_base_divisor;

/*
 * String passed by the bootloader.
 */
extern char *BSP_commandline_string;

#define BSP_Convert_decrementer(_value)                                        \
  ((unsigned long long)((((unsigned long long)BSP_time_base_divisor) *         \
                         1000000ULL) /                                         \
                        ((unsigned long long)BSP_bus_frequency)) *             \
   ((unsigned long long)(_value)))

/* extern int printk(const char *, ...) __attribute__((format(printf, 1, 2)));
 */
extern int BSP_disconnect_clock_handler(void);
extern int BSP_connect_clock_handler(void);

/* clear hostbridge errors
 *
 * NOTE: The routine returns always (-1) if 'enableMCP==1'
 *       [semantics needed by libbspExt] if the MCP input is not wired.
 *       It returns and clears the error bits of the PCI status register.
 *       MCP support is disabled because:
 *         a) the 2100 has no raven chip
 *         b) the raven (2300, 2307, 2700) would raise machine check interrupts
 *            on PCI config space access to empty slots.
 */
extern unsigned long _BSP_clear_hostbridge_errors(int enableMCP, int quiet);

/*
 * Prototypes for methods called only from .S for dependency tracking
 */
char *save_boot_params(void *r3, void *r4, void *r5, char *cmdline_start,
                       char *cmdline_end);
void zero_bss(void);

/*
 * Prototypes for BSP methods which cross file boundaries
 */
void VIA_isa_bridge_interrupts_setup(void);

#endif

#ifdef __cplusplus
};
#endif

/** @} */

#endif
