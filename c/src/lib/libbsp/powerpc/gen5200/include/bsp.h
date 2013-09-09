/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains board specific definitions                   |
\*===============================================================*/

#ifndef __GEN5200_BSP_h
#define __GEN5200_BSP_h

#include <bspopts.h>

#include <libcpu/powerpc-utility.h>

/*
 * Some symbols defined in the linker command file.
 */

LINKER_SYMBOL(bsp_ram_start);
LINKER_SYMBOL(bsp_ram_end);
LINKER_SYMBOL(bsp_ram_size);

LINKER_SYMBOL(bsp_rom_start);
LINKER_SYMBOL(bsp_rom_end);
LINKER_SYMBOL(bsp_rom_size);

LINKER_SYMBOL(bsp_dpram_start);
LINKER_SYMBOL(bsp_dpram_end);
LINKER_SYMBOL(bsp_dpram_size);

LINKER_SYMBOL(bsp_section_text_start);
LINKER_SYMBOL(bsp_section_text_end);
LINKER_SYMBOL(bsp_section_text_size);

LINKER_SYMBOL(bsp_section_data_start);
LINKER_SYMBOL(bsp_section_data_end);
LINKER_SYMBOL(bsp_section_data_size);

LINKER_SYMBOL(bsp_section_bss_start);
LINKER_SYMBOL(bsp_section_bss_end);
LINKER_SYMBOL(bsp_section_bss_size);

LINKER_SYMBOL(bsp_interrupt_stack_start);
LINKER_SYMBOL(bsp_interrupt_stack_end);
LINKER_SYMBOL(bsp_interrupt_stack_size);

LINKER_SYMBOL(bsp_work_area_start);

LINKER_SYMBOL(MBAR);

/* Provide legacy defines */

#ifdef MPC5200_BOARD_PM520_ZE30
#define PM520_ZE30
#endif

#ifdef MPC5200_BOARD_PM520_CR825
#define PM520_CR825
#endif

#ifdef MPC5200_BOARD_ICECUBE
#define icecube
#endif

#ifdef MPC5200_BOARD_BRS5L
#define BRS5L
#endif

/*
 * distinguish board characteristics
 */
/*
 * for PM520 mdule on a ZE30 carrier
 */
#if defined(MPC5200_BOARD_PM520_ZE30)
#define PM520
#endif
/*
 * for PM520 mdule on a CR825 carrier
 */
#if defined(MPC5200_BOARD_PM520_CR825)
#define PM520
#endif

#if !defined(HAS_UBOOT)
  /* we need the low level initialization in start.S*/
  #define NEED_LOW_LEVEL_INIT
#endif

#if defined(MPC5200_BOARD_BRS5L)
/*
 * IMD Custom Board BRS5L
 */

#define HAS_NVRAM_93CXX

#elif defined(MPC5200_BOARD_BRS6L)
  #define MPC5200_BRS6L_FPGA_BEGIN 0x800000
  #define MPC5200_BRS6L_FPGA_SIZE (64 * 1024)
  #define MPC5200_BRS6L_FPGA_END \
    (MPC5200_BRS6L_FPGA_BEGIN + MPC5200_BRS6L_FPGA_SIZE)

  #define MPC5200_BRS6L_MRAM_BEGIN 0xff000000
  #define MPC5200_BRS6L_MRAM_SIZE (4 * 1024 * 1024)
  #define MPC5200_BRS6L_MRAM_END \
    (MPC5200_BRS6L_MRAM_BEGIN + MPC5200_BRS6L_MRAM_SIZE)
#elif defined (PM520)

/* Nothing special */

#elif defined (MPC5200_BOARD_ICECUBE)
/*
 *  Codename: IceCube
 *  Compatible Boards:
 *     Freescape MPC5200LITE
 *     Embedded Planet EP5200
 */

#elif defined (MPC5200_BOARD_DP2)

/* Nothing special */

#else
#error "board type not defined"
#endif

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif


