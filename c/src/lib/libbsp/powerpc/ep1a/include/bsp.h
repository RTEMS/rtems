/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <libcpu/io.h>
#include <rtems/clockdrv.h>
#include <bsp/vectors.h>

/* fundamental addresses for BSP (CHRPxxx and PREPxxx are from libcpu/io.h) */
#define	_IO_BASE	        CHRP_ISA_IO_BASE
#define _ISA_MEM_BASE           CHRP_ISA_MEM_BASE
/* address of our ram on the PCI bus   */
#define PCI_DRAM_OFFSET         CHRP_PCI_DRAM_OFFSET
#define PCI_MEM_BASE            0x80000000
#define PCI_MEM_BASE_ADJUSTMENT 0
/* address of our ram on the PCI bus   */
#define	PCI_DRAM_OFFSET		CHRP_PCI_DRAM_OFFSET

/* offset of pci memory as seen from the CPU */
#undef  PCI_MEM_BASE
#define PCI_MEM_BASE		0x00000000

/* Override the default values for the following     DEFAULT */
#define PCI_CONFIG_ADDR                 0xfec00000  /* 0xcf8 */
#define PCI_CONFIG_DATA                 0xfee00000  /* 0xcfc */

/*
 * EP1A configuration Registers.
 * Note:  All addresses assume flash boot.
 */

#define EQUIPMENT_PRESENT_REGISTER1     ((volatile unsigned char *)0xffa00000)
#define EQUIPMENT_PRESENT_REGISTER2     ((volatile unsigned char *)0xffa00008)
#define BOARD_REVISION_REGISTER1        ((volatile unsigned char *)0xffa00010)
#define BOARD_REVISION_REGISTER2        ((volatile unsigned char *)0xffa00018)
#define GENERAL_REGISTER1               ((volatile unsigned char *)0xffa00020)
#define GENERAL_REGISTER2               ((volatile unsigned char *)0xffa00028)
#define WATCHDOG_TRIGGER                ((volatile unsigned char *)0xffa00030)

/* EQUIPMENT_PRESENT_REGISTER1 */
#define BANK_MEMORY_SIZE_128MB          0x20
#define BANK_MEMORY_SIZE_64MB           0x10
#define ECC_ENABLED                     0x04

/* EQUIPMENT-PRESENT_REGISTER2 */
#define PLL_CFG_MASK                     0xf8
#define MHZ_33_66_200                    0x70   /* PCI MEM CPU Frequency */
#define MHZ_33_100_200                   0x80   /* PCI MEM CPU Frequency */
#define MHZ_33_66_266                    0xb0   /* PCI MEM CPU Frequency */
#define MHZ_33_66_333                    0x50   /* PCI MEM CPU Frequency */
#define MHZ_33_100_333                   0x08   /* PCI MEM CPU Frequency */
#define MHZ_33_100_350                   0x78   /* PCI MEM CPU Frequency */

#define PMC_SLOT1_PRESENT                0x02
#define PMC_SLOT2_PRESENT                0x01

/* BOARD_REVISION_REGISTER1 */
#define ARTWORK_REVISION_MASK            0xf0
#define BUILD_REVISION_MASK              0x0f

/* BOARD_REVISION_REGISTER2 */
#define HARDWARE_ID_MASK                 0xe0
#define HARDWARE_ID_PPC5_EP1A            0xe0
#define HARDWARE_ID_EP1B                 0xc0

/* GENERAL_REGISTER1 */
#define DISABLE_WATCHDOG                  0x80
#define DISABLE_RESET_SWITCH              0x40
#define DISABLE_USER_FLASH                0x20
#define DISABLE_BOOT_FLASH                0x10
#define LED4_OFF                          0x08
#define LED3_OFF                          0x04
#define LED2_OFF                          0x02
#define LED1_OFF                          0x01


/* GENERAL_REGISTER2 */
#define BSP_FLASH_VPP_ENABLE              0x01
#define BSP_FLASH_PAGE_MASK               0x38
#define BSP_FLASH_PAGE_SHIFT              0x03
#define BSP_BIT_SLOWSTART                 0x04
#define BSP_OFFLINE                       0x02
#define BSP_SYSFAIL                       0x01

/* WATCHDOG_TRIGGER */
#define BSP_FLASH_BASE                   0xff000000
#define BSP_VME_A16_BASE                 0x9fff0000
#define BSP_VME_A24_BASE                 0x9f000000

/*
 *  address definitions for several devices
 *
 */
#define UART_OFFSET_1_8245 (0x04500)
#define UART_OFFSET_2_8245 (0x04600)
#define UART_BASE_COM1     0xff800000
#define UART_BASE_COM2     0xff800040

#include <bsp/openpic.h>

/* Note docs list 0x41000 but OpenPIC has a 0x1000 pad at the start
 * assume that open pic specifies this pad but not mentioned in
 * 8245 docs.
 * This is an offset from EUMBBAR
 */
#define BSP_OPEN_PIC_BASE_OFFSET     0x40000

/* BSP_PIC_DO_EOI is optionally used by the 'vmeUniverse' driver
 * to implement VME IRQ priorities in software.
 * Note that this requires support by the interrupt controller
 * driver (cf. libbsp/shared/powerpc/irq/openpic_i8259_irq.c)
 * and the BSP-specific universe initialization/configuration
 * (cf. libbsp/shared/powerpc/vme/VMEConfig.h vme_universe.c)
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
#define outport_byte(port,value) outb(value,port)
#define outport_word(port,value) outw(value,port)
#define outport_long(port,value) outl(value,port)

#define inport_byte(port,value) (value = inb(port))
#define inport_word(port,value) (value = inw(port))
#define inport_long(port,value) (value = inl(port))

/*
 * EUMMBAR
 */
extern unsigned int EUMBBAR;

/*
 * Total memory
 */
extern unsigned int BSP_mem_size;

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

#define BSP_Convert_decrementer( _value ) \
  ((unsigned long long) ((((unsigned long long)BSP_time_base_divisor) * 1000000ULL) /((unsigned long long) BSP_bus_frequency)) * ((unsigned long long) (_value)))

#define Processor_Synchronize() \
  __asm__ (" eieio ")

extern void BSP_panic(char *s);
extern int BSP_disconnect_clock_handler (void);
extern int BSP_connect_clock_handler (void);

/*
 * FLASH
 */
int BSP_FLASH_Enable_writes( uint32_t area );
int BSP_FLASH_Disable_writes(  uint32_t area );
void BSP_FLASH_set_page( uint8_t  page );

#define BSP_FLASH_ENABLE_WRITES( _area) BSP_FLASH_Enable_writes( _area )
#define BSP_FLASH_DISABLE_WRITES(_area) BSP_FLASH_Disable_writes( _area )
#define BSP_FLASH_SET_PAGE(_page)       BSP_FLASH_set_page( _page )

/* clear hostbridge errors
 *
 * enableMCP: whether to enable MCP checkstop / machine check interrupts
 *            on the hostbridge and in HID0.
 *
 *            NOTE: HID0 and MEREN are left alone if this flag is 0
 *
 * quiet    : be silent
 *
 * RETURNS  : raven MERST register contents (lowermost 16 bits), 0 if
 *            there were no errors
 */
extern unsigned long _BSP_clear_hostbridge_errors(int enableMCP, int quiet);

#endif

#endif
