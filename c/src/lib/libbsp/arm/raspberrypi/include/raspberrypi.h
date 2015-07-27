/**
 * @file raspberrypi.h
 *
 * @ingroup raspberrypi_reg
 *
 * @brief Register definitions.
 */

/*
 *  Copyright (c) 2014 Andre Marques <andre.lousa.marques at gmail.com>
 *  Copyright (c) 2013 Alan Cudmore.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_RASPBERRYPI_H
#define LIBBSP_ARM_RASPBERRYPI_RASPBERRYPI_H

#include <stdint.h>
#include <bsp/utility.h>

/**
 * @defgroup raspberrypi_reg Register Definitions
 *
 * @ingroup arm_raspberrypi
 *
 * @brief Register Definitions
 *
 * @{
 */

/**
 * @name Register Macros
 *
 * @{
 */

#define BCM2835_REG(x)           (*(volatile uint32_t *)(x))
#define BCM2835_BIT(n)           (1 << (n))

/** @} */

/**
 * @name Peripheral Base Register Address
 *
 * @{
 */

#if (BSP_IS_RPI2 == 1)
   #define RPI_PERIPHERAL_BASE      0x3F000000
#else
   #define RPI_PERIPHERAL_BASE      0x20000000
#endif

#define RPI_PERIPHERAL_SIZE         0x01000000

/**
 * @name Internal ARM Timer Registers
 *
 * @{
 */

#define BCM2835_CLOCK_FREQ       250000000

#define BCM2835_TIMER_BASE       (RPI_PERIPHERAL_BASE + 0xB400)

#define BCM2835_TIMER_LOD        (BCM2835_TIMER_BASE+0x00)
#define BCM2835_TIMER_VAL        (BCM2835_TIMER_BASE+0x04)
#define BCM2835_TIMER_CTL        (BCM2835_TIMER_BASE+0x08)
#define BCM2835_TIMER_CLI        (BCM2835_TIMER_BASE+0x0C)
#define BCM2835_TIMER_RIS        (BCM2835_TIMER_BASE+0x10)
#define BCM2835_TIMER_MIS        (BCM2835_TIMER_BASE+0x14)
#define BCM2835_TIMER_RLD        (BCM2835_TIMER_BASE+0x18)
#define BCM2835_TIMER_DIV        (BCM2835_TIMER_BASE+0x1C)
#define BCM2835_TIMER_CNT        (BCM2835_TIMER_BASE+0x20)

#define BCM2835_TIMER_PRESCALE    0xF9

/** @} */

/**
 * @name GPIO Registers
 *
 * @{
 */

#define BCM2835_GPIO_REGS_BASE   (RPI_PERIPHERAL_BASE + 0x200000)

#define BCM2835_GPIO_GPFSEL1     (BCM2835_GPIO_REGS_BASE+0x04)
#define BCM2835_GPIO_GPSET0      (BCM2835_GPIO_REGS_BASE+0x1C)
#define BCM2835_GPIO_GPCLR0      (BCM2835_GPIO_REGS_BASE+0x28)
#define BCM2835_GPIO_GPLEV0      (BCM2835_GPIO_REGS_BASE+0x34)
#define BCM2835_GPIO_GPEDS0      (BCM2835_GPIO_REGS_BASE+0x40)
#define BCM2835_GPIO_GPREN0      (BCM2835_GPIO_REGS_BASE+0x4C)
#define BCM2835_GPIO_GPFEN0      (BCM2835_GPIO_REGS_BASE+0x58)
#define BCM2835_GPIO_GPHEN0      (BCM2835_GPIO_REGS_BASE+0x64)
#define BCM2835_GPIO_GPLEN0      (BCM2835_GPIO_REGS_BASE+0x70)
#define BCM2835_GPIO_GPAREN0     (BCM2835_GPIO_REGS_BASE+0x7C)
#define BCM2835_GPIO_GPAFEN0     (BCM2835_GPIO_REGS_BASE+0x88)
#define BCM2835_GPIO_GPPUD       (BCM2835_GPIO_REGS_BASE+0x94)
#define BCM2835_GPIO_GPPUDCLK0   (BCM2835_GPIO_REGS_BASE+0x98)  

/** @} */

/**
 * @name AUX Registers
 *
 * @{
 */

#define BCM2835_AUX_BASE         (RPI_PERIPHERAL_BASE + 0x215000)

