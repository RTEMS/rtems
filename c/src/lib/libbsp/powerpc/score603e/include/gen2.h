/*  Gen2.h
 *
 *  This include file contains all Generation 2 board addreses
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __SCORE_GENERATION_2_h
#define __SCORE_GENERATION_2_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>

/*
 * ISA/PCI I/O space.
 */
#define SCORE603E_VME_JUMPER_ADDR      0x00e20000
#define BSP_FLASH_BASE                 0x04000000
#define SCORE603E_ISA_PCI_IO_BASE      0x80000000
#define SCORE603E_TIMER_PORT_C         0xfd000000
#define SCORE603E_TIMER_INT_ACK        0xfd000000
#define SCORE603E_TIMER_PORT_B         0xfd000008
#define SCORE603E_TIMER_PORT_A         0xfd000004

#define SCORE603E_BOARD_CTRL_REG       ((volatile uint8_t*)0xfd00002c)
#define SCORE603E_BRD_FLASH_DISABLE_MASK     0x40

#define SCORE603E_85C30_CTRL_0         ((volatile uint8_t*)0xfe200020)
#define SCORE603E_85C30_DATA_0         ((volatile uint8_t*)0xfe200024)
#define SCORE603E_85C30_CTRL_1         ((volatile uint8_t*)0xfe200028)
#define SCORE603E_85C30_DATA_1         ((volatile uint8_t*)0xfe20002c)
#define SCORE603E_85C30_CTRL_2         ((volatile uint8_t*)0xfe200000)
#define SCORE603E_85C30_DATA_2         ((volatile uint8_t*)0xfe200004)
#define SCORE603E_85C30_CTRL_3         ((volatile uint8_t*)0xfe200008)
#define SCORE603E_85C30_DATA_3         ((volatile uint8_t*)0xfe20000c)

/*
 * PSC8 - PMC Card
 */
/* address of our ram on the PCI bus   */
#define PCI_DRAM_OFFSET              PREP_PCI_DRAM_OFFSET
#define BSP_PCI_CONFIGURATION_BASE   0x80800000
#define BSP_PMC_BASE                 BSP_PCI_CONFIGURATION_BASE
#define PCI_MEM_BASE_ADJUSTMENT      0
#define BSP_PCI_PMC_DEVICE_BASE      0x80808000
#define BSP_PCI_REGISTER_BASE        0xfc000000

#define BSP_PCI_DEVICE_ADDRESS( _offset) \
         ((volatile uint32_t *)( BSP_PCI_PMC_DEVICE_BASE + _offset ))


#define BSP_PMC_SERIAL_ADDRESS( _offset )    \
        ((volatile uint8_t*)(BSP_PCI_REGISTER_BASE + _offset))

/*
 * PMC serial channels - (4-7: 232 and 8-11: 422)
 */
#define SCORE603E_85C30_CTRL_4        BSP_PMC_SERIAL_ADDRESS(0x00200020)
#define SCORE603E_85C30_DATA_4        BSP_PMC_SERIAL_ADDRESS(0x00200024)
#define SCORE603E_85C30_CTRL_5        BSP_PMC_SERIAL_ADDRESS(0x00200028)
#define SCORE603E_85C30_DATA_5        BSP_PMC_SERIAL_ADDRESS(0x0020002c)
#define SCORE603E_85C30_CTRL_6        BSP_PMC_SERIAL_ADDRESS(0x00200030)
#define SCORE603E_85C30_DATA_6        BSP_PMC_SERIAL_ADDRESS(0x00200034)
#define SCORE603E_85C30_CTRL_7        BSP_PMC_SERIAL_ADDRESS(0x00200038)
#define SCORE603E_85C30_DATA_7        BSP_PMC_SERIAL_ADDRESS(0x0020003c)
#define SCORE603E_85C30_CTRL_8        BSP_PMC_SERIAL_ADDRESS(0x00200000)
#define SCORE603E_85C30_DATA_8        BSP_PMC_SERIAL_ADDRESS(0x00200004)
#define SCORE603E_85C30_CTRL_9        BSP_PMC_SERIAL_ADDRESS(0x00200008)
#define SCORE603E_85C30_DATA_9        BSP_PMC_SERIAL_ADDRESS(0x0020000c)
#define SCORE603E_85C30_CTRL_10       BSP_PMC_SERIAL_ADDRESS(0x00200010)
#define SCORE603E_85C30_DATA_10       BSP_PMC_SERIAL_ADDRESS(0x00200014)
#define SCORE603E_85C30_CTRL_11       BSP_PMC_SERIAL_ADDRESS(0x00200018)
#define SCORE603E_85C30_DATA_11       BSP_PMC_SERIAL_ADDRESS(0x0020001c)

#define	_IO_BASE		       PREP_ISA_IO_BASE
#define SCORE603E_PCI_IO_CFG_ADDR      0x80000cf8
#define SCORE603E_PCI_IO_CFG_DATA      0x80000cfc

#define SCORE603E_UNIVERSE_BASE        0x80030000
#define SCORE603E_IO_VME_UNIVERSE_BASE 0x80007000
#define PCI_MEM_BASE                   0xc0000000
#define BSP_PCI_MEM_BASE               PCI_MEM_BASE  /* XXX - May want to remove this later */
#define BSP_NVRAM_BASE           0xfd100000
#define BSP_RTC_ADDRESS          ((volatile unsigned char *)0xfd180000)
#define SCORE603E_JP1_JP2_PROM_BASE    0xfff00000
#define SCORE603E_NOT_JP1_2_FLASH_BASE 0xff800000

