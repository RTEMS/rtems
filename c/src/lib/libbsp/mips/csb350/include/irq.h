/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief interrupt definitions.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_MIPS_AU1X00_IRQ_H
#define LIBBSP_MIPS_AU1X00_IRQ_H

#ifndef ASM
  #include <rtems.h>
  #include <rtems/irq.h>
  #include <rtems/irq-extension.h>
  #include <rtems/score/mips.h>
#endif

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

/*
 *  Interrupt Vector Numbers
 *
 */
/* MIPS_INTERRUPT_BASE should be 32 (0x20) */
#define BSP_INTERRUPT_VECTOR_MIN      0
#define AU1X00_IRQ_SW0                (MIPS_INTERRUPT_BASE + 0)
#define AU1X00_IRQ_SW1                (MIPS_INTERRUPT_BASE + 1)
#define AU1X00_IRQ_IC0_REQ0           (MIPS_INTERRUPT_BASE + 2)
#define AU1X00_IRQ_IC0_REQ1           (MIPS_INTERRUPT_BASE + 3)
#define AU1X00_IRQ_IC1_REQ0           (MIPS_INTERRUPT_BASE + 4)
#define AU1X00_IRQ_IC1_REQ1           (MIPS_INTERRUPT_BASE + 5)
#define AU1X00_IRQ_PERF               (MIPS_INTERRUPT_BASE + 6)
#define AU1X00_IRQ_CNT                (MIPS_INTERRUPT_BASE + 7)

#define AU1X00_IRQ_IC0_BASE           (MIPS_INTERRUPT_BASE + 8)
#define AU1X00_IRQ_UART0              (MIPS_INTERRUPT_BASE + 8)
#define AU1X00_IRQ_INTA               (MIPS_INTERRUPT_BASE + 9)
#define AU1X00_IRQ_INTB               (MIPS_INTERRUPT_BASE + 10)
#define AU1X00_IRQ_UART3              (MIPS_INTERRUPT_BASE + 11)
#define AU1X00_IRQ_INTC               (MIPS_INTERRUPT_BASE + 12)
#define AU1X00_IRQ_INTD               (MIPS_INTERRUPT_BASE + 13)
#define AU1X00_IRQ_DMA0               (MIPS_INTERRUPT_BASE + 14)
#define AU1X00_IRQ_DMA1               (MIPS_INTERRUPT_BASE + 15)
#define AU1X00_IRQ_DMA2               (MIPS_INTERRUPT_BASE + 16)
#define AU1X00_IRQ_DMA3               (MIPS_INTERRUPT_BASE + 17)
#define AU1X00_IRQ_DMA4               (MIPS_INTERRUPT_BASE + 18)
#define AU1X00_IRQ_DMA5               (MIPS_INTERRUPT_BASE + 19)
#define AU1X00_IRQ_DMA6               (MIPS_INTERRUPT_BASE + 20)
#define AU1X00_IRQ_DMA7               (MIPS_INTERRUPT_BASE + 21)
#define AU1X00_IRQ_TOY_TICK           (MIPS_INTERRUPT_BASE + 22)
#define AU1X00_IRQ_TOY_MATCH0         (MIPS_INTERRUPT_BASE + 23)
#define AU1X00_IRQ_TOY_MATCH1         (MIPS_INTERRUPT_BASE + 24)
#define AU1X00_IRQ_TOY_MATCH2         (MIPS_INTERRUPT_BASE + 25)
#define AU1X00_IRQ_RTC_TICK           (MIPS_INTERRUPT_BASE + 26)
#define AU1X00_IRQ_RTC_MATCH0         (MIPS_INTERRUPT_BASE + 27)
#define AU1X00_IRQ_RTC_MATCH1         (MIPS_INTERRUPT_BASE + 28)
#define AU1X00_IRQ_RTC_MATCH2         (MIPS_INTERRUPT_BASE + 29)
#define AU1X00_IRQ_PCI_ERR            (MIPS_INTERRUPT_BASE + 30)
#define AU1X00_IRQ_RSV0               (MIPS_INTERRUPT_BASE + 31)
#define AU1X00_IRQ_USB_DEV            (MIPS_INTERRUPT_BASE + 32)
#define AU1X00_IRQ_USB_SUSPEND        (MIPS_INTERRUPT_BASE + 33)
#define AU1X00_IRQ_USB_HOST           (MIPS_INTERRUPT_BASE + 34)
#define AU1X00_IRQ_AC97_ACSYNC        (MIPS_INTERRUPT_BASE + 35)
#define AU1X00_IRQ_MAC0               (MIPS_INTERRUPT_BASE + 36)
#define AU1X00_IRQ_MAC1               (MIPS_INTERRUPT_BASE + 37)
#define AU1X00_IRQ_RSV1               (MIPS_INTERRUPT_BASE + 38)
#define AU1X00_IRQ_AC97_CMD           (MIPS_INTERRUPT_BASE + 39)

