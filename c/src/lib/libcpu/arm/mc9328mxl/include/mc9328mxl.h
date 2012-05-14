/*
 * Motorola MC9328MXL Register definitions
 *
 * Copyright (c) 2003 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __MC9328MXL_H__
#define __MC9328MXL_H__

#include <stdint.h>

#define bit(_x_) (1 << (_x_))

/* Define some constants relating to the CPU */
#define MC9328MXL_NUM_INTS  64        /* CPU supports 64 interrupts */

/* Define the base addresses of the internal registers */
#define MC9328MXL_AIPI1_BASE    0x00200000
#define MC9328MXL_AIPI2_BASE    0x00210000
#define MC9328MXL_WDOG_BASE     0x00201000
#define MC9328MXL_TMR1_BASE     0x00202000
#define MC9328MXL_TMR2_BASE     0x00203000
#define MC9328MXL_RTC_BASE      0x00204000
#define MC9328MXL_LCDC_BASE     0x00205000
#define MC9328MXL_UART1_BASE    0x00206000
#define MC9328MXL_UART2_BASE    0x00207000
#define MC9328MXL_PWM_BASE      0x00208000
#define MC9328MXL_DMAC_BASE     0x00209000
#define MC9328MXL_USBD_BASE     0x00212000
#define MC9328MXL_SPI1_BASE     0x00213000
#define MC9328MXL_SPI2_BASE     0x00219000
#define MC9328MXL_MMC_BASE      0x00214000
#define MC9328MXL_I2C_BASE      0x00217000
#define MC9328MXL_SSI_BASE      0x00218000
#define MC9328MXL_MSHC_BASE     0x0021a000
#define MC9328MXL_PLL_BASE      0x0021b000
#define MC9328MXL_RESET_BASE    0x0021b800
#define MC9328MXL_SYSCTRL_BASE  0x0021b804
#define MC9328MXL_GPIOA_BASE    0x0021c000
#define MC9328MXL_GPIOB_BASE    0x0021c100
#define MC9328MXL_GPIOC_BASE    0x0021c200
#define MC9328MXL_GPIOD_BASE    0x0021c300
#define MC9328MXL_EIM_BASE      0x00220000
#define MC9328MXL_SDRAM_BASE    0x00221000
#define MC9328MXL_MMA_BASE      0x00222000
#define MC9328MXL_AITC_BASE     0x00223000
#define MC9328MXL_CSI_BASE      0x00224000


#define MC9328MXL_TMR1_TCTL   (*((volatile uint32_t *)((MC9328MXL_TMR1_BASE) + 0x00)))
#define MC9328MXL_TMR1_TPRER  (*((volatile uint32_t *)((MC9328MXL_TMR1_BASE) + 0x04)))
#define MC9328MXL_TMR1_TCMP   (*((volatile uint32_t *)((MC9328MXL_TMR1_BASE) + 0x08)))
#define MC9328MXL_TMR1_TCR    (*((volatile uint32_t *)((MC9328MXL_TMR1_BASE) + 0x0c)))
#define MC9328MXL_TMR1_TCN    (*((volatile uint32_t *)((MC9328MXL_TMR1_BASE) + 0x10)))
#define MC9328MXL_TMR1_TSTAT  (*((volatile uint32_t *)((MC9328MXL_TMR1_BASE) + 0x14)))

#define MC9328MXL_TMR2_TCTL   (*((volatile uint32_t *)((MC9328MXL_TMR2_BASE) + 0x00)))
#define MC9328MXL_TMR2_TPRER  (*((volatile uint32_t *)((MC9328MXL_TMR2_BASE) + 0x04)))
#define MC9328MXL_TMR2_TCMP   (*((volatile uint32_t *)((MC9328MXL_TMR2_BASE) + 0x08)))
#define MC9328MXL_TMR2_TCR    (*((volatile uint32_t *)((MC9328MXL_TMR2_BASE) + 0x0c)))
#define MC9328MXL_TMR2_TCN    (*((volatile uint32_t *)((MC9328MXL_TMR2_BASE) + 0x10)))
#define MC9328MXL_TMR2_TSTAT  (*((volatile uint32_t *)((MC9328MXL_TMR2_BASE) + 0x14)))

