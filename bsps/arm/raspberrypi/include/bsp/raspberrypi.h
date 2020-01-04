/**
 * @file
 *
 * @ingroup raspberrypi_reg
 *
 * @brief Register definitions.
 */

/*
 *  Copyright (c) 2014-2015 Andre Marques <andre.lousa.marques at gmail.com>
 *  Copyright (c) 2013 Alan Cudmore.
 *  Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_RASPBERRYPI_H
#define LIBBSP_ARM_RASPBERRYPI_RASPBERRYPI_H

#include <bspopts.h>
#include <stdint.h>
#include <bsp/utility.h>

/**
 * @defgroup raspberrypi_reg Register Definitions
 *
 * @ingroup RTEMSBSPsARMRaspberryPi
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
  #define RPI_PERIPHERAL_BASE    0x3F000000
  #define BASE_OFFSET            0X3F000000
#else
  #define RPI_PERIPHERAL_BASE    0x20000000
  #define BASE_OFFSET            0X5E000000
#endif

#define RPI_PERIPHERAL_SIZE      0x01000000

/**
 * @name Bus to Physical address translation
 *       Macro.
 * @{
 */

#define BUS_TO_PHY(x)            ((x) - BASE_OFFSET)

/** @} */

/**
 * @name Internal ARM Timer Registers
 *
 * @{
 */

#define BCM2835_CLOCK_FREQ       250000000

#define BCM2835_TIMER_BASE       (RPI_PERIPHERAL_BASE + 0xB400)

#define BCM2835_TIMER_LOD        (BCM2835_TIMER_BASE + 0x00)
#define BCM2835_TIMER_VAL        (BCM2835_TIMER_BASE + 0x04)
#define BCM2835_TIMER_CTL        (BCM2835_TIMER_BASE + 0x08)
#define BCM2835_TIMER_CLI        (BCM2835_TIMER_BASE + 0x0C)
#define BCM2835_TIMER_RIS        (BCM2835_TIMER_BASE + 0x10)
#define BCM2835_TIMER_MIS        (BCM2835_TIMER_BASE + 0x14)
#define BCM2835_TIMER_RLD        (BCM2835_TIMER_BASE + 0x18)
#define BCM2835_TIMER_DIV        (BCM2835_TIMER_BASE + 0x1C)
#define BCM2835_TIMER_CNT        (BCM2835_TIMER_BASE + 0x20)

#define BCM2835_TIMER_PRESCALE    0xF9

/** @} */

/**
 * @name Power Management and Watchdog  Registers
 *
 * @{
 */

#define BCM2835_PM_PASSWD_MAGIC  0x5a000000

#define BCM2835_PM_BASE          (RPI_PERIPHERAL_BASE + 0x100000)

#define BCM2835_PM_GNRIC         (BCM2835_PM_BASE + 0x00)
#define BCM2835_PM_GNRIC_POWUP   0x00000001
#define BCM2835_PM_GNRIC_POWOK   0x00000002
#define BCM2835_PM_GNRIC_ISPOW   0x00000004
#define BCM2835_PM_GNRIC_MEMREP  0x00000008
#define BCM2835_PM_GNRIC_MRDONE  0x00000010
#define BCM2835_PM_GNRIC_ISFUNC  0x00000020
#define BCM2835_PM_GNRIC_RSTN    0x00000fc0
#define BCM2835_PM_GNRIC_ENAB    0x00001000
#define BCM2835_PM_GNRIC_CFG     0x007f0000

#define BCM2835_PM_AUDIO         (BCM2835_PM_BASE + 0x04)
#define BCM2835_PM_AUDIO_APSM    0x000fffff
#define BCM2835_PM_AUDIO_CTRLEN  0x00100000
#define BCM2835_PM_AUDIO_RSTN    0x00200000

#define BCM2835_PM_STATUS        (BCM2835_PM_BASE + 0x18)

