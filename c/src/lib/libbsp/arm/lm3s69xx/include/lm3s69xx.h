/*
 * Copyright © 2013 Eugeniy Meshcheryakov <eugen@debian.org>
 *
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
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

#ifndef LIBBSP_ARM_LM3S69XX_LM3S69XX_H
#define LIBBSP_ARM_LM3S69XX_LM3S69XX_H
#include <bspopts.h>
#include <bsp/utility.h>

#define LM3S69XX_SYSCON_BASE 0x400fe000

#define LM3S69XX_UART_0_BASE 0x4000c000
#define LM3S69XX_UART_1_BASE 0x4000d000
#define LM3S69XX_UART_2_BASE 0x4000e000

#ifdef LM3S69XX_USE_AHB_FOR_GPIO
#define LM3S69XX_GPIO_A_BASE 0x40058000
#define LM3S69XX_GPIO_B_BASE 0x40059000
#define LM3S69XX_GPIO_C_BASE 0x4005a000
#define LM3S69XX_GPIO_D_BASE 0x4005b000
#define LM3S69XX_GPIO_E_BASE 0x4005c000
#define LM3S69XX_GPIO_F_BASE 0x4005d000
#if LM3S69XX_NUM_GPIO_BLOCKS > 6
#define LM3S69XX_GPIO_G_BASE 0x4005e000
#if LM3S69XX_NUM_GPIO_BLOCKS > 7
#define LM3S69XX_GPIO_H_BASE 0x4005f000
#endif
#endif

#define LM3S69XX_GPIO(port) ((volatile lm3s69xx_gpio *)(LM3S69XX_GPIO_A_BASE + (port) * 0x1000))
#else /* LM3S69XX_USE_AHB_FOR_GPIO */
#define LM3S69XX_GPIO_A_BASE 0x40004000
#define LM3S69XX_GPIO_B_BASE 0x40005000
#define LM3S69XX_GPIO_C_BASE 0x40006000
#define LM3S69XX_GPIO_D_BASE 0x40007000
#define LM3S69XX_GPIO_E_BASE 0x40024000
#define LM3S69XX_GPIO_F_BASE 0x40025000
#if LM3S69XX_NUM_GPIO_BLOCKS > 6
#define LM3S69XX_GPIO_G_BASE 0x40026000
#if LM3S69XX_NUM_GPIO_BLOCKS > 7
#define LM3S69XX_GPIO_H_BASE 0x40027000
#endif
#endif

#define LM3S69XX_GPIO(port) ((volatile lm3s69xx_gpio *)(((port) < 4) ? \
           (LM3S69XX_GPIO_A_BASE + (port) * 0x1000) : \
           (LM3S69XX_GPIO_E_BASE + ((port) - 4) * 0x1000)))
#endif /* LM3S69XX_USE_AHB_FOR_GPIO */

#define LM3S69XX_SSI_0_BASE 0x40008000
#if LM3S69XX_NUM_SSI_BLOCKS > 1
#define LM3S69XX_SSI_1_BASE 0x40009000
#if LM3S69XX_NUM_SSI_BLOCKS > 2
#define LM3S69XX_SSI_2_BASE 0x4000A000
#if LM3S69XX_NUM_SSI_BLOCKS > 3
#define LM3S69XX_SSI_3_BASE 0x4000B000
#endif
#endif
#endif

#define LM3S69XX_SYSCON ((volatile lm3s69xx_syscon *)LM3S69XX_SYSCON_BASE)

#define LM3S69XX_PLL_FREQUENCY 400000000U

typedef struct  {
  uint32_t data[256]; /* Masked data registers are included here. */
  uint32_t dir;
  uint32_t is;
  uint32_t ibe;
  uint32_t iev;
  uint32_t im;
  uint32_t ris;
  uint32_t mis;
  uint32_t icr;
  uint32_t afsel;

  uint32_t reserved_0[55];

  uint32_t dr2r;
  uint32_t dr4r;
  uint32_t dr8r;
  uint32_t odr;
  uint32_t pur;
  uint32_t pdr;
  uint32_t slr;
  uint32_t den;
  uint32_t lock;
  uint32_t cr;
  uint32_t amsel;
} lm3s69xx_gpio;