#define MC9328MXL_TMR_TCTL_SWR                (bit(15))
#define MC9328MXL_TMR_TCTL_FRR                (bit(8))
#define MC9328MXL_TMR_TCTL_CAP_DIS            (0 << 6)
#define MC9328MXL_TMR_TCTL_CAP_RISE           (1 << 6)
#define MC9328MXL_TMR_TCTL_CAP_FALL           (2 << 6)
#define MC9328MXL_TMR_TCTL_CAP_ANY            (3 << 6)
#define MC9328MXL_TMR_TCTL_OM                 (bit(5))
#define MC9328MXL_TMR_TCTL_IRQEN              (bit(4))
#define MC9328MXL_TMR_TCTL_CLKSRC_STOP        (0 << 1)
#define MC9328MXL_TMR_TCTL_CLKSRC_PCLK1       (1 << 1)
#define MC9328MXL_TMR_TCTL_CLKSRC_PCLK_DIV16  (2 << 1)
#define MC9328MXL_TMR_TCTL_CLKSRC_TIN         (3 << 1)
#define MC9328MXL_TMR_TCTL_CLKSRC_32KHX       (4 << 1)
#define MC9328MXL_TMR_TCTL_TEN                (bit(0))

#define MC9328MXL_UART1_RXD   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x00)))
#define MC9328MXL_UART1_TXD   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x40)))
#define MC9328MXL_UART1_CR1   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x80)))
#define MC9328MXL_UART1_CR2   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x84)))
#define MC9328MXL_UART1_CR3   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x88)))
#define MC9328MXL_UART1_CR4   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x8c)))
#define MC9328MXL_UART1_FCR   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x90)))
#define MC9328MXL_UART1_SR1   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x94)))
#define MC9328MXL_UART1_SR2   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x98)))
#define MC9328MXL_UART1_ESC   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0x9c)))
#define MC9328MXL_UART1_TIM   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xa0)))
#define MC9328MXL_UART1_BIR   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xa4)))
#define MC9328MXL_UART1_BMR   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xa8)))
#define MC9328MXL_UART1_BRC   (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xac)))
#define MC9328MXL_UART1_IPR1  (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xb0)))
#define MC9328MXL_UART1_IPR2  (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xb4)))
#define MC9328MXL_UART1_IPR3  (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xb8)))
#define MC9328MXL_UART1_IPR4  (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xbc)))
#define MC9328MXL_UART1_MPR1  (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xc0)))
#define MC9328MXL_UART1_MPR2  (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xc4)))
#define MC9328MXL_UART1_MPR3  (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xc8)))
#define MC9328MXL_UART1_MPR4  (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xcc)))
#define MC9328MXL_UART1_TS    (*((volatile uint32_t *)((MC9328MXL_UART1_BASE) + 0xd0)))

#define MC9328MXL_UART2_RXD   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x00)))
#define MC9328MXL_UART2_TXD   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x40)))
#define MC9328MXL_UART2_CR1   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x80)))
#define MC9328MXL_UART2_CR2   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x84)))
#define MC9328MXL_UART2_CR3   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x88)))
#define MC9328MXL_UART2_CR4   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x8c)))
#define MC9328MXL_UART2_FCR   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x90)))
#define MC9328MXL_UART2_SR1   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x94)))
#define MC9328MXL_UART2_SR2   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x98)))
#define MC9328MXL_UART2_ESC   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0x9c)))
#define MC9328MXL_UART2_TIM   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xa0)))
#define MC9328MXL_UART2_BIR   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xa4)))
#define MC9328MXL_UART2_BMR   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xa8)))
#define MC9328MXL_UART2_BRC   (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xac)))
#define MC9328MXL_UART2_IPR1  (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xb0)))
#define MC9328MXL_UART2_IPR2  (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xb4)))
#define MC9328MXL_UART2_IPR3  (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xb8)))
#define MC9328MXL_UART2_IPR4  (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xbc)))
#define MC9328MXL_UART2_MPR1  (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xc0)))
#define MC9328MXL_UART2_MPR2  (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xc4)))
#define MC9328MXL_UART2_MPR3  (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xc8)))
#define MC9328MXL_UART2_MPR4  (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xcc)))
#define MC9328MXL_UART2_TS    (*((volatile uint32_t *)((MC9328MXL_UART2_BASE) + 0xd0)))

