/*  Gen1.h
 *
 *  This include file contains all Generation 1 board addreses
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: 
 */

#ifndef __SCORE_GENERATION_1_h
#define __SCORE_GENERATION_1_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>

/*
 * ISA/PCI I/O space.
 */
#define SCORE603E_VME_JUMPER_ADDR      0x00e20000
#define SCORE603E_FLASH_BASE_ADDR      0x01000000
#define SCORE603E_ISA_PCI_IO_BASE      0x80000000
#define SCORE603E_TIMER_PORT_C         0x80000278
#define SCORE603E_TIMER_INT_ACK        0x8000027a
#define SCORE603E_TIMER_PORT_B         0x8000027b
#define SCORE603E_TIMER_PORT_A         0x8000027c
#define SCORE603E_85C30_CTRL_1         ((volatile rtems_unsigned8 *)0x800002f8)
#define SCORE603E_85C30_INT_ACK        ((volatile rtems_unsigned8 *)0x800002fa)
#define SCORE603E_85C30_CTRL_0         ((volatile rtems_unsigned8 *)0x800002fb)
#define SCORE603E_85C30_DATA_1         ((volatile rtems_unsigned8 *)0x800002fc)
#define SCORE603E_85C30_DATA_0         ((volatile rtems_unsigned8 *)0x800002ff)
#define SCORE603E_85C30_CTRL_3         ((volatile rtems_unsigned8 *)0x800003f8)
#define SCORE603E_85C30_CTRL_2         ((volatile rtems_unsigned8 *)0x800003fb)
#define SCORE603E_85C30_DATA_3         ((volatile rtems_unsigned8 *)0x800003fc)
#define SCORE603E_85C30_DATA_2         ((volatile rtems_unsigned8 *)0x800003ff)
#define SCORE603E_PCI_IO_CFG_ADDR      0x80000cf8
#define SCORE603E_PCI_IO_CFG_DATA      0x80000cfc

#define SCORE603E_UNIVERSE_BASE        0x80030000
#define SCORE603E_IO_VME_UNIVERSE_BASE 0x80007000
#define SCORE603E_PCI_MEM_BASE         0xc0000000
#define SCORE603E_NVRAM_BASE           0xc00f0000
#define SCORE603E_RTC_ADDRESS          ((volatile unsigned char *)0xc00f1ff8)
#define SCORE603E_JP1_JP2_PROM_BASE    0xfff00000
#define SCORE603E_NOT_JP1_2_FLASH_BASE 0xff800000

#define SCORE603E_VME_A16_OFFSET       0x04000000
#define SCORE603E_VME_A16_BASE         (SCORE603E_PCI_MEM_BASE+SCORE603E_VME_A16_OFFSET) 

#define SCORE603E_BOARD_CTRL_REG       ((volatile rtems_unsigned32*)0x80000800)
#define SCORE603E_BRD_FLASH_DISABLE_MASK     0x02000000

 /*
 *  Z85C30 Definations for the 232 interface.
 */
#define SCORE603E_85C30_0_CLOCK     10000000         /* 10,000,000 */
#define SCORE603E_85C30_0_CLOCK_X       16  

/*
 *  Z85C30 Definations for the 422 interface.
 */
#define SCORE603E_85C30_1_CLOCK     10000000         /* 10,000,000 */
#define SCORE603E_85C30_1_CLOCK_X       16  


#define SCORE603E_UNIVERSE_CHIP_ID     0x000010E3

/*
 *  Score603e Interupt Definations.
 */

/* 
 * First Score Unique IRQ
 */
#define Score_IRQ_First ( PPC_IRQ_LAST +  1 )

/*
 * 82378ZB IRQ definations.
 */
#define SCORE603E_IRQ00_82378ZB   ( Score_IRQ_First +  0 )  
#define SCORE603E_IRQ01_82378ZB   ( Score_IRQ_First +  1 )  
#define SCORE603E_IRQ02_82378ZB   ( Score_IRQ_First +  2 )
#define SCORE603E_IRQ03_82378ZB   ( Score_IRQ_First +  3 )
#define SCORE603E_IRQ04_82378ZB   ( Score_IRQ_First +  4 )
#define SCORE603E_IRQ05_82378ZB   ( Score_IRQ_First +  5 )
#define SCORE603E_IRQ06_82378ZB   ( Score_IRQ_First +  6 )
#define SCORE603E_IRQ07_82378ZB   ( Score_IRQ_First +  7 )
#define SCORE603E_IRQ08_82378ZB   ( Score_IRQ_First +  8 )
#define SCORE603E_IRQ09_82378ZB   ( Score_IRQ_First +  9 )
#define SCORE603E_IRQ10_82378ZB   ( Score_IRQ_First + 10 )
#define SCORE603E_IRQ11_82378ZB   ( Score_IRQ_First + 11 )
#define SCORE603E_IRQ12_82378ZB   ( Score_IRQ_First + 12 )
#define SCORE603E_IRQ13_82378ZB   ( Score_IRQ_First + 13 )
#define SCORE603E_IRQ14_82378ZB   ( Score_IRQ_First + 14 )
#define SCORE603E_IRQ15_82378ZB   ( Score_IRQ_First + 15 )

#define MAX_BOARD_IRQS             SCORE603E_IRQ15_82378ZB

#define SCORE603E_85C30_1_IRQ          SCORE603E_IRQ03_82378ZB   
#define SCORE603E_85C30_0_IRQ          SCORE603E_IRQ04_82378ZB 
#define SCORE603E_UNIVERSE_IRQ         SCORE603E_IRQ12_82378ZB


#define Write_82378ZB( _offset, _data ) { \
  volatile rtems_unsigned8 *addr;         \
  addr = (volatile rtems_unsigned8 *)(SCORE603E_ISA_PCI_IO_BASE + _offset);\
  *addr = _data;                         }

#define Read_82378ZB( _offset, _data ) { \
  volatile rtems_unsigned8 *addr;         \
  addr = (volatile rtems_unsigned8 *)(SCORE603E_ISA_PCI_IO_BASE + _offset);\
  _data = *addr;                         }


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