#define AUX_ENABLES              (BCM2835_AUX_BASE+0x04)
#define AUX_MU_IO_REG            (BCM2835_AUX_BASE+0x40)
#define AUX_MU_IER_REG           (BCM2835_AUX_BASE+0x44)
#define AUX_MU_IIR_REG           (BCM2835_AUX_BASE+0x48)
#define AUX_MU_LCR_REG           (BCM2835_AUX_BASE+0x4C)
#define AUX_MU_MCR_REG           (BCM2835_AUX_BASE+0x50)
#define AUX_MU_LSR_REG           (BCM2835_AUX_BASE+0x54)
#define AUX_MU_MSR_REG           (BCM2835_AUX_BASE+0x58)
#define AUX_MU_SCRATCH           (BCM2835_AUX_BASE+0x5C)
#define AUX_MU_CNTL_REG          (BCM2835_AUX_BASE+0x60)
#define AUX_MU_STAT_REG          (BCM2835_AUX_BASE+0x64)
#define AUX_MU_BAUD_REG          (BCM2835_AUX_BASE+0x68)

/** @} */

/**
 * @name UART 0 (PL011) Registers
 *
 * @{
 */

#define BCM2835_UART0_BASE       (RPI_PERIPHERAL_BASE + 0x201000)

#define BCM2835_UART0_DR         (BCM2835_UART0_BASE+0x00)
#define BCM2835_UART0_RSRECR     (BCM2835_UART0_BASE+0x04)
#define BCM2835_UART0_FR         (BCM2835_UART0_BASE+0x18)
#define BCM2835_UART0_ILPR       (BCM2835_UART0_BASE+0x20)
#define BCM2835_UART0_IBRD       (BCM2835_UART0_BASE+0x24)
#define BCM2835_UART0_FBRD       (BCM2835_UART0_BASE+0x28)
#define BCM2835_UART0_LCRH       (BCM2835_UART0_BASE+0x2C)
#define BCM2835_UART0_CR         (BCM2835_UART0_BASE+0x30)
#define BCM2835_UART0_IFLS       (BCM2835_UART0_BASE+0x34)
#define BCM2835_UART0_IMSC       (BCM2835_UART0_BASE+0x38)
#define BCM2835_UART0_RIS        (BCM2835_UART0_BASE+0x3C)
#define BCM2835_UART0_MIS        (BCM2835_UART0_BASE+0x40)
#define BCM2835_UART0_ICR        (BCM2835_UART0_BASE+0x44)
#define BCM2835_UART0_DMACR      (BCM2835_UART0_BASE+0x48)
#define BCM2835_UART0_ITCR       (BCM2835_UART0_BASE+0x80)
#define BCM2835_UART0_ITIP       (BCM2835_UART0_BASE+0x84)
#define BCM2835_UART0_ITOP       (BCM2835_UART0_BASE+0x88)
#define BCM2835_UART0_TDR        (BCM2835_UART0_BASE+0x8C)

#define BCM2835_UART0_MIS_RX    0x10
#define BCM2835_UART0_MIS_TX    0x20
#define BCM2835_UART0_IMSC_RX   0x10
#define BCM2835_UART0_IMSC_TX   0x20
#define BCM2835_UART0_FR_RXFE   0x10
#define BCM2835_UART0_FR_TXFF   0x20
#define BCM2835_UART0_ICR_RX    0x10
#define BCM2835_UART0_ICR_TX    0x20

/** @} */

/**
 * @name I2C (BSC) Registers
 *
 * @{
 */

#define BCM2835_I2C_BASE           (0x20804000)

#define BCM2835_I2C_C              (BCM2835_I2C_BASE+0x00)
#define BCM2835_I2C_S              (BCM2835_I2C_BASE+0x04)
#define BCM2835_I2C_DLEN           (BCM2835_I2C_BASE+0x08)
#define BCM2835_I2C_A              (BCM2835_I2C_BASE+0x0C)
#define BCM2835_I2C_FIFO           (BCM2835_I2C_BASE+0x10)
#define BCM2835_I2C_DIV            (BCM2835_I2C_BASE+0x14)
#define BCM2835_I2C_DEL            (BCM2835_I2C_BASE+0x18)
#define BCM2835_I2C_CLKT           (BCM2835_I2C_BASE+0x1C)

/** @} */

/**
 * @name SPI Registers
 *
 * @{
 */

#define BCM2835_SPI_BASE           (0x20204000)