typedef struct {
    volatile uint32_t rxd;
    volatile uint32_t _res0[15];
    volatile uint32_t txd;
    volatile uint32_t _res1[15];
    volatile uint32_t cr1;
    volatile uint32_t cr2;
    volatile uint32_t cr3;
    volatile uint32_t cr4;
    volatile uint32_t fcr;
    volatile uint32_t sr1;
    volatile uint32_t sr2;
    volatile uint32_t esc;
    volatile uint32_t tim;
    volatile uint32_t bir;
    volatile uint32_t bmr;
    volatile uint32_t brc;
    volatile uint32_t ipr1;
    volatile uint32_t ipr2;
    volatile uint32_t ipr3;
    volatile uint32_t ipr4;
    volatile uint32_t mpr1;
    volatile uint32_t mpr2;
    volatile uint32_t mpr3;
    volatile uint32_t mpr4;
    volatile uint32_t ts;
} mc9328mxl_uart_regs_t;

#define MC9328MXL_UART_RXD_CHARRDY    (bit(15))
#define MC9328MXL_UART_RXD_ERR        (bit(14))
#define MC9328MXL_UART_RXD_OVRRUN     (bit(13))
#define MC9328MXL_UART_RXD_FRMERR     (bit(12))
#define MC9328MXL_UART_RXD_BRK        (bit(11))
#define MC9328MXL_UART_RXD_PRERR      (bit(10))
#define MC9328MXL_UART_RXD_CHARMASK   (0xff)

#define MC9328MXL_UART_CR1_ADEN       (bit(15))
#define MC9328MXL_UART_CR1_ADBR       (bit(14))
#define MC9328MXL_UART_CR1_TRDYEN     (bit(13))
#define MC9328MXL_UART_CR1_IDEN       (bit(12))
#define MC9328MXL_UART_CR1_ICD4       (0x0 << 10)
#define MC9328MXL_UART_CR1_ICD8       (0x1 << 10)
#define MC9328MXL_UART_CR1_ICD16      (0x2 << 10)
#define MC9328MXL_UART_CR1_ICD32      (0x3 << 10)
#define MC9328MXL_UART_CR1_RRDYEN     (bit(9))
#define MC9328MXL_UART_CR1_RDMAEN     (bit(8))
#define MC9328MXL_UART_CR1_IREN       (bit(7))
#define MC9328MXL_UART_CR1_TXMPTYEN   (bit(6))
#define MC9328MXL_UART_CR1_RTSDEN     (bit(5))
#define MC9328MXL_UART_CR1_SNDBRK     (bit(4))
#define MC9328MXL_UART_CR1_TDMAEN     (bit(3))
#define MC9328MXL_UART_CR1_UARTCLKEN  (bit(2))
#define MC9328MXL_UART_CR1_DOZE       (bit(1))
#define MC9328MXL_UART_CR1_UARTEN     (bit(0))


#define MC9328MXL_UART_CR2_ESCI       (bit(15))
#define MC9328MXL_UART_CR2_IRTS       (bit(14))
#define MC9328MXL_UART_CR2_CTSC       (bit(13))
#define MC9328MXL_UART_CR2_CTS        (bit(12))
#define MC9328MXL_UART_CR2_ESCEN      (bit(11))
#define MC9328MXL_UART_CR2_RTEC_RE    (0 << 9)
#define MC9328MXL_UART_CR2_RTEC_FE    (1 << 9)
#define MC9328MXL_UART_CR2_RTEC_ANY   (2 << 9)
#define MC9328MXL_UART_CR2_PREN       (bit(8))
#define MC9328MXL_UART_CR2_PROE       (bit(7))
#define MC9328MXL_UART_CR2_STPB       (bit(6))
#define MC9328MXL_UART_CR2_WS         (bit(5))
#define MC9328MXL_UART_CR2_RTSEN      (bit(4))
#define MC9328MXL_UART_CR2_TXEN       (bit(2))
#define MC9328MXL_UART_CR2_RXEN       (bit(1))
#define MC9328MXL_UART_CR2_SRST       (bit(0))

