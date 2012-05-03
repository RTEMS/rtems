/**
 * @file
 *
 * @ingroup lpc24xx_regs
 *
 * @brief Register definitions.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC24XX_LPC24XX_H
#define LIBBSP_ARM_LPC24XX_LPC24XX_H

#include <rtems/score/cpu.h>
#include <bsp/utility.h>
#include <bsp/lpc-i2s.h>

#ifdef ARM_MULTILIB_ARCH_V7M
  #include <bsp/lpc17xx.h>
#endif

/**
 * @defgroup lpc24xx_regs Register Definitions
 *
 * @ingroup lpc24xx
 *
 * @brief Register definitions.
 *
 * @{
 */

#ifdef ARM_MULTILIB_ARCH_V4

/* Vectored Interrupt Controller (VIC) */
#define VIC_BASE_ADDR	0xFFFFF000
#define VICIRQStatus   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x000))
#define VICFIQStatus   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x004))
#define VICRawIntr     (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x008))
#define VICIntSelect   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x00C))
#define VICIntEnable   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x010))
#define VICIntEnClear  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x014))
#define VICSoftInt     (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x018))
#define VICSoftIntClear (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x01C))
#define VICProtection  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x020))
#define VICSWPrioMask  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x024))

#define VICVectAddrBase ((volatile uint32_t *) (VIC_BASE_ADDR + 0x100))
#define VICVectAddr0   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x100))
#define VICVectAddr1   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x104))
#define VICVectAddr2   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x108))
#define VICVectAddr3   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x10C))
#define VICVectAddr4   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x110))
#define VICVectAddr5   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x114))
#define VICVectAddr6   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x118))
#define VICVectAddr7   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x11C))
#define VICVectAddr8   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x120))
#define VICVectAddr9   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x124))
#define VICVectAddr10  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x128))
#define VICVectAddr11  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x12C))
#define VICVectAddr12  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x130))
#define VICVectAddr13  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x134))
#define VICVectAddr14  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x138))
#define VICVectAddr15  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x13C))
#define VICVectAddr16  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x140))
#define VICVectAddr17  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x144))
#define VICVectAddr18  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x148))
#define VICVectAddr19  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x14C))
#define VICVectAddr20  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x150))
#define VICVectAddr21  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x154))
#define VICVectAddr22  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x158))
#define VICVectAddr23  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x15C))
#define VICVectAddr24  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x160))
#define VICVectAddr25  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x164))
#define VICVectAddr26  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x168))
#define VICVectAddr27  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x16C))
#define VICVectAddr28  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x170))
#define VICVectAddr29  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x174))
#define VICVectAddr30  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x178))
#define VICVectAddr31  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x17C))

#define VICVectPriorityBase ((volatile uint32_t *) (VIC_BASE_ADDR + 0x200))
#define VICVectPriority0   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x200))
#define VICVectPriority1   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x204))
#define VICVectPriority2   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x208))
#define VICVectPriority3   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x20C))
#define VICVectPriority4   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x210))
#define VICVectPriority5   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x214))
#define VICVectPriority6   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x218))
#define VICVectPriority7   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x21C))
#define VICVectPriority8   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x220))
#define VICVectPriority9   (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x224))
#define VICVectPriority10  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x228))
#define VICVectPriority11  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x22C))
#define VICVectPriority12  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x230))
#define VICVectPriority13  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x234))
#define VICVectPriority14  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x238))
#define VICVectPriority15  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x23C))
#define VICVectPriority16  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x240))
#define VICVectPriority17  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x244))
#define VICVectPriority18  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x248))
#define VICVectPriority19  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x24C))
#define VICVectPriority20  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x250))
#define VICVectPriority21  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x254))
#define VICVectPriority22  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x258))
#define VICVectPriority23  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x25C))
#define VICVectPriority24  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x260))
#define VICVectPriority25  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x264))
#define VICVectPriority26  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x268))
#define VICVectPriority27  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x26C))
#define VICVectPriority28  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x270))
#define VICVectPriority29  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x274))
#define VICVectPriority30  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x278))
#define VICVectPriority31  (*(volatile uint32_t *) (VIC_BASE_ADDR + 0x27C))

#define VICVectAddr    (*(volatile uint32_t *) (VIC_BASE_ADDR + 0xF00))

#endif /* ARM_MULTILIB_ARCH_V4 */

/* Pin Connect Block */
#ifdef ARM_MULTILIB_ARCH_V4
  #define PINSEL_BASE_ADDR	0xE002C000
#else
  #define PINSEL_BASE_ADDR	0x4002C000
#endif

#ifdef ARM_MULTILIB_ARCH_V4

#define PINSEL0        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x00))
#define PINSEL1        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x04))
#define PINSEL2        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x08))
#define PINSEL3        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x0C))
#define PINSEL4        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x10))
#define PINSEL5        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x14))
#define PINSEL6        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x18))
#define PINSEL7        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x1C))
#define PINSEL8        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x20))
#define PINSEL9        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x24))
#define PINSEL10       (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x28))
#define PINSEL11       (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x2C))

#define PINMODE0        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x40))
#define PINMODE1        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x44))
#define PINMODE2        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x48))
#define PINMODE3        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x4C))
#define PINMODE4        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x50))
#define PINMODE5        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x54))
#define PINMODE6        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x58))
#define PINMODE7        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x5C))
#define PINMODE8        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x60))
#define PINMODE9        (*(volatile uint32_t *) (PINSEL_BASE_ADDR + 0x64))

#endif /* ARM_MULTILIB_ARCH_V4 */

/* General Purpose Input/Output (GPIO) */
#ifdef ARM_MULTILIB_ARCH_V4
  #define GPIO_BASE_ADDR		0xE0028000
#else
  #define GPIO_BASE_ADDR		0x40028000
#endif
#define IOPIN0         (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x00))
#define IOSET0         (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x04))
#define IODIR0         (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x08))
#define IOCLR0         (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x0C))
#define IOPIN1         (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x10))
#define IOSET1         (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x14))
#define IODIR1         (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x18))
#define IOCLR1         (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x1C))

/* GPIO Interrupt Registers */
#define IO0_INT_EN_R    (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x90))
#define IO0_INT_EN_F    (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x94))
#define IO0_INT_STAT_R  (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x84))
#define IO0_INT_STAT_F  (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x88))
#define IO0_INT_CLR     (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x8C))

#define IO2_INT_EN_R    (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0xB0))
#define IO2_INT_EN_F    (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0xB4))
#define IO2_INT_STAT_R  (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0xA4))
#define IO2_INT_STAT_F  (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0xA8))
#define IO2_INT_CLR     (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0xAC))

#define IO_INT_STAT     (*(volatile uint32_t *) (GPIO_BASE_ADDR + 0x80))

#ifdef ARM_MULTILIB_ARCH_V4

#define PARTCFG_BASE_ADDR		0x3FFF8000
#define PARTCFG        (*(volatile uint32_t *) (PARTCFG_BASE_ADDR + 0x00))

#endif /* ARM_MULTILIB_ARCH_V4 */

/* Fast I/O setup */
#ifdef ARM_MULTILIB_ARCH_V4
  #define FIO_BASE_ADDR		0x3FFFC000
#else
  #define FIO_BASE_ADDR		0x20098000
#endif
#define FIO0DIR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x00))
#define FIO0MASK       (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x10))
#define FIO0PIN        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x14))
#define FIO0SET        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x18))
#define FIO0CLR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x1C))

#define FIO1DIR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x20))
#define FIO1MASK       (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x30))
#define FIO1PIN        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x34))
#define FIO1SET        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x38))
#define FIO1CLR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x3C))

#define FIO2DIR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x40))
#define FIO2MASK       (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x50))
#define FIO2PIN        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x54))
#define FIO2SET        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x58))
#define FIO2CLR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x5C))

#define FIO3DIR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x60))
#define FIO3MASK       (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x70))
#define FIO3PIN        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x74))
#define FIO3SET        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x78))
#define FIO3CLR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x7C))

#define FIO4DIR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x80))
#define FIO4MASK       (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x90))
#define FIO4PIN        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x94))
#define FIO4SET        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x98))
#define FIO4CLR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0x9C))

#ifdef ARM_MULTILIB_ARCH_V7M

#define FIO5DIR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0xa0))
#define FIO5MASK       (*(volatile uint32_t *) (FIO_BASE_ADDR + 0xb0))
#define FIO5PIN        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0xb4))
#define FIO5SET        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0xb8))
#define FIO5CLR        (*(volatile uint32_t *) (FIO_BASE_ADDR + 0xbC))

#endif /* ARM_MULTILIB_ARCH_V7M */

/* FIOs can be accessed through WORD, HALF-WORD or BYTE. */
#define FIO0DIR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x00))
#define FIO1DIR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x20))
#define FIO2DIR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x40))
#define FIO3DIR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x60))
#define FIO4DIR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x80))

#define FIO0DIR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x01))
#define FIO1DIR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x21))
#define FIO2DIR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x41))
#define FIO3DIR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x61))
#define FIO4DIR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x81))

#define FIO0DIR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x02))
#define FIO1DIR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x22))
#define FIO2DIR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x42))
#define FIO3DIR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x62))
#define FIO4DIR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x82))

#define FIO0DIR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x03))
#define FIO1DIR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x23))
#define FIO2DIR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x43))
#define FIO3DIR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x63))
#define FIO4DIR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x83))

#define FIO0DIRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x00))
#define FIO1DIRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x20))
#define FIO2DIRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x40))
#define FIO3DIRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x60))
#define FIO4DIRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x80))

#define FIO0DIRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x02))
#define FIO1DIRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x22))
#define FIO2DIRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x42))
#define FIO3DIRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x62))
#define FIO4DIRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x82))

#define FIO0MASK0      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x10))
#define FIO1MASK0      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x30))
#define FIO2MASK0      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x50))
#define FIO3MASK0      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x70))
#define FIO4MASK0      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x90))

#define FIO0MASK1      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x11))
#define FIO1MASK1      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x21))
#define FIO2MASK1      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x51))
#define FIO3MASK1      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x71))
#define FIO4MASK1      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x91))

#define FIO0MASK2      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x12))
#define FIO1MASK2      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x32))
#define FIO2MASK2      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x52))
#define FIO3MASK2      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x72))
#define FIO4MASK2      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x92))

#define FIO0MASK3      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x13))
#define FIO1MASK3      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x33))
#define FIO2MASK3      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x53))
#define FIO3MASK3      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x73))
#define FIO4MASK3      (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x93))

#define FIO0MASKL      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x10))
#define FIO1MASKL      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x30))
#define FIO2MASKL      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x50))
#define FIO3MASKL      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x70))
#define FIO4MASKL      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x90))

#define FIO0MASKU      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x12))
#define FIO1MASKU      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x32))
#define FIO2MASKU      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x52))
#define FIO3MASKU      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x72))
#define FIO4MASKU      (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x92))

#define FIO0PIN0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x14))
#define FIO1PIN0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x34))
#define FIO2PIN0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x54))
#define FIO3PIN0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x74))
#define FIO4PIN0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x94))

#define FIO0PIN1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x15))
#define FIO1PIN1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x25))
#define FIO2PIN1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x55))
#define FIO3PIN1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x75))
#define FIO4PIN1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x95))

#define FIO0PIN2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x16))
#define FIO1PIN2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x36))
#define FIO2PIN2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x56))
#define FIO3PIN2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x76))
#define FIO4PIN2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x96))