#define BCM2835_PM_RSTC          (BCM2835_PM_BASE + 0x1c)
#define BCM2835_PM_RSTC_DRCFG    0x00000003
#define BCM2835_PM_RSTC_WRCFG    0x00000030
#define BCM2835_PM_RSTC_WRCFG_FULL   0x00000020
#define BCM2835_PM_RSTC_SRCFG    0x00000300
#define BCM2835_PM_RSTC_QRCFG    0x00003000
#define BCM2835_PM_RSTC_FRCFG    0x00030000
#define BCM2835_PM_RSTC_HRCFG    0x00300000

#define BCM2835_PM_RSTS          (BCM2835_PM_BASE + 0x20)
#define BCM2835_PM_RSTS_HADDRQ   0x00000001
#define BCM2835_PM_RSTS_HADDRF   0x00000002
#define BCM2835_PM_RSTS_HADDRH   0x00000004
#define BCM2835_PM_RSTS_HADWRQ   0x00000010
#define BCM2835_PM_RSTS_HADWRF   0x00000020
#define BCM2835_PM_RSTS_HADWRH   0x00000040
#define BCM2835_PM_RSTS_HADSRQ   0x00000100
#define BCM2835_PM_RSTS_HADSRF   0x00000200
#define BCM2835_PM_RSTS_HADSRH   0x00000400
#define BCM2835_PM_RSTS_HADPOR   0x00001000

#define BCM2835_PM_WDOG          (BCM2835_PM_BASE + 0x24)

/** @} */

/**
 * @name GPIO Registers
 *
 * @{
 */

#define BCM2835_GPIO_REGS_BASE   (RPI_PERIPHERAL_BASE + 0x200000)

#define BCM2835_GPIO_GPFSEL1     (BCM2835_GPIO_REGS_BASE + 0x04)
#define BCM2835_GPIO_GPSET0      (BCM2835_GPIO_REGS_BASE + 0x1C)
#define BCM2835_GPIO_GPCLR0      (BCM2835_GPIO_REGS_BASE + 0x28)
#define BCM2835_GPIO_GPLEV0      (BCM2835_GPIO_REGS_BASE + 0x34)
#define BCM2835_GPIO_GPEDS0      (BCM2835_GPIO_REGS_BASE + 0x40)
#define BCM2835_GPIO_GPREN0      (BCM2835_GPIO_REGS_BASE + 0x4C)
#define BCM2835_GPIO_GPFEN0      (BCM2835_GPIO_REGS_BASE + 0x58)
#define BCM2835_GPIO_GPHEN0      (BCM2835_GPIO_REGS_BASE + 0x64)
#define BCM2835_GPIO_GPLEN0      (BCM2835_GPIO_REGS_BASE + 0x70)
#define BCM2835_GPIO_GPAREN0     (BCM2835_GPIO_REGS_BASE + 0x7C)
#define BCM2835_GPIO_GPAFEN0     (BCM2835_GPIO_REGS_BASE + 0x88)
#define BCM2835_GPIO_GPPUD       (BCM2835_GPIO_REGS_BASE + 0x94)
#define BCM2835_GPIO_GPPUDCLK0   (BCM2835_GPIO_REGS_BASE + 0x98)

/** @} */

/**
 * @name AUX Registers
 *
 * @{
 */

#define BCM2835_AUX_BASE         (RPI_PERIPHERAL_BASE + 0x215000)

#define AUX_ENABLES              (BCM2835_AUX_BASE + 0x04)
#define AUX_MU_IO_REG            (BCM2835_AUX_BASE + 0x40)
#define AUX_MU_IER_REG           (BCM2835_AUX_BASE + 0x44)
#define AUX_MU_IIR_REG           (BCM2835_AUX_BASE + 0x48)
#define AUX_MU_LCR_REG           (BCM2835_AUX_BASE + 0x4C)
#define AUX_MU_MCR_REG           (BCM2835_AUX_BASE + 0x50)
#define AUX_MU_LSR_REG           (BCM2835_AUX_BASE + 0x54)
#define AUX_MU_MSR_REG           (BCM2835_AUX_BASE + 0x58)
#define AUX_MU_SCRATCH           (BCM2835_AUX_BASE + 0x5C)
#define AUX_MU_CNTL_REG          (BCM2835_AUX_BASE + 0x60)
#define AUX_MU_STAT_REG          (BCM2835_AUX_BASE + 0x64)
#define AUX_MU_BAUD_REG          (BCM2835_AUX_BASE + 0x68)