typedef struct {
  uint32_t did0;
  uint32_t did1;

  uint32_t dc0;
  uint32_t reserved_0;
  uint32_t dc1;
  uint32_t dc2;
  uint32_t dc3;
  uint32_t dc4;
  uint32_t dc5;
  uint32_t dc6;
  uint32_t dc7;

  uint32_t reserved_1;

#define SYSCONPBORCTL_BORIOR BSP_BIT32(1)
  uint32_t pborctl;

#define SYSCONLDOPCTL_VADJ(val) BSP_FLD32(val, 0, 5)
#define SYSCONLDOPCTL_VADJ_MASK BSP_MSK32(0, 5)
  uint32_t ldopctl;

  uint32_t reserved_2[2];

  uint32_t srcr0;
  uint32_t srcr1;
  uint32_t srcr2;

  uint32_t reserved_3;

#define SYSCONRIS_MOSCPUPRIS BSP_BIT32(8)
#define SYSCONRIS_USBPLLRIS BSP_BIT32(7)
#define SYSCONRIS_PLLLRIS BSP_BIT32(6)
#define SYSCONRIS_BORRIS BSP_BIT32(1)
  uint32_t ris;

#define SYSCONIMC_MOSCPUPIM BSP_BIT32(8)
#define SYSCONIMC_USBPLLLIM BSP_BIT32(7)
#define SYSCONIMC_PLLLIM BSP_BIT32(6)
#define SYSCONIMC_BORIM BSP_BIT32(1)
  uint32_t imc;

#define SYSCONMISC_MOSCPUPMIS BSP_BIT32(8)
#define SYSCONMISC_USBPLLLMIS BSP_BIT32(7)
#define SYSCONMISC_PLLLMIS BSP_BIT32(6)
#define SYSCONMISC_BORMIS BSP_BIT32(1)
  uint32_t misc;

#define SYSCONRESC_MOSCFAIL BSP_BIT32(16)
#define SYSCONRESC_SW BSP_BIT32(4)
#define SYSCONRESC_WDT BSP_BIT32(3)
#define SYSCONRESC_BOR BSP_BIT32(2)
#define SYSCONRESC_POR BSP_BIT32(1)
#define SYSCONRESC_EXT BSP_BIT32(0)
  uint32_t resc;

#define SYSCONRCC_AGC BSP_BIT32(27)
#define SYSCONRCC_SYSDIV(val) BSP_FLD32(val, 23, 26)
#define SYSCONRCC_SYSDIV_MSK BSP_MSK32(23, 26)
#define SYSCONRCC_USESYSDIV BSP_BIT32(22)
#define SYSCONRCC_USEPWMDIV BSP_BIT32(20)
#define SYSCONRCC_PWMDIV(val) BSP_FLD32(val, 17, 19)
#define SYSCONRCC_PWMDIV_DIV2_VAL 0
#define SYSCONRCC_PWMDIV_DIV4_VAL 1
#define SYSCONRCC_PWMDIV_DIV8_VAL 2
#define SYSCONRCC_PWMDIV_DIV16_VAL 3
#define SYSCONRCC_PWMDIV_DIV32_VAL 4
#define SYSCONRCC_PWMDIV_DIV64_VAL 5
#define SYSCONRCC_PWMDIV_MSK BSP_MSK32(17, 19)
#define SYSCONRCC_PWRDN BSP_BIT32(13)
#define SYSCONRCC_BYPASS BSP_BIT32(11)
#define SYSCONRCC_XTAL(val) BSP_FLD32(val, 6, 10)
#define SYSCONRCC_XTAL_MSK BSP_MSK32(6, 10)
#define SYSCONRCC_OSCSRC(val) BSP_FLD32(val, 4, 5)
#define SYSCONRCC_OSCSRC_MOSC SYSCONRCC_OSCSRC(0x0)
#define SYSCONRCC_OSCSRC_IOSC SYSCONRCC_OSCSRC(0x1)
#define SYSCONRCC_OSCSRC_IOSC_DIV_4 SYSCONRCC_OSCSRC(0x2)
#define SYSCONRCC_OSCSRC_30KHZ SYSCONRCC_OSCSRC(0x3)
#define SYSCONRCC_OSCSRC_MSK BSP_MSK32(4, 5)
#define SYSCONRCC_IOSCDIS BSP_BIT32(1)
#define SYSCONRCC_MOSCDIS BSP_BIT32(0)
  uint32_t rcc;

#define SYSCONPLLCFG_F(val) BSP_FLD32(val, 5, 13)
#define SYSCONPLLCFG_F_MSK BSP_MSK32(5, 13)
#define SYSCONPLLCFG_R(val) BSP_FLD32(val, 0, 4)
#define SYSCONPLLCFG_R_MSK BSP_MSK32(0, 4)
  uint32_t pllcfg;

  uint32_t reserved_4;

#define SYSCONGPIOHBCTL_PORTH BSP_BIT32(7)
#define SYSCONGPIOHBCTL_PORTG BSP_BIT32(6)
#define SYSCONGPIOHBCTL_PORTF BSP_BIT32(5)
#define SYSCONGPIOHBCTL_PORTE BSP_BIT32(4)
#define SYSCONGPIOHBCTL_PORTD BSP_BIT32(3)
#define SYSCONGPIOHBCTL_PORTC BSP_BIT32(2)
#define SYSCONGPIOHBCTL_PORTB BSP_BIT32(1)
#define SYSCONGPIOHBCTL_PORTA BSP_BIT32(0)
  uint32_t gpiohbctl;

#define SYSCONRCC2_USERCC2 BSP_BIT32(31)
#define SYSCONRCC2_DIV400 BSP_BIT32(30)
#define SYSCONRCC2_SYSDIV2(val) BSP_FLD32(val, 23, 28)
#define SYSCONRCC2_SYSDIV2_MSK BSP_MSK32(23, 28)
#define SYSCONRCC2_SYSDIV2EXT(val) BSP_FLD32(val, 22, 28)
#define SYSCONRCC2_SYSDIV2EXT_MSK BSP_MSK32(22, 28)
#define SYSCONRCC2_USBPWRDN BSP_BIT32(14)
#define SYSCONRCC2_PWRDN2 BSP_BIT32(13)
#define SYSCONRCC2_BYPASS2 BSP_BIT32(11)
#define SYSCONRCC2_OSCSRC2(val) BSP_FLD32(val, 4, 6)
#define SYSCONRCC2_OSCSRC2_MSK BSP_MSK32(4, 6)
  uint32_t rcc2;

  uint32_t reserved_5[2];

#define SYSCONMOSCCTL_CVAL BSP_BIT32(0)
  uint32_t moscctl;

  uint32_t reserved_6[32];

#define SYSCONRCGC0_PWM BSP_BIT32(20)
#define SYSCONRCGC0_ADC BSP_BIT32(16)
#define SYSCONRCGC0_MAXADCSPD(val) BSP_FLD32(val, 8, 9)
#define SYSCONRCGC0_MAXADCSPD_MSK BSP_MSK32(8, 9)
#define SYSCONRCGC0_HIB BSP_BIT32(6)
#define SYSCONRCGC0_WDT BSP_BIT32(3)
  uint32_t rcgc0;

#define SYSCONRCGC1_COMP1 BSP_BIT32(25)
#define SYSCONRCGC1_COMP0 BSP_BIT32(24)
#define SYSCONRCGC1_TIMER3 BSP_BIT32(19)
#define SYSCONRCGC1_TIMER2 BSP_BIT32(18)
#define SYSCONRCGC1_TIMER1 BSP_BIT32(17)
#define SYSCONRCGC1_TIMER0 BSP_BIT32(16)
#define SYSCONRCGC1_I2C1 BSP_BIT32(14)
#define SYSCONRCGC1_I2C0 BSP_BIT32(12)
#define SYSCONRCGC1_QEI0 BSP_BIT32(8)
#if LM3S69XX_NUM_SSI_BLOCKS > 1
#define SYSCONRCGC1_SSI1 BSP_BIT32(5)
#endif
#define SYSCONRCGC1_SSI0 BSP_BIT32(4)
#define SYSCONRCGC1_UART2 BSP_BIT32(2)
#define SYSCONRCGC1_UART1 BSP_BIT32(1)
#define SYSCONRCGC1_UART0 BSP_BIT32(0)
  uint32_t rcgc1;

#define SYSCONRCGC2_USB0 BSP_BIT32(16)
#define SYSCONRCGC2_UDMA BSP_BIT32(13)
#if LM3S69XX_NUM_GPIO_BLOCKS > 7
#define SYSCONRCGC2_GPIOH BSP_BIT32(7)
#endif
#define SYSCONRCGC2_GPIOG BSP_BIT32(6)
#define SYSCONRCGC2_GPIOF BSP_BIT32(5)
#define SYSCONRCGC2_GPIOE BSP_BIT32(4)
#define SYSCONRCGC2_GPIOD BSP_BIT32(3)
#define SYSCONRCGC2_GPIOC BSP_BIT32(2)
#define SYSCONRCGC2_GPIOB BSP_BIT32(1)
#define SYSCONRCGC2_GPIOA BSP_BIT32(0)
  uint32_t rcgc2;

  uint32_t reserved_7;

  uint32_t scgc0;
  uint32_t scgc1;
  uint32_t scgc2;

  uint32_t reserved_8;

  uint32_t dcgc0;
  uint32_t dcgc1;
  uint32_t dcgc2;

  uint32_t reserved_9[6];

#define SYSCONDSLPCLKCFG_DSDIVORIDE(val) BSP_FLD32(val, 23, 28)
#define SYSCONDSLPCLKCFG_DSDIVORIDE_MSK BSP_MSK32(23, 28)
#define SYSCONDSLPCLKCFG_DSOSCSRC(val) BSP_FLD32(val, 4, 6)
#define SYSCONDSLPCLKCFG_DSOSCSRC_MSK BSP_MSK32(4, 6)
  uint32_t dslpclkcfg;
} lm3s69xx_syscon;