#define MC9328MXL_UART_CR3_DPEC_RE    (0 << 14)  /* UART2 only */
#define MC9328MXL_UART_CR3_DPEC_FE    (1 << 14)  /* UART2 only */
#define MC9328MXL_UART_CR3_DPEC_ANY   (2 << 14)  /* UART2 only */
#define MC9328MXL_UART_CR3_DTREN      (bit(13))  /* UART2 only */
#define MC9328MXL_UART_CR3_PARERREN   (bit(12))
#define MC9328MXL_UART_CR3_FRAERREN   (bit(11))
#define MC9328MXL_UART_CR3_DSR        (bit(10))  /* UART2 only */
#define MC9328MXL_UART_CR3_DCD        (bit(9))   /* UART2 only */
#define MC9328MXL_UART_CR3_RI         (bit(8))   /* UART2 only */
#define MC9328MXL_UART_CR3_RXDSEN     (bit(6))
#define MC9328MXL_UART_CR3_AIRINTEN   (bit(5))
#define MC9328MXL_UART_CR3_AWAKEN     (bit(4))
#define MC9328MXL_UART_CR3_REF25      (bit(3))
#define MC9328MXL_UART_CR3_REF30      (bit(2))
#define MC9328MXL_UART_CR3_INVT       (bit(1))
#define MC9328MXL_UART_CR3_BPEN       (bit(0))

#define MC9328MXL_UART_CR4_CTSTL(_x_) (((_x_) & 0x3f) << 10)
#define MC9328MXL_UART_CR4_INVR       (bit(9))
#define MC9328MXL_UART_CR4_ENIRI      (bit(8))
#define MC9328MXL_UART_CR4_WKEN       (bit(7))
#define MC9328MXL_UART_CR4_REF16      (bit(6))
#define MC9328MXL_UART_CR4_IRSC       (bit(5))
#define MC9328MXL_UART_CR4_TCEN       (bit(3))
#define MC9328MXL_UART_CR4_BKEN       (bit(2))
#define MC9328MXL_UART_CR4_OREN       (bit(1))
#define MC9328MXL_UART_CR4_DREN       (bit(0))

#define MC9328MXL_UART_FCR_TXTL(x)    (((x) & 0x3f) << 10)
#define MC9328MXL_UART_FCR_RFDIV_1    (5 << 7)
#define MC9328MXL_UART_FCR_RFDIV_2    (4 << 7)
#define MC9328MXL_UART_FCR_RFDIV_3    (3 << 7)
#define MC9328MXL_UART_FCR_RFDIV_4    (2 << 7)
#define MC9328MXL_UART_FCR_RFDIV_5    (1 << 7)
#define MC9328MXL_UART_FCR_RFDIV_6    (0 << 7)
#define MC9328MXL_UART_FCR_RFDIV_7    (6 << 7)
#define MC9328MXL_UART_FCR_RFDIV_MASK (7 << 7)
#define MC9328MXL_UART_FCR_RXTL(x)    (((x) & 0x3f) << 0)

#define MC9328MXL_UART_SR1_PARERR     (bit(15))
#define MC9328MXL_UART_SR1_RTSS       (bit(14))
#define MC9328MXL_UART_SR1_TRDY       (bit(13))
#define MC9328MXL_UART_SR1_RTSD       (bit(12))
#define MC9328MXL_UART_SR1_ESCF       (bit(11))
#define MC9328MXL_UART_SR1_FRMERR     (bit(10))
#define MC9328MXL_UART_SR1_RRDY       (bit(9))
#define MC9328MXL_UART_SR1_RXDS       (bit(6))
#define MC9328MXL_UART_SR1_AIRINT     (bit(5))
#define MC9328MXL_UART_SR1_AWAKE      (bit(4))

