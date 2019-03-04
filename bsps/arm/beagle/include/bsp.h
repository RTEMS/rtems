/**
 * @file
 *
 * @ingroup RTEMSBSPsARMBeagle
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 * Modified by Ben Gras <beng@shrike-systems.com> to add lots
 * of beagleboard/beaglebone definitions, delete lpc32xx specific
 * ones, and merge with some other header files.
 */

#ifndef LIBBSP_ARM_BEAGLE_BSP_H
#define LIBBSP_ARM_BEAGLE_BSP_H

/**
 * @defgroup RTEMSBSPsARMBeagle Beaglebone
 *
 * @ingroup RTEMSBSPsARM
 *
 * @brief Beaglebone Board Support Package.
 *
 * @{
 */

#include <bspopts.h>
#include <stdint.h>
#include <bsp/start.h>
#include <bsp/default-initial-extension.h>
#include <bsp/beagleboneblack.h>

#include <rtems.h>
#include <rtems/irq-extension.h>

#include <libcpu/omap3.h>
#include <libcpu/am335x.h>

#define BSP_FEATURE_IRQ_EXTENSION

/* UART base clock frequency */
#define UART_CLOCK     48000000

/* Access memory-mapped I/O devices */
#define mmio_read(a)    (*(volatile uint32_t *)(a))
#define mmio_write(a,v) (*(volatile uint32_t *)(a) = (v))
#define mmio_set(a,v)   mmio_write((a), mmio_read((a)) | (v))
#define mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#define REG16(x)(*((volatile uint16_t *)(x)))
#define REG(x)(*((volatile uint32_t *)(x)))
#define BIT(x)(0x1 << (x))
// Start and End included
#define BITS(Start, End) (((1 << (End+1)) - 1) & ~((1 << (Start)) - 1))

#define udelay(u) rtems_task_wake_after(1 + ((u)/rtems_configuration_get_microseconds_per_tick()))

/* Write a uint32_t value to a memory address. */
static inline void
write32(uint32_t address, uint32_t value)
{
    REG(address) = value;
}

/* Read an uint32_t from a memory address */
static inline uint32_t
read32(uint32_t address)
{
    return REG(address);
}

/* Set a 32 bits value depending on a mask */
static inline void
set32(uint32_t address, uint32_t mask, uint32_t value)
{
    uint32_t val;
    val = read32(address);
    /* clear the bits */
    val &= ~(mask);
    /* apply the value using the mask */
    val |= (value & mask);
    write32(address, val);
}

/* Write a uint16_t value to a memory address. */
static inline void
write16(uint32_t address, uint16_t value)
{
    REG16(address) = value;
}

/* Read an uint16_t from a memory address */
static inline uint16_t
read16(uint32_t address)
{
    return REG16(address);
}

/* Data synchronization barrier */
static inline void dsb(void)
{
    asm volatile("dsb" : : : "memory");
}

/* Instruction synchronization barrier */
static inline void isb(void)
{
    asm volatile("isb" : : : "memory");
}

/* flush data cache */
static inline void flush_data_cache(void)
{
    asm volatile(
        "mov r0, #0\n"
        "mcr p15, #0, r0, c7, c10, #4\n"
        : /* No outputs */
        : /* No inputs */
        : "r0","memory"
    );
}

#define __arch_getb(a)      (*(volatile unsigned char *)(a))
#define __arch_getw(a)      (*(volatile unsigned short *)(a))
#define __arch_getl(a)      (*(volatile unsigned int *)(a))

#define __arch_putb(v,a)    (*(volatile unsigned char *)(a) = (v))
#define __arch_putw(v,a)    (*(volatile unsigned short *)(a) = (v))
#define __arch_putl(v,a)    (*(volatile unsigned int *)(a) = (v))

#define writeb(v,c) ({ unsigned char  __v = v; __arch_putb(__v,c); __v; })
#define writew(v,c) ({ unsigned short __v = v; __arch_putw(__v,c); __v; })
#define writel(v,c) ({ unsigned int __v = v; __arch_putl(__v,c); __v; })

#define readb(c)  ({ unsigned char  __v = __arch_getb(c); __v; })
#define readw(c)  ({ unsigned short __v = __arch_getw(c); __v; })
#define readl(c)  ({ unsigned int __v = __arch_getl(c); __v; })

#define SYSTEM_CLOCK_12       12000000
#define SYSTEM_CLOCK_13       13000000
#define SYSTEM_CLOCK_192      19200000
#define SYSTEM_CLOCK_96       96000000

#if !defined(IS_DM3730) && !defined(IS_AM335X)
#error Unrecognized BSP configured.
#endif

#if IS_DM3730
#define BSP_DEVICEMEM_START 0x48000000
#define BSP_DEVICEMEM_END   0x5F000000
#endif

#if IS_AM335X
#define BSP_DEVICEMEM_START 0x44000000
#define BSP_DEVICEMEM_END   0x57000000
#endif

/* per-target uart config */
#if IS_AM335X
#define BSP_CONSOLE_UART        1
#define BSP_CONSOLE_UART_BASE   BEAGLE_BASE_UART_1
#define BSP_CONSOLE_UART_IRQ    OMAP3_UART1_IRQ
#define BEAGLE_BASE_UART_1      0x44E09000
#define BEAGLE_BASE_UART_2      0x48022000
#define BEAGLE_BASE_UART_3      0x48024000
#endif

/* per-target uart config */
#if IS_DM3730
#define BSP_CONSOLE_UART        3
#define BSP_CONSOLE_UART_BASE   BEAGLE_BASE_UART_3
#define BSP_CONSOLE_UART_IRQ    OMAP3_UART3_IRQ
#define BEAGLE_BASE_UART_1      0x4806A000
#define BEAGLE_BASE_UART_2      0x4806C000
#define BEAGLE_BASE_UART_3      0x49020000
#endif