typedef struct {
#define UARTDR_OE BSP_BIT32(11)
#define UARTDR_BE BSP_BIT32(10)
#define UARTDR_PE BSP_BIT32(9)
#define UARTDR_FE BSP_BIT32(8)
#define UARTDR_ERROR_MSK BSP_MSK32(8, 11)
#define UARTDR_DATA(val) BSP_FLD32(val, 0, 7)
#define UARTDR_DATA_GET(reg) BSP_FLD32GET(reg, 0, 7)
  uint32_t dr;

  uint32_t rsr_ecr;
  uint32_t reserved_0[4];

#define UARTFR_TXFE BSP_BIT32(7)
#define UARTFR_RXFF BSP_BIT32(6)
#define UARTFR_TXFF BSP_BIT32(5)
#define UARTFR_RXFE BSP_BIT32(4)
#define UARTFR_BUSY BSP_BIT32(3)
  uint32_t fr;

  uint32_t reserved_1;

  uint32_t ilpr;
  uint32_t ibrd;
  uint32_t fbrd;

#define UARTLCRH_SPS BSP_BIT32(7)
#define UARTLCRH_WLEN(val) BSP_FLD32(val, 5, 6)
#define UARTLCRH_FEN BSP_BIT32(4)
#define UARTLCRH_STP2 BSP_BIT32(3)
#define UARTLCRH_EPS BSP_BIT32(2)
#define UARTLCRH_PEN BSP_BIT32(1)
#define UARTLCRH_BRK BSP_BIT32(0)
  uint32_t lcrh;

#define UARTCTL_RXE BSP_BIT32(9)
#define UARTCTL_TXE BSP_BIT32(8)
#define UARTCTL_LBE BSP_BIT32(7)
#define UARTCTL_SIRLP BSP_BIT32(2)
#define UARTCTL_SIREN BSP_BIT32(1)
#define UARTCTL_UARTEN BSP_BIT32(0)
  uint32_t ctl;

#define UARTIFLS_RXIFLSEL(val) BSP_FLD32(val, 3, 5)
#define UARTIFLS_TXIFLSEL(val) BSP_FLD32(val, 0, 2)
  uint32_t ifls;

#define UARTI_OE BSP_BIT32(10)
#define UARTI_BE BSP_BIT32(9)
#define UARTI_PE BSP_BIT32(8)
#define UARTI_FE BSP_BIT32(7)
#define UARTI_RT BSP_BIT32(6)
#define UARTI_TX BSP_BIT32(5)
#define UARTI_RX BSP_BIT32(4)
  uint32_t im;
  uint32_t ris;
  uint32_t mis;
  uint32_t icr;
#if LM3S69XX_HAS_UDMA
  uint32_t dmactl;
#endif
} lm3s69xx_uart;