#define FIO0PIN3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x17))
#define FIO1PIN3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x37))
#define FIO2PIN3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x57))
#define FIO3PIN3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x77))
#define FIO4PIN3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x97))

#define FIO0PINL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x14))
#define FIO1PINL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x34))
#define FIO2PINL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x54))
#define FIO3PINL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x74))
#define FIO4PINL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x94))

#define FIO0PINU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x16))
#define FIO1PINU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x36))
#define FIO2PINU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x56))
#define FIO3PINU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x76))
#define FIO4PINU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x96))

#define FIO0SET0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x18))
#define FIO1SET0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x38))
#define FIO2SET0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x58))
#define FIO3SET0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x78))
#define FIO4SET0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x98))

#define FIO0SET1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x19))
#define FIO1SET1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x29))
#define FIO2SET1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x59))
#define FIO3SET1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x79))
#define FIO4SET1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x99))

#define FIO0SET2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x1A))
#define FIO1SET2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x3A))
#define FIO2SET2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x5A))
#define FIO3SET2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x7A))
#define FIO4SET2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x9A))

#define FIO0SET3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x1B))
#define FIO1SET3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x3B))
#define FIO2SET3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x5B))
#define FIO3SET3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x7B))
#define FIO4SET3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x9B))

#define FIO0SETL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x18))
#define FIO1SETL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x38))
#define FIO2SETL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x58))
#define FIO3SETL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x78))
#define FIO4SETL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x98))

#define FIO0SETU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x1A))
#define FIO1SETU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x3A))
#define FIO2SETU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x5A))
#define FIO3SETU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x7A))
#define FIO4SETU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x9A))

#define FIO0CLR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x1C))
#define FIO1CLR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x3C))
#define FIO2CLR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x5C))
#define FIO3CLR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x7C))
#define FIO4CLR0       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x9C))

#define FIO0CLR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x1D))
#define FIO1CLR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x2D))
#define FIO2CLR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x5D))
#define FIO3CLR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x7D))
#define FIO4CLR1       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x9D))

#define FIO0CLR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x1E))
#define FIO1CLR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x3E))
#define FIO2CLR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x5E))
#define FIO3CLR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x7E))
#define FIO4CLR2       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x9E))

#define FIO0CLR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x1F))
#define FIO1CLR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x3F))
#define FIO2CLR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x5F))
#define FIO3CLR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x7F))
#define FIO4CLR3       (*(volatile uint8_t *) (FIO_BASE_ADDR + 0x9F))

#define FIO0CLRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x1C))
#define FIO1CLRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x3C))
#define FIO2CLRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x5C))
#define FIO3CLRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x7C))
#define FIO4CLRL       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x9C))

#define FIO0CLRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x1E))
#define FIO1CLRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x3E))
#define FIO2CLRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x5E))
#define FIO3CLRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x7E))
#define FIO4CLRU       (*(volatile uint16_t *) (FIO_BASE_ADDR + 0x9E))

#ifdef ARM_MULTILIB_ARCH_V4

/* System Control Block(SCB) modules include Memory Accelerator Module,
Phase Locked Loop, VPB divider, Power Control, External Interrupt,
Reset, and Code Security/Debugging */
#define SCB_BASE_ADDR	0xE01FC000

/* Memory Accelerator Module (MAM) */
#define MAMCR          (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x000))
#define MAMTIM         (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x004))
#define MEMMAP         (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x040))

/* Phase Locked Loop (PLL) */
#define PLLCON         (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x080))
#define PLLCFG         (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x084))
#define PLLSTAT        (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x088))
#define PLLFEED        (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x08C))

/* Power Control */
#define PCON           (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x0C0))
#define PCONP          (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x0C4))

/* Clock Divider */
// #define APBDIV         (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x100))
#define CCLKCFG        (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x104))
#define USBCLKCFG      (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x108))
#define CLKSRCSEL      (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x10C))
#define PCLKSEL0       (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x1A8))
#define PCLKSEL1       (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x1AC))

/* External Interrupts */
#define EXTINT         (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x140))
#define INTWAKE        (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x144))
#define EXTMODE        (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x148))
#define EXTPOLAR       (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x14C))

/* Reset, reset source identification */
#define RSIR           (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x180))

/* RSID, code security protection */
#define CSPR           (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x184))

/* AHB configuration */
#define AHBCFG1        (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x188))
#define AHBCFG2        (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x18C))

/* System Controls and Status */
#define SCS            (*(volatile uint32_t *) (SCB_BASE_ADDR + 0x1A0))

#endif /* ARM_MULTILIB_ARCH_V4 */

/* External Memory Controller (EMC) */
#ifdef ARM_MULTILIB_ARCH_V4
  #define EMC_BASE_ADDR		0xFFE08000
#else
  #define EMC_BASE_ADDR		0x2009c000
#endif
#define EMC_CTRL       (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x000))
#define EMC_STAT       (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x004))
#define EMC_CONFIG     (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x008))

/* Dynamic RAM access registers */
#define EMC_DYN_CTRL     (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x020))
#define EMC_DYN_RFSH     (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x024))
#define EMC_DYN_RD_CFG   (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x028))
#define EMC_DYN_RP       (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x030))
#define EMC_DYN_RAS      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x034))
#define EMC_DYN_SREX     (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x038))
#define EMC_DYN_APR      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x03C))
#define EMC_DYN_DAL      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x040))
#define EMC_DYN_WR       (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x044))
#define EMC_DYN_RC       (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x048))
#define EMC_DYN_RFC      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x04C))
#define EMC_DYN_XSR      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x050))
#define EMC_DYN_RRD      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x054))
#define EMC_DYN_MRD      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x058))

#define EMC_DYN_CFG0     (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x100))
#define EMC_DYN_RASCAS0  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x104))
#define EMC_DYN_CFG1     (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x140))
#define EMC_DYN_RASCAS1  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x144))
#define EMC_DYN_CFG2     (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x160))
#define EMC_DYN_RASCAS2  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x164))
#define EMC_DYN_CFG3     (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x180))
#define EMC_DYN_RASCAS3  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x184))

/* static RAM access registers */
#define EMC_STA_BASE_0    ((uint32_t *) (EMC_BASE_ADDR + 0x200))
#define EMC_STA_CFG0      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x200))
#define EMC_STA_WAITWEN0  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x204))
#define EMC_STA_WAITOEN0  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x208))
#define EMC_STA_WAITRD0   (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x20C))
#define EMC_STA_WAITPAGE0 (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x210))
#define EMC_STA_WAITWR0   (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x214))
#define EMC_STA_WAITTURN0 (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x218))

#define EMC_STA_BASE_1    ((uint32_t *) (EMC_BASE_ADDR + 0x220))
#define EMC_STA_CFG1      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x220))
#define EMC_STA_WAITWEN1  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x224))
#define EMC_STA_WAITOEN1  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x228))
#define EMC_STA_WAITRD1   (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x22C))
#define EMC_STA_WAITPAGE1 (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x230))
#define EMC_STA_WAITWR1   (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x234))
#define EMC_STA_WAITTURN1 (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x238))

#define EMC_STA_BASE_2    ((uint32_t *) (EMC_BASE_ADDR + 0x240))
#define EMC_STA_CFG2      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x240))
#define EMC_STA_WAITWEN2  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x244))
#define EMC_STA_WAITOEN2  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x248))
#define EMC_STA_WAITRD2   (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x24C))
#define EMC_STA_WAITPAGE2 (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x250))
#define EMC_STA_WAITWR2   (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x254))
#define EMC_STA_WAITTURN2 (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x258))

#define EMC_STA_BASE_3    ((uint32_t *) (EMC_BASE_ADDR + 0x260))
#define EMC_STA_CFG3      (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x260))
#define EMC_STA_WAITWEN3  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x264))
#define EMC_STA_WAITOEN3  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x268))
#define EMC_STA_WAITRD3   (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x26C))
#define EMC_STA_WAITPAGE3 (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x270))
#define EMC_STA_WAITWR3   (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x274))
#define EMC_STA_WAITTURN3 (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x278))

#define EMC_STA_EXT_WAIT  (*(volatile uint32_t *) (EMC_BASE_ADDR + 0x880))


/* Timer 0 */
#ifdef ARM_MULTILIB_ARCH_V4
  #define TMR0_BASE_ADDR		0xE0004000
#else
  #define TMR0_BASE_ADDR		0x40004000
#endif
#define T0IR           (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x00))
#define T0TCR          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x04))
#define T0TC           (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x08))
#define T0PR           (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x0C))
#define T0PC           (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x10))
#define T0MCR          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x14))
#define T0MR0          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x18))
#define T0MR1          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x1C))
#define T0MR2          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x20))
#define T0MR3          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x24))
#define T0CCR          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x28))
#define T0CR0          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x2C))
#define T0CR1          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x30))
#define T0CR2          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x34))
#define T0CR3          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x38))
#define T0EMR          (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x3C))
#define T0CTCR         (*(volatile uint32_t *) (TMR0_BASE_ADDR + 0x70))

/* Timer 1 */
#ifdef ARM_MULTILIB_ARCH_V4
  #define TMR1_BASE_ADDR		0xE0008000
#else
  #define TMR1_BASE_ADDR		0x40008000
#endif
#define T1IR           (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x00))
#define T1TCR          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x04))
#define T1TC           (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x08))
#define T1PR           (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x0C))
#define T1PC           (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x10))
#define T1MCR          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x14))
#define T1MR0          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x18))
#define T1MR1          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x1C))
#define T1MR2          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x20))
#define T1MR3          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x24))
#define T1CCR          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x28))
#define T1CR0          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x2C))
#define T1CR1          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x30))
#define T1CR2          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x34))
#define T1CR3          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x38))
#define T1EMR          (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x3C))
#define T1CTCR         (*(volatile uint32_t *) (TMR1_BASE_ADDR + 0x70))

/* Timer 2 */
#ifdef ARM_MULTILIB_ARCH_V4
  #define TMR2_BASE_ADDR		0xE0070000
#else
  #define TMR2_BASE_ADDR		0x40090000
#endif
#define T2IR           (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x00))
#define T2TCR          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x04))
#define T2TC           (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x08))
#define T2PR           (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x0C))
#define T2PC           (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x10))
#define T2MCR          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x14))
#define T2MR0          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x18))
#define T2MR1          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x1C))
#define T2MR2          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x20))
#define T2MR3          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x24))
#define T2CCR          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x28))
#define T2CR0          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x2C))
#define T2CR1          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x30))
#define T2CR2          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x34))
#define T2CR3          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x38))
#define T2EMR          (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x3C))
#define T2CTCR         (*(volatile uint32_t *) (TMR2_BASE_ADDR + 0x70))

/* Timer 3 */
#ifdef ARM_MULTILIB_ARCH_V4
  #define TMR3_BASE_ADDR		0xE0074000
#else
  #define TMR3_BASE_ADDR		0x40094000
#endif
#define T3IR           (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x00))
#define T3TCR          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x04))
#define T3TC           (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x08))
#define T3PR           (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x0C))
#define T3PC           (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x10))
#define T3MCR          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x14))
#define T3MR0          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x18))
#define T3MR1          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x1C))
#define T3MR2          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x20))
#define T3MR3          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x24))
#define T3CCR          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x28))
#define T3CR0          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x2C))
#define T3CR1          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x30))
#define T3CR2          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x34))
#define T3CR3          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x38))
#define T3EMR          (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x3C))
#define T3CTCR         (*(volatile uint32_t *) (TMR3_BASE_ADDR + 0x70))