#define AU1X00_IRQ_IC1_BASE           (MIPS_INTERRUPT_BASE + 40)
#define AU1X00_IRQ_GPIO0              (MIPS_INTERRUPT_BASE + 40)
#define AU1X00_IRQ_GPIO1              (MIPS_INTERRUPT_BASE + 41)
#define AU1X00_IRQ_GPIO2              (MIPS_INTERRUPT_BASE + 42)
#define AU1X00_IRQ_GPIO3              (MIPS_INTERRUPT_BASE + 43)
#define AU1X00_IRQ_GPIO4              (MIPS_INTERRUPT_BASE + 44)
#define AU1X00_IRQ_GPIO5              (MIPS_INTERRUPT_BASE + 45)
#define AU1X00_IRQ_GPIO6              (MIPS_INTERRUPT_BASE + 46)
#define AU1X00_IRQ_GPIO7              (MIPS_INTERRUPT_BASE + 47)
#define AU1X00_IRQ_GPIO8              (MIPS_INTERRUPT_BASE + 48)
#define AU1X00_IRQ_GPIO9              (MIPS_INTERRUPT_BASE + 49)
#define AU1X00_IRQ_GPIO10             (MIPS_INTERRUPT_BASE + 50)
#define AU1X00_IRQ_GPIO11             (MIPS_INTERRUPT_BASE + 51)
#define AU1X00_IRQ_GPIO12             (MIPS_INTERRUPT_BASE + 52)
#define AU1X00_IRQ_GPIO13             (MIPS_INTERRUPT_BASE + 53)
#define AU1X00_IRQ_GPIO14             (MIPS_INTERRUPT_BASE + 54)
#define AU1X00_IRQ_GPIO15             (MIPS_INTERRUPT_BASE + 55)
#define AU1X00_IRQ_GPIO200            (MIPS_INTERRUPT_BASE + 56)
#define AU1X00_IRQ_GPIO201            (MIPS_INTERRUPT_BASE + 57)
#define AU1X00_IRQ_GPIO202            (MIPS_INTERRUPT_BASE + 58)
#define AU1X00_IRQ_GPIO203            (MIPS_INTERRUPT_BASE + 59)
#define AU1X00_IRQ_GPIO20             (MIPS_INTERRUPT_BASE + 60)
#define AU1X00_IRQ_GPIO204            (MIPS_INTERRUPT_BASE + 61)
#define AU1X00_IRQ_GPIO205            (MIPS_INTERRUPT_BASE + 62)
#define AU1X00_IRQ_GPIO23             (MIPS_INTERRUPT_BASE + 63)
#define AU1X00_IRQ_GPIO24             (MIPS_INTERRUPT_BASE + 64)
#define AU1X00_IRQ_GPIO25             (MIPS_INTERRUPT_BASE + 65)
#define AU1X00_IRQ_GPIO26             (MIPS_INTERRUPT_BASE + 66)
#define AU1X00_IRQ_GPIO27             (MIPS_INTERRUPT_BASE + 67)
#define AU1X00_IRQ_GPIO28             (MIPS_INTERRUPT_BASE + 68)
#define AU1X00_IRQ_GPIO206            (MIPS_INTERRUPT_BASE + 69)
#define AU1X00_IRQ_GPIO207            (MIPS_INTERRUPT_BASE + 70)
#define AU1X00_IRQ_GPIO208_215        (MIPS_INTERRUPT_BASE + 71)

#define AU1X00_MAXIMUM_VECTORS        (MIPS_INTERRUPT_BASE + 72)

#define BSP_INTERRUPT_VECTOR_MAX      AU1X00_MAXIMUM_VECTORS 

/** @} */

#endif /* LIBBSP_MIPS_AU1X00_IRQ_H */