#define MC9328MXL_UART_SR2_ADET       (bit(15))
#define MC9328MXL_UART_SR2_TXFE       (bit(14))
#define MC9328MXL_UART_SR2_DTRF       (bit(13))
#define MC9328MXL_UART_SR2_IDLE       (bit(12))
#define MC9328MXL_UART_SR2_IRINT      (bit(8))
#define MC9328MXL_UART_SR2_WAKE       (bit(7))
#define MC9328MXL_UART_SR2_RTSF       (bit(4))
#define MC9328MXL_UART_SR2_TXDC       (bit(3))
#define MC9328MXL_UART_SR2_BRCD       (bit(2))
#define MC9328MXL_UART_SR2_ORE        (bit(1))
#define MC9328MXL_UART_SR2_RDR        (bit(0))


#define MC9328MXL_PLL_CSCR   (*((volatile uint32_t *)((MC9328MXL_PLL_BASE) + 0x00)))
#define MC9328MXL_PLL_MPCTL0 (*((volatile uint32_t *)((MC9328MXL_PLL_BASE) + 0x04)))
#define MC9328MXL_PLL_MPCTL1 (*((volatile uint32_t *)((MC9328MXL_PLL_BASE) + 0x08)))
#define MC9328MXL_PLL_SPCTL0 (*((volatile uint32_t *)((MC9328MXL_PLL_BASE) + 0x0c)))
#define MC9328MXL_PLL_SPCTL1 (*((volatile uint32_t *)((MC9328MXL_PLL_BASE) + 0x10)))
#define MC9328MXL_PLL_PCDR   (*((volatile uint32_t *)((MC9328MXL_PLL_BASE) + 0x20)))

#define MC9328MXL_PLL_CSCR_CLKOSEL_PERCLK1 (0 << 29)
#define MC9328MXL_PLL_CSCR_CLKOSEL_HCLK    (1 << 29)
#define MC9328MXL_PLL_CSCR_CLKOSEL_CLK48M  (2 << 29)
#define MC9328MXL_PLL_CSCR_CLKOSEL_CLK16M  (3 << 29)
#define MC9328MXL_PLL_CSCR_CLKOSEL_PREMCLK (4 << 29)
#define MC9328MXL_PLL_CSCR_CLKOSEL_FCLK    (5 << 29)

#define MC9328MXL_PLL_CSCR_USBDIV(_x_)     (((_x_) & 0x7) << 26)
#define MC9328MXL_PLL_CSCR_SDCNT_1         (0 << 24)
#define MC9328MXL_PLL_CSCR_SDCNT_2         (1 << 24)
#define MC9328MXL_PLL_CSCR_SDCNT_3         (2 << 24)
#define MC9328MXL_PLL_CSCR_SDCNT_4         (3 << 24)

#define MC9328MXL_PLL_CSCR_SPLLRSTRT       (bit(22))
#define MC9328MXL_PLL_CSCR_MPLLRSTRT       (bit(21))
#define MC9328MXL_PLL_CSCR_CLK16SEL        (bit(18))
#define MC9328MXL_PLL_CSCR_OSCEN           (bit(17))
#define MC9328MXL_PLL_CSCR_SYSSEL          (bit(16))
#define MC9328MXL_PLL_CSCR_PRESC           (bit(15))
#define MC9328MXL_PLL_CSCR_BLKDIV(_x_)     (((_x_) & 0xf) << 10)
#define MC9328MXL_PLL_CSCR_SPEN            (bit(1))
#define MC9328MXL_PLL_CSCR_MPEN            (bit(0))

#define MC9328MXL_PLL_PCDR_PCLK1_MASK      (0x0000000f)
#define MC9328MXL_PLL_PCDR_PCLK1_SHIFT     (0)
#define MC9328MXL_PLL_PCDR_PCLK2_MASK      (0x000000f0)
#define MC9328MXL_PLL_PCDR_PCLK2_SHIFT     (4)
#define MC9328MXL_PLL_PCDR_PCLK3_MASK      (0x007f0000)
#define MC9328MXL_PLL_PCDR_PCLK3_SHIFT     (16)