#if (SCORE603E_USE_SDS) | (SCORE603E_USE_OPEN_FIRMWARE) | (SCORE603E_USE_NONE)
#define SCORE603E_VME_A16_OFFSET       0x04000000
#elif (SCORE603E_USE_DINK)
#define SCORE603E_VME_A16_OFFSET       0x11000000
#define SCORE603E_VME_A24_OFFSET       0x10000000
#define BSP_VME_A24_BASE               (BSP_PCI_MEM_BASE+SCORE603E_VME_A24_OFFSET)
#else
#error "SCORE603E gen2.h -- what ROM monitor are you using"
#endif

#define BSP_VME_A16_BASE         (BSP_PCI_MEM_BASE+SCORE603E_VME_A16_OFFSET)

/*
 *  Definations for the ICM 1770 RTC chip
 */
    /*
     * These values are programed into a register and must not be changed.
     */
#define ICM1770_CRYSTAL_FREQ_32K      0x00
#define ICM1770_CRYSTAL_FREQ_1M       0x01
#define ICM1770_CRYSTAL_FREQ_2M       0x02
#define ICM1770_CRYSTAL_FREQ_4M       0x03

#define BSP_RTC_FREQUENCY           ICM1770_CRYSTAL_FREQ_32K

/*
 *  Z85C30 Definations for the 423 interface.
 */
#define SCORE603E_85C30_0_CLOCK     14745600  /* 10,000,000 ?10->14.5 */
#define SCORE603E_85C30_0_CLOCK_X       16

/*
 *  Z85C30 Definations for the 422 interface.
 */
#define SCORE603E_85C30_1_CLOCK     16000000  /* 10,000,000 ?10->14.5 */
#define SCORE603E_85C30_1_CLOCK_X       16

/*
 *  Z85C30 Definations for the PMC serial chips
 */
#define SCORE603E_85C30_PMC_CLOCK     16000000  /* 10,000,000 ?10->14.5 */
#define SCORE603E_85C30_PMC_CLOCK_X       16

#define SCORE603E_85C30_2_CLOCK       SCORE603E_85C30_PMC_CLOCK
#define SCORE603E_85C30_3_CLOCK       SCORE603E_85C30_PMC_CLOCK
#define SCORE603E_85C30_4_CLOCK       SCORE603E_85C30_PMC_CLOCK
#define SCORE603E_85C30_5_CLOCK       SCORE603E_85C30_PMC_CLOCK
#define SCORE603E_85C30_2_CLOCK_X     SCORE603E_85C30_PMC_CLOCK_X
#define SCORE603E_85C30_3_CLOCK_X     SCORE603E_85C30_PMC_CLOCK_X
#define SCORE603E_85C30_4_CLOCK_X     SCORE603E_85C30_PMC_CLOCK_X
#define SCORE603E_85C30_5_CLOCK_X     SCORE603E_85C30_PMC_CLOCK_X

#define SCORE603E_UNIVERSE_CHIP_ID     0x000010E3

/*
 * FPGA Interupt Address Definations.
 */
#define SCORE603E_FPGA_VECT_DATA    ((volatile uint16_t*)0xfd000040)
#define SCORE603E_FPGA_BIT1_15_0    ((volatile uint16_t*)0xfd000044)
#define SCORE603E_FPGA_MASK_DATA    ((volatile uint16_t*)0xfd000048)
#define SCORE603E_FPGA_IRQ_INPUT    ((volatile uint16_t*)0xfd00004c)

/*
 * The PMC status word is at the PMC base address
 */
#define BSP_PMC_STATUS_ADDRESS  (BSP_PMC_SERIAL_ADDRESS (0))
#define Is_PMC_85C30_4_IRQ( _status ) (_status & 0x80)    /* SCC 422-1 */
#define Is_PMC_85C30_2_IRQ( _status ) (_status & 0x40)    /* SCC 232-1 */
#define Is_PMC_85C30_5_IRQ( _status ) (_status & 0x20)    /* SCC 422-2 */
#define Is_PMC_85C30_3_IRQ( _status ) (_status & 0x08)    /* SCC 232-2 */

#define SCORE603E_PMC_CONTROL_ADDRESS    BSP_PMC_SERIAL_ADDRESS(0x100000)
#define SCORE603E_PMC_SCC_232_LOOPBACK (_word) (_word|0x20)

#define PMC_SET_232_LOOPBACK(_word)   (_word | 0x02)
#define PMC_CLEAR_232_LOOPBACK(_word) (_word & 0xfd)
#define PMC_SET_422_LOOPBACK(_word)   (_word | 0x01)
#define PMC_CLEAR_422_LOOPBACK(_word) (_word & 0xfe)

/*
 *  Score603e Interupt Definations.
 */


/*
 *  BSP_TIMER_AVG_OVERHEAD and BSP_TIMER_LEAST_VALID for the shared timer
 *  driver.
 */

#define BSP_TIMER_AVG_OVERHEAD   4  /* It typically takes xx clicks        */
                                    /*     to start/stop the timer.        */
#define BSP_TIMER_LEAST_VALID    1  /* Don't trust a value lower than this */

/*
 *  Convert decrement value to tenths of microsecnds (used by
 *  shared timer driver).
 *
 *    + CPU has a 66.67 Mhz bus,
 *    + There are 4 bus cycles per click
 *    + We return value in 1/10 microsecond units.
 *   Modified following equation to integer equation to remove
 *   floating point math.
 *   (int) ((float)(_value) / ((66.67 * 0.1) / 4.0))
 */

#define BSP_Convert_decrementer( _value ) \
  (int) (((_value) * 4000) / 6667)

#ifdef __cplusplus
}
#endif

#endif