#define BCM2835_SPI_CS             (BCM2835_SPI_BASE+0x00)
#define BCM2835_SPI_FIFO           (BCM2835_SPI_BASE+0x04)
#define BCM2835_SPI_CLK            (BCM2835_SPI_BASE+0x08)
#define BCM2835_SPI_DLEN           (BCM2835_SPI_BASE+0x0C)
#define BCM2835_SPI_LTOH           (BCM2835_SPI_BASE+0x10)
#define BCM2835_SPI_DC             (BCM2835_SPI_BASE+0x14)

/** @} */

/**
 * @name I2C/SPI slave BSC Registers
 *
 * @{
 */

#define BCM2835_I2C_SPI_BASE       (0x20214000)

#define BCM2835_I2C_SPI_DR         (BCM2835_I2C_SPI_BASE+0x00)
#define BCM2835_I2C_SPI_RSR        (BCM2835_I2C_SPI_BASE+0x04)
#define BCM2835_I2C_SPI_SLV        (BCM2835_I2C_SPI_BASE+0x08)
#define BCM2835_I2C_SPI_CR         (BCM2835_I2C_SPI_BASE+0x0C)
#define BCM2835_I2C_SPI_FR         (BCM2835_I2C_SPI_BASE+0x10)
#define BCM2835_I2C_SPI_IFLS       (BCM2835_I2C_SPI_BASE+0x14)
#define BCM2835_I2C_SPI_IMSC       (BCM2835_I2C_SPI_BASE+0x18)
#define BCM2835_I2C_SPI_RIS        (BCM2835_I2C_SPI_BASE+0x1C)
#define BCM2835_I2C_SPI_MIS        (BCM2835_I2C_SPI_BASE+0x20)
#define BCM2835_I2C_SPI_ICR        (BCM2835_I2C_SPI_BASE+0x24)
#define BCM2835_I2C_SPI_DMACR      (BCM2835_I2C_SPI_BASE+0x28)
#define BCM2835_I2C_SPI_TDR        (BCM2835_I2C_SPI_BASE+0x2C)
#define BCM2835_I2C_SPI_GPUSTAT    (BCM2835_I2C_SPI_BASE+0x30)
#define BCM2835_I2C_SPI_HCTRL      (BCM2835_I2C_SPI_BASE+0x34)

/** @} */

/**
 * @name IRQ Registers
 *
 * @{
 */

#define BCM2835_BASE_INTC         (RPI_PERIPHERAL_BASE + 0xB200)

#define BCM2835_IRQ_BASIC         (BCM2835_BASE_INTC + 0x00)
#define BCM2835_IRQ_PENDING1      (BCM2835_BASE_INTC + 0x04)
#define BCM2835_IRQ_PENDING2      (BCM2835_BASE_INTC + 0x08)
#define BCM2835_IRQ_FIQ_CTRL      (BCM2835_BASE_INTC + 0x0C)
#define BCM2835_IRQ_ENABLE1       (BCM2835_BASE_INTC + 0x10)
#define BCM2835_IRQ_ENABLE2       (BCM2835_BASE_INTC + 0x14)
#define BCM2835_IRQ_ENABLE_BASIC  (BCM2835_BASE_INTC + 0x18)
#define BCM2835_IRQ_DISABLE1      (BCM2835_BASE_INTC + 0x1C)
#define BCM2835_IRQ_DISABLE2      (BCM2835_BASE_INTC + 0x20)
#define BCM2835_IRQ_DISABLE_BASIC (BCM2835_BASE_INTC + 0x24)

/** @} */

/**
 * @name GPU Timer Registers
 *
 * @{
 */

/**
 * NOTE: The GPU uses Compare registers 0 and 2 for
 *       it's own RTOS. 1 and 3 are available for use in
 *       RTEMS.
 */
#define BCM2835_GPU_TIMER_BASE    (RPI_PERIPHERAL_BASE + 0x3000)

#define BCM2835_GPU_TIMER_CS      (BCM2835_TIMER_BASE+0x00)
#define BCM2835_GPU_TIMER_CLO     (BCM2835_TIMER_BASE+0x04)
#define BCM2835_GPU_TIMER_CHI     (BCM2835_TIMER_BASE+0x08)
#define BCM2835_GPU_TIMER_C0      (BCM2835_TIMER_BASE+0x0C)
#define BCM2835_GPU_TIMER_C1      (BCM2835_TIMER_BASE+0x10)
#define BCM2835_GPU_TIMER_C2      (BCM2835_TIMER_BASE+0x14)
#define BCM2835_GPU_TIMER_C3      (BCM2835_TIMER_BASE+0x18)

/** @} */

/** @} */

#endif /* LIBBSP_ARM_RASPBERRYPI_RASPBERRYPI_H */