/** @} */

/** @} */

/**
 * @name I2C (BSC) Registers
 *
 * @{
 */

#define BCM2835_I2C_BASE           (RPI_PERIPHERAL_BASE + 0x804000)

#define BCM2835_I2C_C              (BCM2835_I2C_BASE + 0x00)
#define BCM2835_I2C_S              (BCM2835_I2C_BASE + 0x04)
#define BCM2835_I2C_DLEN           (BCM2835_I2C_BASE + 0x08)
#define BCM2835_I2C_A              (BCM2835_I2C_BASE + 0x0C)
#define BCM2835_I2C_FIFO           (BCM2835_I2C_BASE + 0x10)
#define BCM2835_I2C_DIV            (BCM2835_I2C_BASE + 0x14)
#define BCM2835_I2C_DEL            (BCM2835_I2C_BASE + 0x18)
#define BCM2835_I2C_CLKT           (BCM2835_I2C_BASE + 0x1C)

/** @} */

/**
 * @name SPI Registers
 *
 * @{
 */

#define BCM2835_SPI_BASE           (RPI_PERIPHERAL_BASE + 0x204000)

#define BCM2835_SPI_CS             (BCM2835_SPI_BASE + 0x00)
#define BCM2835_SPI_FIFO           (BCM2835_SPI_BASE + 0x04)
#define BCM2835_SPI_CLK            (BCM2835_SPI_BASE + 0x08)
#define BCM2835_SPI_DLEN           (BCM2835_SPI_BASE + 0x0C)
#define BCM2835_SPI_LTOH           (BCM2835_SPI_BASE + 0x10)
#define BCM2835_SPI_DC             (BCM2835_SPI_BASE + 0x14)

/** @} */

/**
 * @name I2C/SPI slave BSC Registers
 *
 * @{
 */

#define BCM2835_I2C_SPI_BASE       (RPI_PERIPHERAL_BASE + 0x214000)

#define BCM2835_I2C_SPI_DR         (BCM2835_I2C_SPI_BASE + 0x00)
#define BCM2835_I2C_SPI_RSR        (BCM2835_I2C_SPI_BASE + 0x04)
#define BCM2835_I2C_SPI_SLV        (BCM2835_I2C_SPI_BASE + 0x08)
#define BCM2835_I2C_SPI_CR         (BCM2835_I2C_SPI_BASE + 0x0C)
#define BCM2835_I2C_SPI_FR         (BCM2835_I2C_SPI_BASE + 0x10)
#define BCM2835_I2C_SPI_IFLS       (BCM2835_I2C_SPI_BASE + 0x14)
#define BCM2835_I2C_SPI_IMSC       (BCM2835_I2C_SPI_BASE + 0x18)
#define BCM2835_I2C_SPI_RIS        (BCM2835_I2C_SPI_BASE + 0x1C)
#define BCM2835_I2C_SPI_MIS        (BCM2835_I2C_SPI_BASE + 0x20)
#define BCM2835_I2C_SPI_ICR        (BCM2835_I2C_SPI_BASE + 0x24)
#define BCM2835_I2C_SPI_DMACR      (BCM2835_I2C_SPI_BASE + 0x28)
#define BCM2835_I2C_SPI_TDR        (BCM2835_I2C_SPI_BASE + 0x2C)
#define BCM2835_I2C_SPI_GPUSTAT    (BCM2835_I2C_SPI_BASE + 0x30)
#define BCM2835_I2C_SPI_HCTRL      (BCM2835_I2C_SPI_BASE + 0x34)

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