/* Pulse Width Modulator (PWM) */
#ifdef ARM_MULTILIB_ARCH_V4
  #define PWM0_BASE_ADDR		0xE0014000
#else
  #define PWM0_BASE_ADDR		0x40014000
#endif
#define PWM0IR          (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x00))
#define PWM0TCR         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x04))
#define PWM0TC          (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x08))
#define PWM0PR          (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x0C))
#define PWM0PC          (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x10))
#define PWM0MCR         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x14))
#define PWM0MR0         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x18))
#define PWM0MR1         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x1C))
#define PWM0MR2         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x20))
#define PWM0MR3         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x24))
#define PWM0CCR         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x28))
#define PWM0CR0         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x2C))
#define PWM0CR1         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x30))
#define PWM0CR2         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x34))
#define PWM0CR3         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x38))
#define PWM0EMR         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x3C))
#define PWM0MR4         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x40))
#define PWM0MR5         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x44))
#define PWM0MR6         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x48))
#define PWM0PCR         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x4C))
#define PWM0LER         (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x50))
#define PWM0CTCR        (*(volatile uint32_t *) (PWM0_BASE_ADDR + 0x70))

#ifdef ARM_MULTILIB_ARCH_V4
  #define PWM1_BASE_ADDR		0xE0018000
#else
  #define PWM1_BASE_ADDR		0x40018000
#endif
#define PWM1IR          (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x00))
#define PWM1TCR         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x04))
#define PWM1TC          (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x08))
#define PWM1PR          (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x0C))
#define PWM1PC          (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x10))
#define PWM1MCR         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x14))
#define PWM1MR0         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x18))
#define PWM1MR1         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x1C))
#define PWM1MR2         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x20))
#define PWM1MR3         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x24))
#define PWM1CCR         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x28))
#define PWM1CR0         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x2C))
#define PWM1CR1         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x30))
#define PWM1CR2         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x34))
#define PWM1CR3         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x38))
#define PWM1EMR         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x3C))
#define PWM1MR4         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x40))
#define PWM1MR5         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x44))
#define PWM1MR6         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x48))
#define PWM1PCR         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x4C))
#define PWM1LER         (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x50))
#define PWM1CTCR        (*(volatile uint32_t *) (PWM1_BASE_ADDR + 0x70))


/* Universal Asynchronous Receiver Transmitter 0 (UART0) */
#ifdef ARM_MULTILIB_ARCH_V4
  #define UART0_BASE_ADDR		0xE000C000
#else
  #define UART0_BASE_ADDR		0x4000C000
#endif
#define U0RBR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x00))
#define U0THR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x00))
#define U0DLL          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x00))
#define U0DLM          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x04))
#define U0IER          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x04))
#define U0IIR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x08))
#define U0FCR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x08))
#define U0LCR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x0C))
#define U0LSR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x14))
#define U0SCR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x1C))
#define U0ACR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x20))
#define U0ICR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x24))
#define U0FDR          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x28))
#define U0TER          (*(volatile uint32_t *) (UART0_BASE_ADDR + 0x30))

/* Universal Asynchronous Receiver Transmitter 1 (UART1) */
#ifdef ARM_MULTILIB_ARCH_V4
  #define UART1_BASE_ADDR		0xE0010000
#else
  #define UART1_BASE_ADDR		0x40010000
#endif
#define U1RBR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x00))
#define U1THR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x00))
#define U1DLL          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x00))
#define U1DLM          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x04))
#define U1IER          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x04))
#define U1IIR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x08))
#define U1FCR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x08))
#define U1LCR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x0C))
#define U1MCR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x10))
#define U1LSR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x14))
#define U1MSR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x18))
#define U1SCR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x1C))
#define U1ACR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x20))
#define U1FDR          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x28))
#define U1TER          (*(volatile uint32_t *) (UART1_BASE_ADDR + 0x30))

/* Universal Asynchronous Receiver Transmitter 2 (UART2) */
#ifdef ARM_MULTILIB_ARCH_V4
  #define UART2_BASE_ADDR		0xE0078000
#else
  #define UART2_BASE_ADDR		0x40098000
#endif
#define U2RBR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x00))
#define U2THR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x00))
#define U2DLL          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x00))
#define U2DLM          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x04))
#define U2IER          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x04))
#define U2IIR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x08))
#define U2FCR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x08))
#define U2LCR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x0C))
#define U2LSR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x14))
#define U2SCR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x1C))
#define U2ACR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x20))
#define U2ICR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x24))
#define U2FDR          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x28))
#define U2TER          (*(volatile uint32_t *) (UART2_BASE_ADDR + 0x30))

/* Universal Asynchronous Receiver Transmitter 3 (UART3) */
#ifdef ARM_MULTILIB_ARCH_V4
  #define UART3_BASE_ADDR		0xE007C000
#else
  #define UART3_BASE_ADDR		0x4009C000
#endif
#define U3RBR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x00))
#define U3THR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x00))
#define U3DLL          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x00))
#define U3DLM          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x04))
#define U3IER          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x04))
#define U3IIR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x08))
#define U3FCR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x08))
#define U3LCR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x0C))
#define U3LSR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x14))
#define U3SCR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x1C))
#define U3ACR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x20))
#define U3ICR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x24))
#define U3FDR          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x28))
#define U3TER          (*(volatile uint32_t *) (UART3_BASE_ADDR + 0x30))

/* I2C Interface 0 */
#ifdef ARM_MULTILIB_ARCH_V4
  #define I2C0_BASE_ADDR		0xE001C000
#else
  #define I2C0_BASE_ADDR		0x4001C000
#endif
#define I20CONSET      (*(volatile uint32_t *) (I2C0_BASE_ADDR + 0x00))
#define I20STAT        (*(volatile uint32_t *) (I2C0_BASE_ADDR + 0x04))
#define I20DAT         (*(volatile uint32_t *) (I2C0_BASE_ADDR + 0x08))
#define I20ADR         (*(volatile uint32_t *) (I2C0_BASE_ADDR + 0x0C))
#define I20SCLH        (*(volatile uint32_t *) (I2C0_BASE_ADDR + 0x10))
#define I20SCLL        (*(volatile uint32_t *) (I2C0_BASE_ADDR + 0x14))
#define I20CONCLR      (*(volatile uint32_t *) (I2C0_BASE_ADDR + 0x18))

/* I2C Interface 1 */
#ifdef ARM_MULTILIB_ARCH_V4
  #define I2C1_BASE_ADDR		0xE005C000
#else
  #define I2C1_BASE_ADDR		0x4005C000
#endif
#define I21CONSET      (*(volatile uint32_t *) (I2C1_BASE_ADDR + 0x00))
#define I21STAT        (*(volatile uint32_t *) (I2C1_BASE_ADDR + 0x04))
#define I21DAT         (*(volatile uint32_t *) (I2C1_BASE_ADDR + 0x08))
#define I21ADR         (*(volatile uint32_t *) (I2C1_BASE_ADDR + 0x0C))
#define I21SCLH        (*(volatile uint32_t *) (I2C1_BASE_ADDR + 0x10))
#define I21SCLL        (*(volatile uint32_t *) (I2C1_BASE_ADDR + 0x14))
#define I21CONCLR      (*(volatile uint32_t *) (I2C1_BASE_ADDR + 0x18))

/* I2C Interface 2 */
#ifdef ARM_MULTILIB_ARCH_V4
  #define I2C2_BASE_ADDR		0xE0080000
#else
  #define I2C2_BASE_ADDR		0x400a0000
#endif
#define I22CONSET      (*(volatile uint32_t *) (I2C2_BASE_ADDR + 0x00))
#define I22STAT        (*(volatile uint32_t *) (I2C2_BASE_ADDR + 0x04))
#define I22DAT         (*(volatile uint32_t *) (I2C2_BASE_ADDR + 0x08))
#define I22ADR         (*(volatile uint32_t *) (I2C2_BASE_ADDR + 0x0C))
#define I22SCLH        (*(volatile uint32_t *) (I2C2_BASE_ADDR + 0x10))
#define I22SCLL        (*(volatile uint32_t *) (I2C2_BASE_ADDR + 0x14))
#define I22CONCLR      (*(volatile uint32_t *) (I2C2_BASE_ADDR + 0x18))

/* SPI0 (Serial Peripheral Interface 0) */
#define SPI0_BASE_ADDR		0xE0020000
#define S0SPCR         (*(volatile uint32_t *) (SPI0_BASE_ADDR + 0x00))
#define S0SPSR         (*(volatile uint32_t *) (SPI0_BASE_ADDR + 0x04))
#define S0SPDR         (*(volatile uint32_t *) (SPI0_BASE_ADDR + 0x08))
#define S0SPCCR        (*(volatile uint32_t *) (SPI0_BASE_ADDR + 0x0C))
#define S0SPINT        (*(volatile uint32_t *) (SPI0_BASE_ADDR + 0x1C))

/* SSP0 Controller */
#ifdef ARM_MULTILIB_ARCH_V4
  #define SSP0_BASE_ADDR		0xE0068000
#else
  #define SSP0_BASE_ADDR		0x40088000
#endif
#define SSP0CR0        (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x00))
#define SSP0CR1        (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x04))
#define SSP0DR         (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x08))
#define SSP0SR         (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x0C))
#define SSP0CPSR       (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x10))
#define SSP0IMSC       (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x14))
#define SSP0RIS        (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x18))
#define SSP0MIS        (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x1C))
#define SSP0ICR        (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x20))
#define SSP0DMACR      (*(volatile uint32_t *) (SSP0_BASE_ADDR + 0x24))

/* SSP1 Controller */
#ifdef ARM_MULTILIB_ARCH_V4
  #define SSP1_BASE_ADDR		0xE0030000
#else
  #define SSP1_BASE_ADDR		0x40030000
#endif
#define SSP1CR0        (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x00))
#define SSP1CR1        (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x04))
#define SSP1DR         (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x08))
#define SSP1SR         (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x0C))
#define SSP1CPSR       (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x10))
#define SSP1IMSC       (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x14))
#define SSP1RIS        (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x18))
#define SSP1MIS        (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x1C))
#define SSP1ICR        (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x20))
#define SSP1DMACR      (*(volatile uint32_t *) (SSP1_BASE_ADDR + 0x24))

#ifdef ARM_MULTILIB_ARCH_V7M

/* SSP2 Controller */
#define SSP2_BASE_ADDR		0x400ac000
#define SSP2CR0        (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x00))
#define SSP2CR1        (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x04))
#define SSP2DR         (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x08))
#define SSP2SR         (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x0C))
#define SSP2CPSR       (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x10))
#define SSP2IMSC       (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x14))
#define SSP2RIS        (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x18))
#define SSP2MIS        (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x1C))
#define SSP2ICR        (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x20))
#define SSP2DMACR      (*(volatile uint32_t *) (SSP2_BASE_ADDR + 0x24))

#endif /* ARM_MULTILIB_ARCH_V4 */

/* Real Time Clock */
#ifdef ARM_MULTILIB_ARCH_V4
  #define RTC_BASE_ADDR		0xE0024000
#else
  #define RTC_BASE_ADDR		0x40024000