#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/rtc.h>
#include <i2cdrv.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>
#include <bsp/u-boot.h>
#include <bsp/default-initial-extension.h>

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int rtems_mpc5200_fec_driver_attach_detach (struct rtems_bsdnet_ifconfig *config, int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_mpc5200_fec_driver_attach_detach

/* miscellaneous stuff assumed to exist */

/*
 * We need to decide how much memory will be non-cacheable. This
 * will mainly be memory that will be used in DMA (network and serial
 * buffers).
 */
/*
#define NOCACHE_MEM_SIZE 512*1024
*/

/*
 *  Device Driver Table Entries
 */

#ifdef HAS_NVRAM_93CXX
#define NVRAM_DRIVER_TABLE_ENTRY \
  { nvram_driver_initialize, nvram_driver_open, nvram_driver_close, \
    nvram_driver_read, nvram_driver_write, NULL }
#endif

/*
 * indicate, that BSP has IDE driver
 */
#define RTEMS_BSP_HAS_IDE_DRIVER

/* functions */

/* #define SHOW_MORE_INIT_SETTINGS 1 */

/* ata modes */
/* #undef ATA_USE_INT */
#define ATA_USE_INT

/* clock settings */
#if defined(HAS_UBOOT)
#define IPB_CLOCK (bsp_uboot_board_info.bi_ipbfreq)
#define XLB_CLOCK (bsp_uboot_board_info.bi_busfreq)
#define G2_CLOCK  (bsp_uboot_board_info.bi_intfreq)
#elif defined(MPC5200_BOARD_BRS5L) || defined(MPC5200_BOARD_BRS6L)
#define IPB_CLOCK 66000000   /* 66 MHz */
#define XLB_CLOCK 132000000  /* 132 MHz */
#define G2_CLOCK  396000000  /* 396 MHz */
#else
#define IPB_CLOCK 33000000   /* 33 MHz */
#define XLB_CLOCK 66000000   /* 66 MHz */
#define G2_CLOCK  231000000  /* 231 MHz */
#endif

#if defined(HAS_UBOOT)
#define GEN5200_CONSOLE_BAUD (bsp_uboot_board_info.bi_baudrate)
#else
#define GEN5200_CONSOLE_BAUD 115200
#endif

/*
 *  Convert decrement value to tenths of microsecnds (used by
 *  shared timer driver).
 *
 *    + CPU has a XLB_CLOCK bus,
 *    + There are 4 bus cycles per click
 *    + We return value in 1/10 microsecond units.
 *   Modified following equation to integer equation to remove
 *   floating point math.
 *   (int) ((float)(_value) / ((XLB_CLOCK/1000000 * 0.1) / 4.0))
 */

#define BSP_Convert_decrementer( _value ) \
  (int) (((_value) * 4000) / (XLB_CLOCK/10000))

/* slicetimer settings */
#define USE_SLICETIMER_0     TRUE
#define USE_SLICETIMER_1     FALSE

void *bsp_idle_thread( uintptr_t ignored );
#define BSP_IDLE_TASK_BODY bsp_idle_thread

/* BSP specific IRQ Benchmarking support */
void BSP_IRQ_Benchmarking_Reset(void);
void BSP_IRQ_Benchmarking_Report(void);

#if defined(HAS_UBOOT)
  /* Routine to obtain U-Boot environment variables */
  const char *bsp_uboot_getenv(
    const char *name
  );
#endif

void cpu_init(void);

typedef enum {
  MPC5200_FATAL_PCF8563_INVALID_YEAR,
  MPC5200_FATAL_SLICETIMER_0_IRQ_INSTALL,
  MPC5200_FATAL_SLICETIMER_1_IRQ_INSTALL,
  MPC5200_FATAL_TM27_IRQ_INSTALL,
  MPC5200_FATAL_MSCAN_A_INIT,
  MPC5200_FATAL_MSCAN_B_INIT,
  MPC5200_FATAL_MSCAN_A_SET_MODE,
  MPC5200_FATAL_MSCAN_B_SET_MODE,
  MPC5200_FATAL_ATA_DISK_IO_INIT,
  MPC5200_FATAL_ATA_DISK_CREATE,
  MPC5200_FATAL_ATA_DMA_SINGLE_IRQ_INSTALL,
  MPC5200_FATAL_ATA_LOCK_CREATE,
  MPC5200_FATAL_ATA_LOCK_DESTROY
} mpc5200_fatal_code;

void mpc5200_fatal(mpc5200_fatal_code code) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* GEN5200 */