#define BCM2835_GPU_TIMER_CS      (BCM2835_GPU_TIMER_BASE + 0x00)
#define BCM2835_GPU_TIMER_CS_M0   0x00000001
#define BCM2835_GPU_TIMER_CS_M1   0x00000002
#define BCM2835_GPU_TIMER_CS_M2   0x00000004
#define BCM2835_GPU_TIMER_CS_M3   0x00000008
#define BCM2835_GPU_TIMER_CLO     (BCM2835_GPU_TIMER_BASE + 0x04)
#define BCM2835_GPU_TIMER_CHI     (BCM2835_GPU_TIMER_BASE + 0x08)
#define BCM2835_GPU_TIMER_C0      (BCM2835_GPU_TIMER_BASE + 0x0C)
#define BCM2835_GPU_TIMER_C1      (BCM2835_GPU_TIMER_BASE + 0x10)
#define BCM2835_GPU_TIMER_C2      (BCM2835_GPU_TIMER_BASE + 0x14)
#define BCM2835_GPU_TIMER_C3      (BCM2835_GPU_TIMER_BASE + 0x18)

/** @} */

/**
 * @name EMMC Registers
 *
 * @{
 */

/**
 * NOTE: Since the SD controller follows the SDHCI standard,
 *       the rtems-libbsd tree already provides the remaining registers.
 */

#define BCM2835_EMMC_BASE              (RPI_PERIPHERAL_BASE + 0x300000)

/** @} */

/**
* @name Mailbox Registers
*
* @{
*/

#define BCM2835_MBOX_BASE   (RPI_PERIPHERAL_BASE+0xB880)

#define BCM2835_MBOX_PEEK   (BCM2835_MBOX_BASE+0x10)
#define BCM2835_MBOX_READ   (BCM2835_MBOX_BASE+0x00)
#define BCM2835_MBOX_WRITE  (BCM2835_MBOX_BASE+0x20)
#define BCM2835_MBOX_STATUS (BCM2835_MBOX_BASE+0x18)
#define BCM2835_MBOX_SENDER (BCM2835_MBOX_BASE+0x14)
#define BCM2835_MBOX_CONFIG (BCM2835_MBOX_BASE+0x1C)

#define BCM2835_MBOX_FULL   0x80000000
#define BCM2835_MBOX_EMPTY  0x40000000

/** @} */

/**
* @name Mailbox Channels
*
* @{
*/

/* Power Manager channel */
#define BCM2835_MBOX_CHANNEL_PM         0
/* Framebuffer channel */
#define BCM2835_MBOX_CHANNEL_FB         1
 /* Virtual UART channel */
#define BCM2835_MBOX_CHANNEL_VUART      2
 /* VCHIQ channel */
#define BCM2835_MBOX_CHANNEL_VCHIQ      3
 /* LEDs channel */
#define BCM2835_MBOX_CHANNEL_LED        4
 /* Button channel */
#define BCM2835_MBOX_CHANNEL_BUTTON     5
 /* Touch screen channel */
#define BCM2835_MBOX_CHANNEL_TOUCHS     6
/* Property tags (ARM <-> VC) channel */
#define BCM2835_MBOX_CHANNEL_PROP_AVC   8
 /* Property tags (VC <-> ARM) channel */
#define BCM2835_MBOX_CHANNEL_PROP_VCA   9

/** @} */

/**
* @name USB Registers
*
* @{
*/

#define BCM2835_USB_BASE	(RPI_PERIPHERAL_BASE + 0x980000) /* DTC_OTG USB controller */

/** @} */

/**
 * @name Raspberry Pi 2 CPU Cores Local Peripherals
 *
 * @{
 */

#define BCM2836_CORE_LOCAL_PERIPH_BASE    0x40000000
#define BCM2836_CORE_LOCAL_PERIPH_SIZE    0x00040000

/** @} */

/**
 * @name Raspberry Pi 2 Mailbox Register Defines
 *
 * @{
 */

#define BCM2836_MAILBOX_0_WRITE_SET_BASE  0x40000080
#define BCM2836_MAILBOX_1_WRITE_SET_BASE  0x40000084
#define BCM2836_MAILBOX_2_WRITE_SET_BASE  0x40000088
#define BCM2836_MAILBOX_3_WRITE_SET_BASE  0x4000008C
#define BCM2836_MAILBOX_0_READ_CLEAR_BASE 0x400000C0
#define BCM2836_MAILBOX_1_READ_CLEAR_BASE 0x400000C4
#define BCM2836_MAILBOX_2_READ_CLEAR_BASE 0x400000C8
#define BCM2836_MAILBOX_3_READ_CLEAR_BASE 0x400000CC