#endif
#define RTC_ILR         (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x00))
#define RTC_CTC         (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x04))
#define RTC_CCR         (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x08))
#define RTC_CIIR        (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x0C))
#define RTC_AMR         (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x10))
#define RTC_CTIME0      (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x14))
#define RTC_CTIME1      (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x18))
#define RTC_CTIME2      (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x1C))
#define RTC_SEC         (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x20))
#define RTC_MIN         (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x24))
#define RTC_HOUR        (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x28))
#define RTC_DOM         (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x2C))
#define RTC_DOW         (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x30))
#define RTC_DOY         (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x34))
#define RTC_MONTH       (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x38))
#define RTC_YEAR        (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x3C))
#define RTC_CISS        (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x40))
#define RTC_ALSEC       (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x60))
#define RTC_ALMIN       (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x64))
#define RTC_ALHOUR      (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x68))
#define RTC_ALDOM       (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x6C))
#define RTC_ALDOW       (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x70))
#define RTC_ALDOY       (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x74))
#define RTC_ALMON       (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x78))
#define RTC_ALYEAR      (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x7C))
#define RTC_PREINT      (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x80))
#define RTC_PREFRAC     (*(volatile uint32_t *) (RTC_BASE_ADDR + 0x84))


/* A/D Converter 0 (AD0) */
#ifdef ARM_MULTILIB_ARCH_V4
  #define AD0_BASE_ADDR		0xE0034000
#else
  #define AD0_BASE_ADDR		0x40034000
#endif
#define AD0CR          (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x00))
#define AD0GDR         (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x04))
#define AD0INTEN       (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x0C))
#define AD0_DATA_START ((volatile uint32_t *) (AD0_BASE_ADDR + 0x10))
#define AD0DR0         (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x10))
#define AD0DR1         (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x14))
#define AD0DR2         (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x18))
#define AD0DR3         (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x1C))
#define AD0DR4         (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x20))
#define AD0DR5         (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x24))
#define AD0DR6         (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x28))
#define AD0DR7         (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x2C))
#define AD0STAT        (*(volatile uint32_t *) (AD0_BASE_ADDR + 0x30))


/* D/A Converter */
#ifdef ARM_MULTILIB_ARCH_V4
  #define DAC_BASE_ADDR		0xE006C000
#else
  #define DAC_BASE_ADDR		0x4008C000
#endif
#define DACR           (*(volatile uint32_t *) (DAC_BASE_ADDR + 0x00))


/* Watchdog */
#ifdef ARM_MULTILIB_ARCH_V4
  #define WDG_BASE_ADDR		0xE0000000
#else
  #define WDG_BASE_ADDR		0x40000000
#endif
#define WDMOD          (*(volatile uint32_t *) (WDG_BASE_ADDR + 0x00))
#define WDTC           (*(volatile uint32_t *) (WDG_BASE_ADDR + 0x04))
#define WDFEED         (*(volatile uint32_t *) (WDG_BASE_ADDR + 0x08))
#define WDTV           (*(volatile uint32_t *) (WDG_BASE_ADDR + 0x0C))
#define WDCLKSEL       (*(volatile uint32_t *) (WDG_BASE_ADDR + 0x10))

/* CAN CONTROLLERS AND ACCEPTANCE FILTER */
#ifdef ARM_MULTILIB_ARCH_V4
  #define CAN_ACCEPT_BASE_ADDR		0xE003C000
#else
  #define CAN_ACCEPT_BASE_ADDR		0x4003C000
#endif
#define CAN_AFMR		(*(volatile uint32_t *) (CAN_ACCEPT_BASE_ADDR + 0x00))
#define CAN_SFF_SA 		(*(volatile uint32_t *) (CAN_ACCEPT_BASE_ADDR + 0x04))
#define CAN_SFF_GRP_SA 	(*(volatile uint32_t *) (CAN_ACCEPT_BASE_ADDR + 0x08))
#define CAN_EFF_SA 		(*(volatile uint32_t *) (CAN_ACCEPT_BASE_ADDR + 0x0C))
#define CAN_EFF_GRP_SA 	(*(volatile uint32_t *) (CAN_ACCEPT_BASE_ADDR + 0x10))
#define CAN_EOT 		(*(volatile uint32_t *) (CAN_ACCEPT_BASE_ADDR + 0x14))
#define CAN_LUT_ERR_ADR (*(volatile uint32_t *) (CAN_ACCEPT_BASE_ADDR + 0x18))
#define CAN_LUT_ERR 	(*(volatile uint32_t *) (CAN_ACCEPT_BASE_ADDR + 0x1C))

#ifdef ARM_MULTILIB_ARCH_V4
  #define CAN_CENTRAL_BASE_ADDR		0xE0040000
#else
  #define CAN_CENTRAL_BASE_ADDR		0x40040000
#endif
#define CAN_TX_SR 	(*(volatile uint32_t *) (CAN_CENTRAL_BASE_ADDR + 0x00))
#define CAN_RX_SR 	(*(volatile uint32_t *) (CAN_CENTRAL_BASE_ADDR + 0x04))
#define CAN_MSR 	(*(volatile uint32_t *) (CAN_CENTRAL_BASE_ADDR + 0x08))

#ifdef ARM_MULTILIB_ARCH_V4
  #define CAN1_BASE_ADDR		0xE0044000
#else
  #define CAN1_BASE_ADDR		0x40044000
#endif
#define CAN1MOD 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x00))
#define CAN1CMR 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x04))
#define CAN1GSR 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x08))
#define CAN1ICR 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x0C))
#define CAN1IER 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x10))
#define CAN1BTR 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x14))
#define CAN1EWL 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x18))
#define CAN1SR 		(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x1C))
#define CAN1RFS 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x20))
#define CAN1RID 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x24))
#define CAN1RDA 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x28))
#define CAN1RDB 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x2C))

#define CAN1TFI1 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x30))
#define CAN1TID1 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x34))
#define CAN1TDA1 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x38))
#define CAN1TDB1 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x3C))
#define CAN1TFI2 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x40))
#define CAN1TID2 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x44))
#define CAN1TDA2 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x48))
#define CAN1TDB2 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x4C))
#define CAN1TFI3 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x50))
#define CAN1TID3 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x54))
#define CAN1TDA3 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x58))
#define CAN1TDB3 	(*(volatile uint32_t *) (CAN1_BASE_ADDR + 0x5C))

#ifdef ARM_MULTILIB_ARCH_V4
  #define CAN2_BASE_ADDR		0xE0048000
#else
  #define CAN2_BASE_ADDR		0x40048000
#endif
#define CAN2MOD 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x00))
#define CAN2CMR 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x04))
#define CAN2GSR 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x08))
#define CAN2ICR 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x0C))
#define CAN2IER 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x10))
#define CAN2BTR 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x14))
#define CAN2EWL 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x18))
#define CAN2SR 		(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x1C))
#define CAN2RFS 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x20))
#define CAN2RID 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x24))
#define CAN2RDA 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x28))
#define CAN2RDB 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x2C))

#define CAN2TFI1 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x30))
#define CAN2TID1 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x34))
#define CAN2TDA1 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x38))
#define CAN2TDB1 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x3C))
#define CAN2TFI2 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x40))
#define CAN2TID2 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x44))
#define CAN2TDA2 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x48))
#define CAN2TDB2 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x4C))
#define CAN2TFI3 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x50))
#define CAN2TID3 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x54))
#define CAN2TDA3 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x58))
#define CAN2TDB3 	(*(volatile uint32_t *) (CAN2_BASE_ADDR + 0x5C))


/* MultiMedia Card Interface(MCI) Controller */
#ifdef ARM_MULTILIB_ARCH_V4
  #define MCI_BASE_ADDR		0xE008C000
#else
  #define MCI_BASE_ADDR		0x400c0000
#endif
#define MCI_POWER      (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x00))
#define MCI_CLOCK      (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x04))
#define MCI_ARGUMENT   (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x08))
#define MCI_COMMAND    (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x0C))
#define MCI_RESP_CMD   (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x10))
#define MCI_RESP0      (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x14))
#define MCI_RESP1      (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x18))
#define MCI_RESP2      (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x1C))
#define MCI_RESP3      (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x20))
#define MCI_DATA_TMR   (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x24))
#define MCI_DATA_LEN   (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x28))
#define MCI_DATA_CTRL  (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x2C))
#define MCI_DATA_CNT   (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x30))
#define MCI_STATUS     (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x34))
#define MCI_CLEAR      (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x38))
#define MCI_MASK0      (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x3C))
#define MCI_MASK1      (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x40))
#define MCI_FIFO_CNT   (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x48))
#define MCI_FIFO       (*(volatile uint32_t *) (MCI_BASE_ADDR + 0x80))


/* I2S Interface Controller (I2S) */
#ifdef ARM_MULTILIB_ARCH_V4
  #define I2S_BASE_ADDR		0xE0088000
#else
  #define I2S_BASE_ADDR		0x400a8000
#endif
#define I2S_DAO        (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x00))
#define I2S_DAI        (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x04))
#define I2S_TX_FIFO    (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x08))
#define I2S_RX_FIFO    (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x0C))
#define I2S_STATE      (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x10))
#define I2S_DMA1       (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x14))
#define I2S_DMA2       (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x18))
#define I2S_IRQ        (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x1C))
#define I2S_TXRATE     (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x20))
#define I2S_RXRATE     (*(volatile uint32_t *) (I2S_BASE_ADDR + 0x24))


/* General-purpose DMA Controller */
#ifdef ARM_MULTILIB_ARCH_V4
  #define DMA_BASE_ADDR		0xFFE04000
#else
  #define DMA_BASE_ADDR		0x20080000
#endif
#define GPDMA_INT_STAT         (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x000))
#define GPDMA_INT_TCSTAT       (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x004))
#define GPDMA_INT_TCCLR        (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x008))
#define GPDMA_INT_ERR_STAT     (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x00C))
#define GPDMA_INT_ERR_CLR      (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x010))
#define GPDMA_RAW_INT_TCSTAT   (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x014))
#define GPDMA_RAW_INT_ERR_STAT (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x018))
#define GPDMA_ENABLED_CHNS     (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x01C))
#define GPDMA_SOFT_BREQ        (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x020))
#define GPDMA_SOFT_SREQ        (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x024))
#define GPDMA_SOFT_LBREQ       (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x028))
#define GPDMA_SOFT_LSREQ       (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x02C))
#define GPDMA_CONFIG           (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x030))
#define GPDMA_SYNC             (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x034))

/* DMA channel 0 registers */
#define GPDMA_CH0_BASE_ADDR (DMA_BASE_ADDR + 0x100)
#define GPDMA_CH0_SRC      (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x100))
#define GPDMA_CH0_DEST     (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x104))
#define GPDMA_CH0_LLI      (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x108))
#define GPDMA_CH0_CTRL     (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x10C))
#define GPDMA_CH0_CFG      (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x110))

/* DMA channel 1 registers */
#define GPDMA_CH1_BASE_ADDR (DMA_BASE_ADDR + 0x120)
#define GPDMA_CH1_SRC      (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x120))
#define GPDMA_CH1_DEST     (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x124))
#define GPDMA_CH1_LLI      (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x128))
#define GPDMA_CH1_CTRL     (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x12C))
#define GPDMA_CH1_CFG      (*(volatile uint32_t *) (DMA_BASE_ADDR + 0x130))

/* USB Controller */
#ifdef ARM_MULTILIB_ARCH_V4
  #define USB_INT_BASE_ADDR	0xE01FC1C0
  #define USB_BASE_ADDR		0xFFE0C200		/* USB Base Address */