#define MC9328MXL_PLL_SPCTL_PD_MASK        (0x3c000000)
#define MC9328MXL_PLL_SPCTL_PD_SHIFT       (26)
#define MC9328MXL_PLL_SPCTL_MFD_MASK       (0x03ff0000)
#define MC9328MXL_PLL_SPCTL_MFD_SHIFT      (16)
#define MC9328MXL_PLL_SPCTL_MFI_MASK       (0x00003c00)
#define MC9328MXL_PLL_SPCTL_MFI_SHIFT      (10)
#define MC9328MXL_PLL_SPCTL_MFN_MASK       (0x000003ff)
#define MC9328MXL_PLL_SPCTL_MFN_SHIFT      (0)


#define MC9328MXL_GPIOA_DDIR    (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x00)))
#define MC9328MXL_GPIOA_OCR1    (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x04)))
#define MC9328MXL_GPIOA_OCR2    (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x08)))
#define MC9328MXL_GPIOA_ICONFA1 (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x0c)))
#define MC9328MXL_GPIOA_ICONFA2 (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x10)))
#define MC9328MXL_GPIOA_ICONFB1 (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x14)))
#define MC9328MXL_GPIOA_ICONFB2 (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x18)))
#define MC9328MXL_GPIOA_DR      (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x1c)))
#define MC9328MXL_GPIOA_GIUS    (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x20)))
#define MC9328MXL_GPIOA_SSR     (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x24)))
#define MC9328MXL_GPIOA_ICR1    (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x28)))
#define MC9328MXL_GPIOA_ICR2    (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x2c)))
#define MC9328MXL_GPIOA_IMR     (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x30)))
#define MC9328MXL_GPIOA_ISR     (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x34)))
#define MC9328MXL_GPIOA_GPR     (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x38)))
#define MC9328MXL_GPIOA_SWR     (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x3c)))
#define MC9328MXL_GPIOA_PUEN    (*((volatile uint32_t *)((MC9328MXL_GPIOA_BASE) + 0x40)))

#define MC9328MXL_GPIOB_DDIR    (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x00)))
#define MC9328MXL_GPIOB_OCR1    (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x04)))
#define MC9328MXL_GPIOB_OCR2    (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x08)))
#define MC9328MXL_GPIOB_ICONFA1 (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x0c)))
#define MC9328MXL_GPIOB_ICONFA2 (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x10)))
#define MC9328MXL_GPIOB_ICONFB1 (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x14)))
#define MC9328MXL_GPIOB_ICONFB2 (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x18)))
#define MC9328MXL_GPIOB_DR      (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x1c)))
#define MC9328MXL_GPIOB_GIUS    (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x20)))
#define MC9328MXL_GPIOB_SSR     (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x24)))
#define MC9328MXL_GPIOB_ICR1    (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x28)))
#define MC9328MXL_GPIOB_ICR2    (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x2c)))
#define MC9328MXL_GPIOB_IMR     (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x30)))
#define MC9328MXL_GPIOB_ISR     (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x34)))
#define MC9328MXL_GPIOB_GPR     (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x38)))
#define MC9328MXL_GPIOB_SWR     (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x3c)))
#define MC9328MXL_GPIOB_PUEN    (*((volatile uint32_t *)((MC9328MXL_GPIOB_BASE) + 0x40)))

#define MC9328MXL_GPIOC_DDIR    (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x00)))
#define MC9328MXL_GPIOC_OCR1    (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x04)))
#define MC9328MXL_GPIOC_OCR2    (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x08)))
#define MC9328MXL_GPIOC_ICONFA1 (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x0c)))
#define MC9328MXL_GPIOC_ICONFA2 (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x10)))
#define MC9328MXL_GPIOC_ICONFB1 (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x14)))
#define MC9328MXL_GPIOC_ICONFB2 (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x18)))
#define MC9328MXL_GPIOC_DR      (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x1c)))
#define MC9328MXL_GPIOC_GIUS    (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x20)))
#define MC9328MXL_GPIOC_SSR     (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x24)))
#define MC9328MXL_GPIOC_ICR1    (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x28)))
#define MC9328MXL_GPIOC_ICR2    (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x2c)))
#define MC9328MXL_GPIOC_IMR     (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x30)))
#define MC9328MXL_GPIOC_ISR     (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x34)))
#define MC9328MXL_GPIOC_GPR     (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x38)))
#define MC9328MXL_GPIOC_SWR     (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x3c)))
#define MC9328MXL_GPIOC_PUEN    (*((volatile uint32_t *)((MC9328MXL_GPIOC_BASE) + 0x40)))