/* GPIO pin config */
#if IS_AM335X
#define BSP_GPIO_PIN_COUNT 128
#define BSP_GPIO_PINS_PER_BANK 32
#endif

#if IS_DM3730
#define BSP_GPIO_PIN_COUNT 192
#define BSP_GPIO_PINS_PER_BANK 32
#endif

#if BSP_START_COPY_FDT_FROM_U_BOOT
#define BSP_FDT_IS_SUPPORTED
#endif

/* i2c stuff */
typedef struct {
    uint32_t rx_or_tx;
    uint32_t stat;
    uint32_t ctrl;
    uint32_t clk_hi;
    uint32_t clk_lo;
    uint32_t adr;
    uint32_t rxfl;
    uint32_t txfl;
    uint32_t rxb;
    uint32_t txb;
    uint32_t s_tx;
    uint32_t s_txfl;
} beagle_i2c;

/* sctlr */
/* Read System Control Register */
static inline uint32_t read_sctlr(void)
{
    uint32_t ctl;

    asm volatile("mrc p15, 0, %[ctl], c1, c0, 0 @ Read SCTLR\n\t"
        : [ctl] "=r" (ctl));
    return ctl;
}

/* Write System Control Register */
static inline void write_sctlr(uint32_t ctl)
{
    asm volatile("mcr p15, 0, %[ctl], c1, c0, 0 @ Write SCTLR\n\t"
        : : [ctl] "r" (ctl));
    isb();
}

/* Read Auxiliary Control Register */
static inline uint32_t read_actlr(void)
{
    uint32_t ctl;

    asm volatile("mrc p15, 0, %[ctl], c1, c0, 1 @ Read ACTLR\n\t"
            : [ctl] "=r" (ctl));
    return ctl;
}

/* Write Auxiliary Control Register */
static inline void write_actlr(uint32_t ctl)
{
    asm volatile("mcr p15, 0, %[ctl], c1, c0, 1 @ Write ACTLR\n\t"
        : : [ctl] "r" (ctl));
    isb();
}

/* Write Translation Table Base Control Register */
static inline void write_ttbcr(uint32_t bcr)
{
        asm volatile("mcr p15, 0, %[bcr], c2, c0, 2 @ Write TTBCR\n\t"
                        : : [bcr] "r" (bcr));

        isb();
}

/* Read Domain Access Control Register */
static inline uint32_t read_dacr(void)
{
        uint32_t dacr;

        asm volatile("mrc p15, 0, %[dacr], c3, c0, 0 @ Read DACR\n\t"
                        : [dacr] "=r" (dacr));

        return dacr;
}


/* Write Domain Access Control Register */
static inline void write_dacr(uint32_t dacr)
{
        asm volatile("mcr p15, 0, %[dacr], c3, c0, 0 @ Write DACR\n\t"
                        : : [dacr] "r" (dacr));

        isb();
}

static inline void refresh_tlb(void)
{
    dsb();

    /* Invalidate entire unified TLB */
    asm volatile("mcr p15, 0, %[zero], c8, c7, 0 @ TLBIALL\n\t"
        : : [zero] "r" (0));

    /* Invalidate all instruction caches to PoU.
     * Also flushes branch target cache. */
    asm volatile("mcr p15, 0, %[zero], c7, c5, 0"
        : : [zero] "r" (0));

    /* Invalidate entire branch predictor array */
    asm volatile("mcr p15, 0, %[zero], c7, c5, 6"
        : : [zero] "r" (0)); /* flush BTB */

    dsb();
    isb();
}

/* Read Translation Table Base Register 0 */
static inline uint32_t read_ttbr0(void)
{
    uint32_t bar;

    asm volatile("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
        : [bar] "=r" (bar));

    return bar & ARM_TTBR_ADDR_MASK;
}


/* Read Translation Table Base Register 0 */
static inline uint32_t read_ttbr0_unmasked(void)
{
    uint32_t bar;

    asm volatile("mrc p15, 0, %[bar], c2, c0, 0 @ Read TTBR0\n\t"
        : [bar] "=r" (bar));

    return bar;
}

/* Write Translation Table Base Register 0 */
static inline void write_ttbr0(uint32_t bar)
{
    dsb();
    isb();
    /* In our setup TTBR contains the base address *and* the flags
       but other pieces of the kernel code expect ttbr to be the
       base address of the l1 page table. We therefore add the
       flags here and remove them in the read_ttbr0 */
    uint32_t v  =  (bar  & ARM_TTBR_ADDR_MASK ) | ARM_TTBR_FLAGS_CACHED;
    asm volatile("mcr p15, 0, %[bar], c2, c0, 0 @ Write TTBR0\n\t"
        : : [bar] "r" (v));

    refresh_tlb();
}

/* Behaviour on fatal error; default: test-friendly.
 * set breakpoint to bsp_fatal_extension.
 */
/* Enabling BSP_PRESS_KEY_FOR_RESET prevents noninteractive testing */
/*#define  BSP_PRESS_KEY_FOR_RESET     1 */
#define    BSP_PRINT_EXCEPTION_CONTEXT 1
    /* human-readable exception info */
#define    BSP_RESET_BOARD_AT_EXIT 1
    /* causes qemu to exit, signaling end of test */

/**
 * @brief Beagleboard specific set up of the MMU.
 *
 * Provide in the application to override.
 */
BSP_START_TEXT_SECTION void beagle_setup_mmu_and_cache(void);

/* @} */

#endif /* LIBBSP_ARM_BEAGLE_BSP_H */