#else
  #define USB_INT_BASE_ADDR	0x400fc1c0
  #define USB_BASE_ADDR		0x2008c200
#endif

#define USB_INT_STAT    (*(volatile uint32_t *) (USB_INT_BASE_ADDR + 0x00))

/* USB Device Interrupt Registers */
#define DEV_INT_STAT    (*(volatile uint32_t *) (USB_BASE_ADDR + 0x00))
#define DEV_INT_EN      (*(volatile uint32_t *) (USB_BASE_ADDR + 0x04))
#define DEV_INT_CLR     (*(volatile uint32_t *) (USB_BASE_ADDR + 0x08))
#define DEV_INT_SET     (*(volatile uint32_t *) (USB_BASE_ADDR + 0x0C))
#define DEV_INT_PRIO    (*(volatile uint32_t *) (USB_BASE_ADDR + 0x2C))

/* USB Device Endpoint Interrupt Registers */
#define EP_INT_STAT     (*(volatile uint32_t *) (USB_BASE_ADDR + 0x30))
#define EP_INT_EN       (*(volatile uint32_t *) (USB_BASE_ADDR + 0x34))
#define EP_INT_CLR      (*(volatile uint32_t *) (USB_BASE_ADDR + 0x38))
#define EP_INT_SET      (*(volatile uint32_t *) (USB_BASE_ADDR + 0x3C))
#define EP_INT_PRIO     (*(volatile uint32_t *) (USB_BASE_ADDR + 0x40))

/* USB Device Endpoint Realization Registers */
#define REALIZE_EP      (*(volatile uint32_t *) (USB_BASE_ADDR + 0x44))
#define EP_INDEX        (*(volatile uint32_t *) (USB_BASE_ADDR + 0x48))
#define MAXPACKET_SIZE  (*(volatile uint32_t *) (USB_BASE_ADDR + 0x4C))

/* USB Device Command Reagisters */
#define CMD_CODE        (*(volatile uint32_t *) (USB_BASE_ADDR + 0x10))
#define CMD_DATA        (*(volatile uint32_t *) (USB_BASE_ADDR + 0x14))

/* USB Device Data Transfer Registers */
#define RX_DATA         (*(volatile uint32_t *) (USB_BASE_ADDR + 0x18))
#define TX_DATA         (*(volatile uint32_t *) (USB_BASE_ADDR + 0x1C))
#define RX_PLENGTH      (*(volatile uint32_t *) (USB_BASE_ADDR + 0x20))
#define TX_PLENGTH      (*(volatile uint32_t *) (USB_BASE_ADDR + 0x24))
#define USB_CTRL        (*(volatile uint32_t *) (USB_BASE_ADDR + 0x28))

/* USB Device DMA Registers */
#define DMA_REQ_STAT        (*(volatile uint32_t *) (USB_BASE_ADDR + 0x50))
#define DMA_REQ_CLR         (*(volatile uint32_t *) (USB_BASE_ADDR + 0x54))
#define DMA_REQ_SET         (*(volatile uint32_t *) (USB_BASE_ADDR + 0x58))
#define UDCA_HEAD           (*(volatile uint32_t *) (USB_BASE_ADDR + 0x80))
#define EP_DMA_STAT         (*(volatile uint32_t *) (USB_BASE_ADDR + 0x84))
#define EP_DMA_EN           (*(volatile uint32_t *) (USB_BASE_ADDR + 0x88))
#define EP_DMA_DIS          (*(volatile uint32_t *) (USB_BASE_ADDR + 0x8C))
#define DMA_INT_STAT        (*(volatile uint32_t *) (USB_BASE_ADDR + 0x90))
#define DMA_INT_EN          (*(volatile uint32_t *) (USB_BASE_ADDR + 0x94))
#define EOT_INT_STAT        (*(volatile uint32_t *) (USB_BASE_ADDR + 0xA0))
#define EOT_INT_CLR         (*(volatile uint32_t *) (USB_BASE_ADDR + 0xA4))
#define EOT_INT_SET         (*(volatile uint32_t *) (USB_BASE_ADDR + 0xA8))
#define NDD_REQ_INT_STAT    (*(volatile uint32_t *) (USB_BASE_ADDR + 0xAC))
#define NDD_REQ_INT_CLR     (*(volatile uint32_t *) (USB_BASE_ADDR + 0xB0))
#define NDD_REQ_INT_SET     (*(volatile uint32_t *) (USB_BASE_ADDR + 0xB4))
#define SYS_ERR_INT_STAT    (*(volatile uint32_t *) (USB_BASE_ADDR + 0xB8))
#define SYS_ERR_INT_CLR     (*(volatile uint32_t *) (USB_BASE_ADDR + 0xBC))
#define SYS_ERR_INT_SET     (*(volatile uint32_t *) (USB_BASE_ADDR + 0xC0))

/* USB Host Controller */
#ifdef ARM_MULTILIB_ARCH_V4
  #define USBHC_BASE_ADDR		0xFFE0C000
#else
  #define USBHC_BASE_ADDR		0x2008c000
#endif
#define HC_REVISION         (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x00))
#define HC_CONTROL          (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x04))
#define HC_CMD_STAT         (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x08))
#define HC_INT_STAT         (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x0C))
#define HC_INT_EN           (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x10))
#define HC_INT_DIS          (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x14))
#define HC_HCCA             (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x18))
#define HC_PERIOD_CUR_ED    (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x1C))
#define HC_CTRL_HEAD_ED     (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x20))
#define HC_CTRL_CUR_ED      (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x24))
#define HC_BULK_HEAD_ED     (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x28))
#define HC_BULK_CUR_ED      (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x2C))
#define HC_DONE_HEAD        (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x30))
#define HC_FM_INTERVAL      (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x34))
#define HC_FM_REMAINING     (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x38))
#define HC_FM_NUMBER        (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x3C))
#define HC_PERIOD_START     (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x40))
#define HC_LS_THRHLD        (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x44))
#define HC_RH_DESCA         (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x48))
#define HC_RH_DESCB         (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x4C))
#define HC_RH_STAT          (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x50))
#define HC_RH_PORT_STAT1    (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x54))
#define HC_RH_PORT_STAT2    (*(volatile uint32_t *) (USBHC_BASE_ADDR + 0x58))

/* USB OTG Controller */
#ifdef ARM_MULTILIB_ARCH_V4
  #define USBOTG_BASE_ADDR	0xFFE0C100
#else
  #define USBOTG_BASE_ADDR	0x2008c100
#endif
#define OTG_INT_STAT        (*(volatile uint32_t *) (USBOTG_BASE_ADDR + 0x00))
#define OTG_INT_EN          (*(volatile uint32_t *) (USBOTG_BASE_ADDR + 0x04))
#define OTG_INT_SET         (*(volatile uint32_t *) (USBOTG_BASE_ADDR + 0x08))
#define OTG_INT_CLR         (*(volatile uint32_t *) (USBOTG_BASE_ADDR + 0x0C))
#define OTG_STAT_CTRL       (*(volatile uint32_t *) (USBOTG_BASE_ADDR + 0x10))
#define OTG_TIMER           (*(volatile uint32_t *) (USBOTG_BASE_ADDR + 0x14))

#ifdef ARM_MULTILIB_ARCH_V4
  #define USBOTG_I2C_BASE_ADDR	0xFFE0C300
#else
  #define USBOTG_I2C_BASE_ADDR	0x2008c300
#endif
#define OTG_I2C_RX          (*(volatile uint32_t *) (USBOTG_I2C_BASE_ADDR + 0x00))
#define OTG_I2C_TX          (*(volatile uint32_t *) (USBOTG_I2C_BASE_ADDR + 0x00))
#define OTG_I2C_STS         (*(volatile uint32_t *) (USBOTG_I2C_BASE_ADDR + 0x04))
#define OTG_I2C_CTL         (*(volatile uint32_t *) (USBOTG_I2C_BASE_ADDR + 0x08))
#define OTG_I2C_CLKHI       (*(volatile uint32_t *) (USBOTG_I2C_BASE_ADDR + 0x0C))
#define OTG_I2C_CLKLO       (*(volatile uint32_t *) (USBOTG_I2C_BASE_ADDR + 0x10))

#ifdef ARM_MULTILIB_ARCH_V4
  #define USBOTG_CLK_BASE_ADDR	0xFFE0CFF0
#else
  #define USBOTG_CLK_BASE_ADDR	0x2008cff0
#endif
#define OTG_CLK_CTRL        (*(volatile uint32_t *) (USBOTG_CLK_BASE_ADDR + 0x04))
#define OTG_CLK_STAT        (*(volatile uint32_t *) (USBOTG_CLK_BASE_ADDR + 0x08))

/* Ethernet MAC (32 bit data bus) -- all registers are RW unless indicated in parentheses */
#ifdef ARM_MULTILIB_ARCH_V4
  #define MAC_BASE_ADDR		0xFFE00000 /* AHB Peripheral # 0 */
#else
  #define MAC_BASE_ADDR		0x20084000
#endif
#define MAC_MAC1            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x000)) /* MAC config reg 1 */
#define MAC_MAC2            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x004)) /* MAC config reg 2 */
#define MAC_IPGT            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x008)) /* b2b InterPacketGap reg */
#define MAC_IPGR            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x00C)) /* non b2b InterPacketGap reg */
#define MAC_CLRT            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x010)) /* CoLlision window/ReTry reg */
#define MAC_MAXF            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x014)) /* MAXimum Frame reg */
#define MAC_SUPP            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x018)) /* PHY SUPPort reg */
#define MAC_TEST            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x01C)) /* TEST reg */
#define MAC_MCFG            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x020)) /* MII Mgmt ConFiG reg */
#define MAC_MCMD            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x024)) /* MII Mgmt CoMmanD reg */
#define MAC_MADR            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x028)) /* MII Mgmt ADdRess reg */
#define MAC_MWTD            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x02C)) /* MII Mgmt WriTe Data reg (WO) */
#define MAC_MRDD            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x030)) /* MII Mgmt ReaD Data reg (RO) */
#define MAC_MIND            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x034)) /* MII Mgmt INDicators reg (RO) */

#define MAC_SA0             (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x040)) /* Station Address 0 reg */
#define MAC_SA1             (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x044)) /* Station Address 1 reg */
#define MAC_SA2             (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x048)) /* Station Address 2 reg */

#define MAC_COMMAND         (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x100)) /* Command reg */
#define MAC_STATUS          (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x104)) /* Status reg (RO) */
#define MAC_RXDESCRIPTOR    (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x108)) /* Rx descriptor base address reg */
#define MAC_RXSTATUS        (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x10C)) /* Rx status base address reg */
#define MAC_RXDESCRIPTORNUM (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x110)) /* Rx number of descriptors reg */
#define MAC_RXPRODUCEINDEX  (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x114)) /* Rx produce index reg (RO) */
#define MAC_RXCONSUMEINDEX  (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x118)) /* Rx consume index reg */
#define MAC_TXDESCRIPTOR    (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x11C)) /* Tx descriptor base address reg */
#define MAC_TXSTATUS        (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x120)) /* Tx status base address reg */
#define MAC_TXDESCRIPTORNUM (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x124)) /* Tx number of descriptors reg */
#define MAC_TXPRODUCEINDEX  (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x128)) /* Tx produce index reg */
#define MAC_TXCONSUMEINDEX  (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x12C)) /* Tx consume index reg (RO) */