#define MC9328MXL_GPIOD_DDIR    (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x00)))
#define MC9328MXL_GPIOD_OCR1    (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x04)))
#define MC9328MXL_GPIOD_OCR2    (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x08)))
#define MC9328MXL_GPIOD_ICONFA1 (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x0c)))
#define MC9328MXL_GPIOD_ICONFA2 (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x10)))
#define MC9328MXL_GPIOD_ICONFB1 (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x14)))
#define MC9328MXL_GPIOD_ICONFB2 (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x18)))
#define MC9328MXL_GPIOD_DR      (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x1c)))
#define MC9328MXL_GPIOD_GIUS    (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x20)))
#define MC9328MXL_GPIOD_SSR     (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x24)))
#define MC9328MXL_GPIOD_ICR1    (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x28)))
#define MC9328MXL_GPIOD_ICR2    (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x2c)))
#define MC9328MXL_GPIOD_IMR     (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x30)))
#define MC9328MXL_GPIOD_ISR     (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x34)))
#define MC9328MXL_GPIOD_GPR     (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x38)))
#define MC9328MXL_GPIOD_SWR     (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x3c)))
#define MC9328MXL_GPIOD_PUEN    (*((volatile uint32_t *)((MC9328MXL_GPIOD_BASE) + 0x40)))

#define MC9328MXL_AITC_INTCNTL    (*((volatile uint32_t *)((MC9328MXL_AITC_BASE) + 0x00)))
#define MC9328MXL_AITC_NIMASK     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE) + 0x04)))
#define MC9328MXL_AITC_INTENNUM   (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x08)))
#define MC9328MXL_AITC_INTDISNUM  (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x0c)))
#define MC9328MXL_AITC_INTENABLEH (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x10)))
#define MC9328MXL_AITC_INTENABLEL (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x14)))
#define MC9328MXL_AITC_INTTYPEH   (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x18)))
#define MC9328MXL_AITC_INTTYPEL   (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x1c)))
#define MC9328MXL_AITC_NIPIR7     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x20)))
#define MC9328MXL_AITC_NIPRI6     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x24)))
#define MC9328MXL_AITC_NIPRI5     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x28)))
#define MC9328MXL_AITC_NIPRI4     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x2c)))
#define MC9328MXL_AITC_NIPRI3     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x30)))
#define MC9328MXL_AITC_NIPRI2     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x34)))
#define MC9328MXL_AITC_NIPRI1     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x38)))
#define MC9328MXL_AITC_NIPRI0     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x3c)))
#define MC9328MXL_AITC_NIVECSR    (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x40)))
#define MC9328MXL_AITC_FIVECSR    (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x44)))
#define MC9328MXL_AITC_INTSRCH    (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x48)))
#define MC9328MXL_AITC_INTSRCL    (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x4c)))
#define MC9328MXL_AITC_INTFRCH    (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x50)))
#define MC9328MXL_AITC_INTFRCL    (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x54)))
#define MC9328MXL_AITC_NIPNDH     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x58)))
#define MC9328MXL_AITC_NIPNDL     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x5c)))
#define MC9328MXL_AITC_FIPNDH     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x60)))
#define MC9328MXL_AITC_FIPNDL     (*((volatile uint32_t *)((MC9328MXL_AITC_BASE)+ 0x64)))