typedef struct {
#define SSICR0_SCR(val) BSP_FLD32(val, 8, 15)
#define SSICR0_SPH BSP_BIT32(7)
#define SSICR0_SPO BSP_BIT32(6)
#define SSICR0_FRF(val) BSP_FLD32(val, 4, 5)
#define SSICR0_DSS(val) BSP_FLD32(val, 0, 3)
  uint32_t cr0;

#define SSICR1_SOD BSP_BIT32(3)
#define SSICR1_MS BSP_BIT32(2)
#define SSICR1_SSE BSP_BIT32(1)
#define SSICR1_LBM BSP_BIT32(0)
  uint32_t cr1;
  uint32_t dr;

#define SSISR_BSY BSP_BIT32(4)
#define SSISR_RFF BSP_BIT32(3)
#define SSISR_RNE BSP_BIT32(2)
#define SSISR_TNF BSP_BIT32(1)
#define SSISR_TFE BSP_BIT32(0)
  uint32_t sr;

#define SSI_CPSRDIV(val) BSP_FLD32(val, 0, 7)
  uint32_t cpsr;

#define SSII_TX BSP_BIT32(3)
#define SSII_RX BSP_BIT32(2)
#define SSII_RT BSP_BIT32(1)
#define SSII_ROR BSP_BIT32(0)
  uint32_t im;
  uint32_t ris;
  uint32_t mis;
  uint32_t icr;

#if LM3S69XX_HAS_UDMA
#define SSIDMACTL_TXDMAE BSP_BIT32(1)
#define SSIDMACTL_RXDMAE BSP_BIT32(0)
  uint32_t dmactl;
#endif /* LM3S69XX_HAS_UDMA */
} lm3s69xx_ssi;

#endif /* LIBBSP_ARM_LM3S69XX_LM3S69XX_H */