#define MAC_TSV0            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x158)) /* Tx status vector 0 reg (RO) */
#define MAC_TSV1            (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x15C)) /* Tx status vector 1 reg (RO) */
#define MAC_RSV             (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x160)) /* Rx status vector reg (RO) */

#define MAC_FLOWCONTROLCNT  (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x170)) /* Flow control counter reg */
#define MAC_FLOWCONTROLSTS  (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x174)) /* Flow control status reg */

#define MAC_RXFILTERCTRL    (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x200)) /* Rx filter ctrl reg */
#define MAC_RXFILTERWOLSTS  (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x204)) /* Rx filter WoL status reg (RO) */
#define MAC_RXFILTERWOLCLR  (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x208)) /* Rx filter WoL clear reg (WO) */

#define MAC_HASHFILTERL     (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x210)) /* Hash filter LSBs reg */
#define MAC_HASHFILTERH     (*(volatile uint32_t *) (MAC_BASE_ADDR + 0x214)) /* Hash filter MSBs reg */

#define MAC_INTSTATUS       (*(volatile uint32_t *) (MAC_BASE_ADDR + 0xFE0)) /* Interrupt status reg (RO) */
#define MAC_INTENABLE       (*(volatile uint32_t *) (MAC_BASE_ADDR + 0xFE4)) /* Interrupt enable reg  */
#define MAC_INTCLEAR        (*(volatile uint32_t *) (MAC_BASE_ADDR + 0xFE8)) /* Interrupt clear reg (WO) */
#define MAC_INTSET          (*(volatile uint32_t *) (MAC_BASE_ADDR + 0xFEC)) /* Interrupt set reg (WO) */

#define MAC_POWERDOWN       (*(volatile uint32_t *) (MAC_BASE_ADDR + 0xFF4)) /* Power-down reg */
#define MAC_MODULEID        (*(volatile uint32_t *) (MAC_BASE_ADDR + 0xFFC)) /* Module ID reg (RO) */

/* LCD Controller */

#ifdef ARM_MULTILIB_ARCH_V4
  #define LCD_BASE_ADDR 0xFFE10000
#else
  #define LCD_BASE_ADDR 0x20088000
#endif
#define LCD_CFG       (*(volatile uint32_t *) 0xE01FC1B8)
#define LCD_TIMH      (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x000))
#define LCD_TIMV      (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x004))
#define LCD_POL       (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x008))
#define LCD_LE        (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x00C))
#define LCD_UPBASE    (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x010))
#define LCD_LPBASE    (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x014))
#define LCD_CTRL      (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x018))
#define LCD_INTMSK    (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x01C))
#define LCD_INTRAW    (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x020))
#define LCD_INTSTAT   (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x024))
#define LCD_INTCLR    (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x028))
#define LCD_UPCURR    (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x02C))
#define LCD_LPCURR    (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x030))
#define LCD_PAL_ADDR  (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x200))
#define CRSR_IMG      (*(volatile uint32_t *) (LCD_BASE_ADDR + 0x800))
#define CRSR_CTLR     (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC00))
#define CRSR_CFG      (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC04))
#define CRSR_PAL0     (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC08))
#define CRSR_PAL1     (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC0C))
#define CRSR_XY       (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC10))
#define CRSR_CLIP     (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC14))
#define CRSR_INTMSK   (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC20))
#define CRSR_INTCLR   (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC24))
#define CRSR_INTRAW   (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC28))
#define CRSR_INTSTAT  (*(volatile uint32_t *) (LCD_BASE_ADDR + 0xC2C))

/* Register Fields */

#define GET_FIELD( val, mask, shift) \
  (((val) & (mask)) >> (shift))

#define SET_FIELD( val, field, mask, shift) \
  (((val) & ~(mask)) | (((field) << (shift)) & (mask)))

/* CLKSRCSEL */

#define CLKSRCSEL_CLKSRC_MASK 0x00000003U

#define GET_CLKSRCSEL_CLKSRC(reg) \
  GET_FIELD(reg, CLKSRCSEL_CLKSRC_MASK, 0)

#define SET_CLKSRCSEL_CLKSRC(reg, val) \
  SET_FIELD(reg, val, CLKSRCSEL_CLKSRC_MASK, 0)

/* PLLCON */

#define PLLCON_PLLE 0x00000001U

#define PLLCON_PLLC 0x00000002U

/* PLLCFG */

#define PLLCFG_MSEL_MASK 0x00007fffU

#define GET_PLLCFG_MSEL(reg) \
  GET_FIELD(reg, PLLCFG_MSEL_MASK, 0)

#define SET_PLLCFG_MSEL(reg, val) \
  SET_FIELD(reg, val, PLLCFG_MSEL_MASK, 0)

#define PLLCFG_NSEL_MASK 0x00ff0000U

#define GET_PLLCFG_NSEL(reg) \
  GET_FIELD(reg, PLLCFG_NSEL_MASK, 16)

#define SET_PLLCFG_NSEL(reg, val) \
  SET_FIELD(reg, val, PLLCFG_NSEL_MASK, 16)

/* PLLSTAT */

#define PLLSTAT_MSEL_MASK 0x00007fffU

#define GET_PLLSTAT_MSEL(reg) \
  GET_FIELD(reg, PLLSTAT_MSEL_MASK, 0)

#define SET_PLLSTAT_MSEL(reg, val) \
  SET_FIELD(reg, val, PLLSTAT_MSEL_MASK, 0)

#define PLLSTAT_NSEL_MASK 0x00ff0000U

#define GET_PLLSTAT_NSEL(reg) \
  GET_FIELD(reg, PLLSTAT_NSEL_MASK, 16)

#define SET_PLLSTAT_NSEL(reg, val) \
  SET_FIELD(reg, val, PLLSTAT_NSEL_MASK, 16)

#define PLLSTAT_PLLE 0x01000000U

#define PLLSTAT_PLLC 0x02000000U

#define PLLSTAT_PLOCK 0x04000000U

/* CCLKCFG */

#define CCLKCFG_CCLKSEL_MASK 0x000000ffU

#define GET_CCLKCFG_CCLKSEL(reg) \
  GET_FIELD(reg, CCLKCFG_CCLKSEL_MASK, 0)

#define SET_CCLKCFG_CCLKSEL(reg, val) \
  SET_FIELD(reg, val, CCLKCFG_CCLKSEL_MASK, 0)

/* MEMMAP */

#define MEMMAP_MAP_MASK 0x00000003U

#define GET_MEMMAP_MAP(reg) \
  GET_FIELD(reg, MEMMAP_MAP_MASK, 0)

#define SET_MEMMAP_MAP(reg, val) \
  SET_FIELD(reg, val, MEMMAP_MAP_MASK, 0)

/* TIR */

#define TIR_MR0 0x00000001U

#define TIR_MR1 0x00000002U

#define TIR_MR2 0x00000004U

#define TIR_MR3 0x00000008U

#define TIR_CR0 0x00000010U

#define TIR_CR1 0x00000020U

#define TIR_CR2 0x00000040U

#define TIR_CR3 0x00000080U

/* TCR */

#define TCR_EN 0x00000001U

#define TCR_RST 0x00000002U

/* TMCR */

#define TMCR_MR0I 0x00000001U

#define TMCR_MR0R 0x00000002U

#define TMCR_MR0S 0x00000004U

#define TMCR_MR1I 0x00000008U

#define TMCR_MR1R 0x00000010U

#define TMCR_MR1S 0x00000020U

#define TMCR_MR2I 0x00000040U

#define TMCR_MR2R 0x00000080U

#define TMCR_MR2S 0x00000100U

#define TMCR_MR3I 0x00000200U

#define TMCR_MR3R 0x00000400U

#define TMCR_MR3S 0x00000800U

/* PCLKSEL0 */

#define PCLKSEL0_PCLK_WDT_MASK 0x00000003U

#define GET_PCLKSEL0_PCLK_WDT(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_WDT_MASK, 0)

#define SET_PCLKSEL0_PCLK_WDT(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_WDT_MASK, 0)

#define PCLKSEL0_PCLK_TIMER0_MASK 0x0000000cU

#define GET_PCLKSEL0_PCLK_TIMER0(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_TIMER0_MASK, 2)

#define SET_PCLKSEL0_PCLK_TIMER0(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_TIMER0_MASK, 2)

#define PCLKSEL0_PCLK_TIMER1_MASK 0x00000030U

#define GET_PCLKSEL0_PCLK_TIMER1(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_TIMER1_MASK, 4)

#define SET_PCLKSEL0_PCLK_TIMER1(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_TIMER1_MASK, 4)

#define PCLKSEL0_PCLK_UART0_MASK 0x000000c0U

#define GET_PCLKSEL0_PCLK_UART0(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_UART0_MASK, 6)

#define SET_PCLKSEL0_PCLK_UART0(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_UART0_MASK, 6)

#define PCLKSEL0_PCLK_UART1_MASK 0x00000300U

#define GET_PCLKSEL0_PCLK_UART1(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_UART1_MASK, 8)

#define SET_PCLKSEL0_PCLK_UART1(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_UART1_MASK, 8)

#define PCLKSEL0_PCLK_PWM0_MASK 0x00000c00U

#define GET_PCLKSEL0_PCLK_PWM0(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_PWM0_MASK, 10)

#define SET_PCLKSEL0_PCLK_PWM0(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_PWM0_MASK, 10)

#define PCLKSEL0_PCLK_PWM1_MASK 0x00003000U

#define GET_PCLKSEL0_PCLK_PWM1(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_PWM1_MASK, 12)

#define SET_PCLKSEL0_PCLK_PWM1(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_PWM1_MASK, 12)

#define PCLKSEL0_PCLK_I2C0_MASK 0x0000c000U

#define GET_PCLKSEL0_PCLK_I2C0(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_I2C0_MASK, 14)

#define SET_PCLKSEL0_PCLK_I2C0(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_I2C0_MASK, 14)

#define PCLKSEL0_PCLK_SPI_MASK 0x00030000U

#define GET_PCLKSEL0_PCLK_SPI(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_SPI_MASK, 16)

#define SET_PCLKSEL0_PCLK_SPI(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_SPI_MASK, 16)

#define PCLKSEL0_PCLK_RTC_MASK 0x000c0000U

#define GET_PCLKSEL0_PCLK_RTC(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_RTC_MASK, 18)

#define SET_PCLKSEL0_PCLK_RTC(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_RTC_MASK, 18)

#define PCLKSEL0_PCLK_SSP1_MASK 0x00300000U

#define GET_PCLKSEL0_PCLK_SSP1(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_SSP1_MASK, 20)

#define SET_PCLKSEL0_PCLK_SSP1(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_SSP1_MASK, 20)

#define PCLKSEL0_PCLK_DAC_MASK 0x00c00000U

#define GET_PCLKSEL0_PCLK_DAC(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_DAC_MASK, 22)

#define SET_PCLKSEL0_PCLK_DAC(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_DAC_MASK, 22)

#define PCLKSEL0_PCLK_ADC_MASK 0x03000000U

#define GET_PCLKSEL0_PCLK_ADC(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_ADC_MASK, 24)

#define SET_PCLKSEL0_PCLK_ADC(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_ADC_MASK, 24)

#define PCLKSEL0_PCLK_CAN1_MASK 0x0c000000U

#define GET_PCLKSEL0_PCLK_CAN1(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_CAN1_MASK, 26)