/** @} */

/**
 * @name Raspberry Pi 2 Core Timer
 *
 * @{
 */

#define BCM2836_CORE_TIMER_CTRL           0x40000000

#define BCM2836_CORE_TIMER_CTRL_APB_CLK   0x00000100
#define BCM2836_CORE_TIMER_CTRL_INC_2     0x00000200

#define BCM2836_CORE_TIMER_PRESCALER      0x40000008

#define BCM2836_CORE_TIMER_LS32           0x4000001C
#define BCM2836_CORE_TIMER_MS32           0x40000020

/** @} */

/**
 * @name Raspberry Pi 2 Local Timer
 *
 * @{
 */

#define BCM2836_LOCAL_TIMER_CTRL          0x40000034

#define BCM2836_LOCAL_TIMER_CTRL_IRQ_FLAG 0x80000000
#define BCM2836_LOCAL_TIMER_CTRL_IRQ_EN   0x20000000
#define BCM2836_LOCAL_TIMER_CTRL_TIMER_EN 0x10000000
#define BCM2836_LOCAL_TIMER_RELOAD        0x0FFFFFFF

#define BCM2836_LOCAL_TIMER_IRQ_RELOAD    0x40000038

#define BCM2836_LOCAL_TIMER_IRQ_CLEAR     0x80000000
#define BCM2836_LOCAL_TIMER_RELOAD_NOW    0x40000000

#define BCM2836_LOCAL_TIMER_IRQ_ROUTING   0x40000024
#define BCM2836_LOCAL_TIMER_ROU_CORE0_IRQ       0x00
#define BCM2836_LOCAL_TIMER_ROU_CORE1_IRQ       0x01
#define BCM2836_LOCAL_TIMER_ROU_CORE2_IRQ       0x02
#define BCM2836_LOCAL_TIMER_ROU_CORE3_IRQ       0x03
#define BCM2836_LOCAL_TIMER_ROU_CORE0_FIQ       0x04
#define BCM2836_LOCAL_TIMER_ROU_CORE1_FIQ       0x05
#define BCM2836_LOCAL_TIMER_ROU_CORE2_FIQ       0x06
#define BCM2836_LOCAL_TIMER_ROU_CORE3_FIQ       0x07

/** @} */

/**
 * @name Raspberry Pi 2 IRQ Routing
 *
 * @{
 */

#define BCM2836_GPU_IRQ_ROUTING           0x4000000C

#define BCM2836_GPU_IRQ_ROU_GPU_IRQ_CORE0 0x00000000
#define BCM2836_GPU_IRQ_ROU_GPU_IRQ_CORE1 0x00000001
#define BCM2836_GPU_IRQ_ROU_GPU_IRQ_CORE2 0x00000002
#define BCM2836_GPU_IRQ_ROU_GPU_IRQ_CORE4 0x00000003

#define BCM2836_GPU_IRQ_ROU_GPU_FIQ_CORE0 0x00000000
#define BCM2836_GPU_IRQ_ROU_GPU_FIQ_CORE1 0x00000004
#define BCM2836_GPU_IRQ_ROU_GPU_FIQ_CORE2 0x00000008
#define BCM2836_GPU_IRQ_ROU_GPU_FIQ_CORE4 0x0000000C

#define BCM2836_GPU_IRQ_ROU_GPU_FIQ_CORE4 0x0000000C


/** @} */

/**
 * @name Raspberry Pi 2 Interrupt Register Defines
 *
 * @{
 */

/* Timers interrupt control registers */
#define BCM2836_CORE0_TIMER_IRQ_CTRL_BASE 0x40000040
#define BCM2836_CORE1_TIMER_IRQ_CTRL_BASE 0x40000044
#define BCM2836_CORE2_TIMER_IRQ_CTRL_BASE 0x40000048
#define BCM2836_CORE3_TIMER_IRQ_CTRL_BASE 0x4000004C