#define MC9328MXL_INT_UART3_PFERR          (0)
#define MC9328MXL_INT_UART3_RTS            (1)
#define MC9328MXL_INT_UART3_DTR            (2)
#define MC9328MXL_INT_UART3_UARTC          (3)
#define MC9328MXL_INT_UART3_TX             (4)
#define MC9328MXL_INT_PEN_UP               (5)
#define MC9328MXL_INT_CSI                  (6)
#define MC9328MXL_INT_MMA_MAC              (7)
#define MC9328MXL_INT_MMA                  (8)
#define MC9328MXL_INT_COMP                 (9)
#define MC9328MXL_INT_MSIRQ                (10)
#define MC9328MXL_INT_GPIO_PORTA           (11)
#define MC9328MXL_INT_GPIO_PORTB           (12)
#define MC9328MXL_INT_GPIO_PORTC           (13)
#define MC9328MXL_INT_LCDC                 (14)
#define MC9328MXL_INT_SIM_IRQ              (15)
#define MC9328MXL_INT_SIM_DATA             (16)
#define MC9328MXL_INT_RTC                  (17)
#define MC9328MXL_INT_RTC_SAM              (18)
#define MC9328MXL_INT_UART2_PFERR          (19)
#define MC9328MXL_INT_UART2_RTS            (20)
#define MC9328MXL_INT_UART2_DTR            (21)
#define MC9328MXL_INT_UART2_UARTC          (22)
#define MC9328MXL_INT_UART2_TX             (23)
#define MC9328MXL_INT_UART2_RX             (24)
#define MC9328MXL_INT_UART1_PFERR          (25)
#define MC9328MXL_INT_UART1_RTS            (26)
#define MC9328MXL_INT_UART1_DTR            (27)
#define MC9328MXL_INT_UART1_UARTC          (28)
#define MC9328MXL_INT_UART1_TX             (29)
#define MC9328MXL_INT_UART1_RX             (30)
#define MC9328MXL_INT_PEN_DATA             (33)
#define MC9328MXL_INT_PWM                  (34)
#define MC9328MXL_INT_MMC_IRQ              (35)
#define MC9328MXL_INT_SSI2_TX              (36)
#define MC9328MXL_INT_SSI2_RX              (37)
#define MC9328MXL_INT_SSI2_ERR             (38)
#define MC9328MXL_INT_I2C                  (39)
#define MC9328MXL_INT_SPI2                 (40)
#define MC9328MXL_INT_SPI1                 (41)
#define MC9328MXL_INT_SSI_TX               (42)
#define MC9328MXL_INT_SSI_TX_ERR           (43)
#define MC9328MXL_INT_SSI_RX               (44)
#define MC9328MXL_INT_SSI_RX_ERR           (45)
#define MC9328MXL_INT_TOUCH                (46)
#define MC9328MXL_INT_USBD0                (47)
#define MC9328MXL_INT_USBD1                (48)
#define MC9328MXL_INT_USBD2                (49)
#define MC9328MXL_INT_USBD3                (50)
#define MC9328MXL_INT_USBD4                (51)
#define MC9328MXL_INT_USBD5                (52)
#define MC9328MXL_INT_USBD6                (53)
#define MC9328MXL_INT_UART3_RX             (54)
#define MC9328MXL_INT_BTSYS                (55)
#define MC9328MXL_INT_BTTIM                (56)
#define MC9328MXL_INT_BTWUI                (57)
#define MC9328MXL_INT_TIMER2               (58)
#define MC9328MXL_INT_TIMER1               (59)
#define MC9328MXL_INT_DMA_ERR              (60)
#define MC9328MXL_INT_DMA                  (61)
#define MC9328MXL_INT_GPIO_PORTD           (62)
#define MC9328MXL_INT_WDT                  (63)

#define MC9328MXL_AITC_INTCTL_NIAD    BIT(20)
#define MC9328MXL_AITC_INTCTL_FIAD    BIT(19)
#define MC9328MXL_AITC_NIMASK_MASK (0X1F)
#define MC9328MXL_AITC_INTENNUM_MASK  (0X3F)
#define MC9328MXL_AITC_INTDISNUM_MASK (0X3F)
#define MC9328MXL_AITC_NIPRIORITY(_int_, _pri_) \
           (((_pri_) & 0xf) << ((_int_) & 0x7) * 4)


#endif /* __MC9328MXL_H__ */