#define SET_PCLKSEL0_PCLK_CAN1(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_CAN1_MASK, 26)

#define PCLKSEL0_PCLK_CAN2_MASK 0x30000000U

#define GET_PCLKSEL0_PCLK_CAN2(reg) \
  GET_FIELD(reg, PCLKSEL0_PCLK_CAN2_MASK, 28)

#define SET_PCLKSEL0_PCLK_CAN2(reg, val) \
  SET_FIELD(reg, val, PCLKSEL0_PCLK_CAN2_MASK, 28)

/* PCLKSEL1 */

#define PCLKSEL1_PCLK_BAT_RAM_MASK 0x00000003U

#define GET_PCLKSEL1_PCLK_BAT_RAM(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_BAT_RAM_MASK, 0)

#define SET_PCLKSEL1_PCLK_BAT_RAM(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_BAT_RAM_MASK, 0)

#define PCLKSEL1_PCLK_GPIO_MASK 0x0000000cU

#define GET_PCLKSEL1_PCLK_GPIO(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_GPIO_MASK, 2)

#define SET_PCLKSEL1_PCLK_GPIO(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_GPIO_MASK, 2)

#define PCLKSEL1_PCLK_PCB_MASK 0x00000030U

#define GET_PCLKSEL1_PCLK_PCB(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_PCB_MASK, 4)

#define SET_PCLKSEL1_PCLK_PCB(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_PCB_MASK, 4)

#define PCLKSEL1_PCLK_I2C1_MASK 0x000000c0U

#define GET_PCLKSEL1_PCLK_I2C1(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_I2C1_MASK, 6)

#define SET_PCLKSEL1_PCLK_I2C1(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_I2C1_MASK, 6)

#define PCLKSEL1_PCLK_SSP0_MASK 0x00000c00U

#define GET_PCLKSEL1_PCLK_SSP0(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_SSP0_MASK, 10)

#define SET_PCLKSEL1_PCLK_SSP0(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_SSP0_MASK, 10)

#define PCLKSEL1_PCLK_TIMER2_MASK 0x00003000U

#define GET_PCLKSEL1_PCLK_TIMER2(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_TIMER2_MASK, 12)

#define SET_PCLKSEL1_PCLK_TIMER2(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_TIMER2_MASK, 12)

#define PCLKSEL1_PCLK_TIMER3_MASK 0x0000c000U

#define GET_PCLKSEL1_PCLK_TIMER3(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_TIMER3_MASK, 14)

#define SET_PCLKSEL1_PCLK_TIMER3(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_TIMER3_MASK, 14)

#define PCLKSEL1_PCLK_UART2_MASK 0x00030000U

#define GET_PCLKSEL1_PCLK_UART2(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_UART2_MASK, 16)

#define SET_PCLKSEL1_PCLK_UART2(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_UART2_MASK, 16)

#define PCLKSEL1_PCLK_UART3_MASK 0x000c0000U

#define GET_PCLKSEL1_PCLK_UART3(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_UART3_MASK, 18)

#define SET_PCLKSEL1_PCLK_UART3(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_UART3_MASK, 18)

#define PCLKSEL1_PCLK_I2C2_MASK 0x00300000U

#define GET_PCLKSEL1_PCLK_I2C2(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_I2C2_MASK, 20)

#define SET_PCLKSEL1_PCLK_I2C2(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_I2C2_MASK, 20)

#define PCLKSEL1_PCLK_I2S_MASK 0x00c00000U

#define GET_PCLKSEL1_PCLK_I2S(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_I2S_MASK, 22)

#define SET_PCLKSEL1_PCLK_I2S(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_I2S_MASK, 22)

#define PCLKSEL1_PCLK_MCI_MASK 0x03000000U

#define GET_PCLKSEL1_PCLK_MCI(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_MCI_MASK, 24)

#define SET_PCLKSEL1_PCLK_MCI(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_MCI_MASK, 24)

#define PCLKSEL1_PCLK_SYSCON_MASK 0x30000000U

#define GET_PCLKSEL1_PCLK_SYSCON(reg) \
  GET_FIELD(reg, PCLKSEL1_PCLK_SYSCON_MASK, 28)

#define SET_PCLKSEL1_PCLK_SYSCON(reg, val) \
  SET_FIELD(reg, val, PCLKSEL1_PCLK_SYSCON_MASK, 28)

/* RTC_ILR */

#define RTC_ILR_RTCCIF 0x00000001U

#define RTC_ILR_RTCALF 0x00000002U

#define RTC_ILR_RTSSF 0x00000004U

/* RTC_CCR */

#define RTC_CCR_CLKEN 0x00000001U

#define RTC_CCR_CTCRST 0x00000002U

#define RTC_CCR_CLKSRC 0x00000010U

/* SSP */

typedef struct {
  uint32_t cr0;
  uint32_t cr1;
  uint32_t dr;
  uint32_t sr;
  uint32_t cpsr;
  uint32_t imsc;
  uint32_t ris;
  uint32_t mis;
  uint32_t icr;
  uint32_t dmacr;
} lpc24xx_ssp;

/* SSP_CR0 */

#define SSP_CR0_DSS_MASK 0x0000000fU

#define GET_SSP_CR0_DSS(reg) \
  GET_FIELD(reg, SSP_CR0_DSS_MASK, 0)

#define SET_SSP_CR0_DSS(reg, val) \
  SET_FIELD(reg, val, SSP_CR0_DSS_MASK, 0)

#define SSP_CR0_FRF_MASK 0x00000030U

#define GET_SSP_CR0_FRF(reg) \
  GET_FIELD(reg, SSP_CR0_FRF_MASK, 4)

#define SET_SSP_CR0_FRF(reg, val) \
  SET_FIELD(reg, val, SSP_CR0_FRF_MASK, 4)

#define SSP_CR0_CPOL 0x00000040U

#define SSP_CR0_CPHA 0x00000080U

#define SSP_CR0_SCR_MASK 0x0000ff00U

#define GET_SSP_CR0_SCR(reg) \
  GET_FIELD(reg, SSP_CR0_SCR_MASK, 8)

#define SET_SSP_CR0_SCR(reg, val) \
  SET_FIELD(reg, val, SSP_CR0_SCR_MASK, 8)

/* SSP_CR1 */

#define SSP_CR1_LBM 0x00000001U

#define SSP_CR1_SSE 0x00000002U

#define SSP_CR1_MS 0x00000004U

#define SSP_CR1_SOD 0x00000008U

/* SSP_SR */

#define SSP_SR_TFE 0x00000001U

#define SSP_SR_TNF 0x00000002U

#define SSP_SR_RNE 0x00000004U

#define SSP_SR_RFF 0x00000008U

#define SSP_SR_BSY 0x00000010U

/* SSP_IMSC */

#define SSP_IMSC_RORIM 0x00000001U

#define SSP_IMSC_RTIM 0x00000002U

#define SSP_IMSC_RXIM 0x00000004U

#define SSP_IMSC_TXIM 0x00000008U

/* SSP_RIS */

#define SSP_RIS_RORRIS 0x00000001U

#define SSP_RIS_RTRIS 0x00000002U

#define SSP_RIS_RXRIS 0x00000004U

#define SSP_RIS_TXRIS 0x00000008U

/* SSP_MIS */

#define SSP_MIS_RORRIS 0x00000001U

#define SSP_MIS_RTRIS 0x00000002U

#define SSP_MIS_RXRIS 0x00000004U

#define SSP_MIS_TXRIS 0x00000008U

/* SSP_ICR */

#define SSP_ICR_RORRIS 0x00000001U

#define SSP_ICR_RTRIS 0x00000002U

#define SSP_ICR_RXRIS 0x00000004U

#define SSP_ICR_TXRIS 0x00000008U

/* SSP_DMACR */

#define SSP_DMACR_RXDMAE 0x00000001U

#define SSP_DMACR_TXDMAE 0x00000002U

/* GPDMA */

typedef struct {
  uint32_t src;
  uint32_t dest;
  uint32_t lli;
  uint32_t ctrl;
} lpc24xx_dma_descriptor;

typedef struct {
  lpc24xx_dma_descriptor desc;
  uint32_t cfg;
} lpc24xx_dma_channel;

#define GPDMA_CH_NUMBER 2

#define GPDMA_STATUS_CH_0 0x00000001U

#define GPDMA_STATUS_CH_1 0x00000002U

#define GPDMA_CH_BASE_ADDR(i) \
  ((volatile lpc24xx_dma_channel *) \
    ((i) ? GPDMA_CH1_BASE_ADDR : GPDMA_CH0_BASE_ADDR))

/* GPDMA_CONFIG */

#define GPDMA_CONFIG_EN 0x00000001U

#define GPDMA_CONFIG_MODE 0x00000002U

/* GPDMA_ENABLED_CHNS */

#define GPDMA_ENABLED_CHNS_CH0 0x00000001U

#define GPDMA_ENABLED_CHNS_CH1 0x00000002U

/* GPDMA_CH_CTRL */

#define GPDMA_CH_CTRL_TSZ_MASK 0x00000fffU

#define GET_GPDMA_CH_CTRL_TSZ(reg) \
  GET_FIELD(reg, GPDMA_CH_CTRL_TSZ_MASK, 0)

#define SET_GPDMA_CH_CTRL_TSZ(reg, val) \
  SET_FIELD(reg, val, GPDMA_CH_CTRL_TSZ_MASK, 0)

#define GPDMA_CH_CTRL_TSZ_MAX 0x00000fffU

#define GPDMA_CH_CTRL_SBSZ_MASK 0x00007000U

#define GET_GPDMA_CH_CTRL_SBSZ(reg) \
  GET_FIELD(reg, GPDMA_CH_CTRL_SBSZ_MASK, 12)

#define SET_GPDMA_CH_CTRL_SBSZ(reg, val) \
  SET_FIELD(reg, val, GPDMA_CH_CTRL_SBSZ_MASK, 12)

#define GPDMA_CH_CTRL_DBSZ_MASK 0x00038000U

#define GET_GPDMA_CH_CTRL_DBSZ(reg) \
  GET_FIELD(reg, GPDMA_CH_CTRL_DBSZ_MASK, 15)

#define SET_GPDMA_CH_CTRL_DBSZ(reg, val) \
  SET_FIELD(reg, val, GPDMA_CH_CTRL_DBSZ_MASK, 15)

#define GPDMA_CH_CTRL_BSZ_1 0x00000000U

#define GPDMA_CH_CTRL_BSZ_4 0x00000001U

#define GPDMA_CH_CTRL_BSZ_8 0x00000002U

#define GPDMA_CH_CTRL_BSZ_16 0x00000003U

#define GPDMA_CH_CTRL_BSZ_32 0x00000004U

#define GPDMA_CH_CTRL_BSZ_64 0x00000005U

#define GPDMA_CH_CTRL_BSZ_128 0x00000006U

#define GPDMA_CH_CTRL_BSZ_256 0x00000007U

#define GPDMA_CH_CTRL_SW_MASK 0x001c0000U

#define GET_GPDMA_CH_CTRL_SW(reg) \
  GET_FIELD(reg, GPDMA_CH_CTRL_SW_MASK, 18)

#define SET_GPDMA_CH_CTRL_SW(reg, val) \
  SET_FIELD(reg, val, GPDMA_CH_CTRL_SW_MASK, 18)

#define GPDMA_CH_CTRL_DW_MASK 0x00e00000U