#define BCM2836_CORE_TIMER_IRQ_CTRL(cpuidx) \
  (BCM2836_CORE0_TIMER_IRQ_CTRL_BASE + 0x4 * (cpuidx))

/*
 * Where to route timer interrupt to, IRQ/FIQ
 * Setting both the IRQ and FIQ bit gives an FIQ
 */
#define BCM2836_CORE_TIMER_IRQ_CTRL_TIMER0_IRQ  0x01
#define BCM2836_CORE_TIMER_IRQ_CTRL_TIMER1_IRQ  0x02
#define BCM2836_CORE_TIMER_IRQ_CTRL_TIMER2_IRQ  0x04
#define BCM2836_CORE_TIMER_IRQ_CTRL_TIMER3_IRQ  0x08
#define BCM2836_CORE_TIMER_IRQ_CTRL_TIMER0_FIQ  0x10
#define BCM2836_CORE_TIMER_IRQ_CTRL_TIMER1_FIQ  0x20
#define BCM2836_CORE_TIMER_IRQ_CTRL_TIMER2_FIQ  0x40
#define BCM2836_CORE_TIMER_IRQ_CTRL_TIMER3_FIQ  0x80

/* CPU mailbox registers */
#define BCM2836_MAILBOX_IRQ_CTRL_BASE     0x40000050
#define BCM2836_MAILBOX_IRQ_CTRL(cpuidx) \
  (BCM2836_MAILBOX_IRQ_CTRL_BASE + 0x4 * (cpuidx))
/*
 * Where to route mailbox interrupt to, IRQ/FIQ
 * Setting both the IRQ and FIQ bit gives an FIQ
 */
#define BCM2836_MAILBOX_IRQ_CTRL_MBOX0_IRQ      0x01
#define BCM2836_MAILBOX_IRQ_CTRL_MBOX1_IRQ      0x02
#define BCM2836_MAILBOX_IRQ_CTRL_MBOX2_IRQ      0x04
#define BCM2836_MAILBOX_IRQ_CTRL_MBOX3_IRQ      0x08
#define BCM2836_MAILBOX_IRQ_CTRL_MBOX0_FIQ      0x10
#define BCM2836_MAILBOX_IRQ_CTRL_MBOX1_FIQ      0x20
#define BCM2836_MAILBOX_IRQ_CTRL_MBOX2_FIQ      0x40
#define BCM2836_MAILBOX_IRQ_CTRL_MBOX3_FIQ      0x80

#define BCM2836_IRQ_SOURCE_REG_BASE       0x40000060
#define BCM2836_IRQ_SOURCE_REG(cpuidx) \
  (BCM2836_IRQ_SOURCE_REG_BASE + 0x4 * (cpuidx))

#define BCM2836_FIQ_SOURCE_REG_BASE       0x40000070
#define BCM2836_FIQ_SOURCE_REG(cpuidx) \
  (BCM2836_FIQ_SOURCE_REG_BASE + 0x4 * (cpuidx))

#define BCM2836_IRQ_SOURCE_TIMER0         0x00000001
#define BCM2836_IRQ_SOURCE_TIMER1         0x00000002
#define BCM2836_IRQ_SOURCE_TIMER2         0x00000004
#define BCM2836_IRQ_SOURCE_TIMER3         0x00000008
#define BCM2836_IRQ_SOURCE_MBOX0          0x00000010
#define BCM2836_IRQ_SOURCE_MBOX1          0x00000020
#define BCM2836_IRQ_SOURCE_MBOX2          0x00000040
#define BCM2836_IRQ_SOURCE_MBOX3          0x00000080
#define BCM2836_IRQ_SOURCE_GPU            0x00000100
#define BCM2836_IRQ_SOURCE_PMU            0x00000200
#define BCM2836_IRQ_SOURCE_LOCAL_TIMER    0x00000800

/** @} */

#endif /* LIBBSP_ARM_RASPBERRYPI_RASPBERRYPI_H */