#define GET_GPDMA_CH_CTRL_DW(reg) \
  GET_FIELD(reg, GPDMA_CH_CTRL_DW_MASK, 21)

#define SET_GPDMA_CH_CTRL_DW(reg, val) \
  SET_FIELD(reg, val, GPDMA_CH_CTRL_DW_MASK, 21)

#define GPDMA_CH_CTRL_W_8 0x00000000U

#define GPDMA_CH_CTRL_W_16 0x00000001U

#define GPDMA_CH_CTRL_W_32 0x00000002U

#define GPDMA_CH_CTRL_SI 0x04000000U

#define GPDMA_CH_CTRL_DI 0x08000000U

#define GPDMA_CH_CTRL_PROT_MASK 0x70000000U

#define GET_GPDMA_CH_CTRL_PROT(reg) \
  GET_FIELD(reg, GPDMA_CH_CTRL_PROT_MASK, 28)

#define SET_GPDMA_CH_CTRL_PROT(reg, val) \
  SET_FIELD(reg, val, GPDMA_CH_CTRL_PROT_MASK, 28)

#define GPDMA_CH_CTRL_ITC 0x80000000U

/* GPDMA_CH_CFG */

#define GPDMA_CH_CFG_EN 0x00000001U

#define GPDMA_CH_CFG_SRCPER_MASK 0x0000001eU

#define GET_GPDMA_CH_CFG_SRCPER(reg) \
  GET_FIELD(reg, GPDMA_CH_CFG_SRCPER_MASK, 1)

#define SET_GPDMA_CH_CFG_SRCPER(reg, val) \
  SET_FIELD(reg, val, GPDMA_CH_CFG_SRCPER_MASK, 1)

#define GPDMA_CH_CFG_DESTPER_MASK 0x000003c0U

#define GET_GPDMA_CH_CFG_DESTPER(reg) \
  GET_FIELD(reg, GPDMA_CH_CFG_DESTPER_MASK, 6)

#define SET_GPDMA_CH_CFG_DESTPER(reg, val) \
  SET_FIELD(reg, val, GPDMA_CH_CFG_DESTPER_MASK, 6)

#define GPDMA_CH_CFG_PER_SSP0_TX 0x00000000U

#define GPDMA_CH_CFG_PER_SSP0_RX 0x00000001U

#define GPDMA_CH_CFG_PER_SSP1_TX 0x00000002U

#define GPDMA_CH_CFG_PER_SSP1_RX 0x00000003U

#define GPDMA_CH_CFG_PER_SD_MMC 0x00000004U

#define GPDMA_CH_CFG_PER_I2S_CH0 0x00000005U

#define GPDMA_CH_CFG_PER_I2S_CH1 0x00000006U

#define GPDMA_CH_CFG_FLOW_MASK 0x00003800U

#define GET_GPDMA_CH_CFG_FLOW(reg) \
  GET_FIELD(reg, GPDMA_CH_CFG_FLOW_MASK, 11)

#define SET_GPDMA_CH_CFG_FLOW(reg, val) \
  SET_FIELD(reg, val, GPDMA_CH_CFG_FLOW_MASK, 11)

#define GPDMA_CH_CFG_FLOW_MEM_TO_MEM_DMA 0x00000000U

#define GPDMA_CH_CFG_FLOW_MEM_TO_PER_DMA 0x00000001U

#define GPDMA_CH_CFG_FLOW_PER_TO_MEM_DMA 0x00000002U

#define GPDMA_CH_CFG_FLOW_PER_TO_PER_DMA 0x00000003U

#define GPDMA_CH_CFG_FLOW_PER_TO_PER_DEST 0x00000004U

#define GPDMA_CH_CFG_FLOW_MEM_TO_PER_PER 0x00000005U

#define GPDMA_CH_CFG_FLOW_PER_TO_MEM_PER 0x00000006U

#define GPDMA_CH_CFG_FLOW_PER_TO_PER_SRC 0x00000007U

#define GPDMA_CH_CFG_IE 0x00004000U

#define GPDMA_CH_CFG_ITC 0x00008000U

#define GPDMA_CH_CFG_LOCK 0x00010000U

#define GPDMA_CH_CFG_ACTIVE 0x00020000U

#define GPDMA_CH_CFG_HALT 0x00040000U

/* AHBCFG */

#define AHBCFG_SCHEDULER_UNIFORM 0x00000001U

#define AHBCFG_BREAK_BURST_MASK 0x00000006U

#define GET_AHBCFG_BREAK_BURST(reg) \
  GET_FIELD(reg, AHBCFG_BREAK_BURST_MASK, 1)

#define SET_AHBCFG_BREAK_BURST(reg, val) \
  SET_FIELD(reg, val, AHBCFG_BREAK_BURST_MASK, 1)

#define AHBCFG_QUANTUM_BUS_CYCLE 0x00000008U

#define AHBCFG_QUANTUM_SIZE_MASK 0x000000f0U

#define GET_AHBCFG_QUANTUM_SIZE(reg) \
  GET_FIELD(reg, AHBCFG_QUANTUM_SIZE_MASK, 4)

#define SET_AHBCFG_QUANTUM_SIZE(reg, val) \
  SET_FIELD(reg, val, AHBCFG_QUANTUM_SIZE_MASK, 4)

#define AHBCFG_DEFAULT_MASTER_MASK 0x00000700U

#define GET_AHBCFG_DEFAULT_MASTER(reg) \
  GET_FIELD(reg, AHBCFG_DEFAULT_MASTER_MASK, 8)

#define SET_AHBCFG_DEFAULT_MASTER(reg, val) \
  SET_FIELD(reg, val, AHBCFG_DEFAULT_MASTER_MASK, 8)

#define AHBCFG_EP1_MASK 0x00007000U

#define GET_AHBCFG_EP1(reg) \
  GET_FIELD(reg, AHBCFG_EP1_MASK, 12)

#define SET_AHBCFG_EP1(reg, val) \
  SET_FIELD(reg, val, AHBCFG_EP1_MASK, 12)

#define AHBCFG_EP2_MASK 0x00070000U

#define GET_AHBCFG_EP2(reg) \
  GET_FIELD(reg, AHBCFG_EP2_MASK, 16)

#define SET_AHBCFG_EP2(reg, val) \
  SET_FIELD(reg, val, AHBCFG_EP2_MASK, 16)

#define AHBCFG_EP3_MASK 0x00700000U

#define GET_AHBCFG_EP3(reg) \
  GET_FIELD(reg, AHBCFG_EP3_MASK, 20)

#define SET_AHBCFG_EP3(reg, val) \
  SET_FIELD(reg, val, AHBCFG_EP3_MASK, 20)

#define AHBCFG_EP4_MASK 0x07000000U

#define GET_AHBCFG_EP4(reg) \
  GET_FIELD(reg, AHBCFG_EP4_MASK, 24)

#define SET_AHBCFG_EP4(reg, val) \
  SET_FIELD(reg, val, AHBCFG_EP4_MASK, 24)

#define AHBCFG_EP5_MASK 0x70000000U

#define GET_AHBCFG_EP5(reg) \
  GET_FIELD(reg, AHBCFG_EP5_MASK, 28)

#define SET_AHBCFG_EP5(reg, val) \
  SET_FIELD(reg, val, AHBCFG_EP5_MASK, 28)

/* I2C */

typedef struct {
  uint32_t conset;
  uint32_t stat;
  uint32_t dat;
  uint32_t adr;
  uint32_t sclh;
  uint32_t scll;
  uint32_t conclr;
} lpc24xx_i2c;

#define LPC24XX_I2C_AA (1U << 2U)

#define LPC24XX_I2C_SI (1U << 3U)

#define LPC24XX_I2C_STO (1U << 4U)

#define LPC24XX_I2C_STA (1U << 5U)

#define LPC24XX_I2C_EN (1U << 6U)

/* IO */

#ifdef ARM_MULTILIB_ARCH_V4
  #define LPC24XX_PINSEL ((volatile uint32_t *) &PINSEL0)
  #define LPC24XX_PINMODE ((volatile uint32_t *) &PINMODE0)
#else
  #define IOCON_FUNC(val) BSP_FLD32(val, 0, 2)
  #define IOCON_FUNC_GET(reg) BSP_FLD32GET(reg, 0, 2)
  #define IOCON_FUNC_SET(reg, val) BSP_FLD32SET(reg, val, 0, 2)
  #define IOCON_MODE(val) BSP_FLD32(val, 3, 4)
  #define IOCON_MODE_GET(reg) BSP_FLD32GET(reg, 3, 4)
  #define IOCON_MODE_SET(reg, val) BSP_FLD32SET(reg, val, 3, 4)
  #define IOCON_HYS BSP_BIT32(5)
  #define IOCON_INV BSP_BIT32(6)
  #define IOCON_ADMODE BSP_BIT32(7)
  #define IOCON_FILTER BSP_BIT32(8)
  #define IOCON_HS BSP_BIT32(8)
  #define IOCON_SLEW BSP_BIT32(9)
  #define IOCON_HIDRIVE BSP_BIT32(9)
  #define IOCON_OD BSP_BIT32(10)
  #define IOCON_DACEN BSP_BIT32(16)
  #define LPC17XX_IOCON ((volatile uint32_t *) PINSEL_BASE_ADDR)
#endif

typedef struct {
  uint32_t dir;
  uint32_t reserved [3];
  uint32_t mask;
  uint32_t pin;
  uint32_t set;
  uint32_t clr;
} lpc24xx_fio;

#define LPC24XX_FIO ((volatile lpc24xx_fio *) FIO_BASE_ADDR)

#ifdef ARM_MULTILIB_ARCH_V4

/* PCONP */

#define PCONP_GPDMA (1U << 29)
#define PCONP_ETHERNET (1U << 30)
#define PCONP_USB (1U << 31)

#endif /* ARM_MULTILIB_ARCH_V4 */

/* I2S */

static volatile lpc_i2s *const lpc24xx_i2s = (lpc_i2s *) I2S_BASE_ADDR;

/* ADC */

#define ADC_CR_SEL(val) BSP_FLD32(val, 0, 7)
#define ADC_CR_SEL_GET(val) BSP_FLD32GET(val, 0, 7)
#define ADC_CR_SEL_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
#define ADC_CR_CLKDIV(val) BSP_FLD32(val, 8, 15)
#define ADC_CR_CLKDIV_GET(reg) BSP_FLD32GET(reg, 8, 15)
#define ADC_CR_CLKDIV_SET(reg, val) BSP_FLD32SET(reg, val, 8, 15)
#define ADC_CR_BURST BSP_BIT32(16)
#define ADC_CR_CLKS(val) BSP_FLD32(val, 17, 19)
#define ADC_CR_PDN BSP_BIT32(21)
#define ADC_CR_START(val) BSP_FLD32(val, 24, 26)
#define ADC_CR_EDGE BSP_BIT32(27)

#define ADC_DR_VALUE(reg) BSP_FLD32GET(reg, 6, 15)
#define ADC_DR_OVERRUN BSP_BIT32(30)
#define ADC_DR_DONE BSP_BIT32(31)

/* DAC */

#define DAC_STEPS 1024
#define DAC_VALUE(val) BSP_FLD32(val, 6, 15)
#define DAC_BIAS BSP_BIT32(16)

/** @} */

#endif /* LIBBSP_ARM_LPC24XX_LPC24XX_H */
