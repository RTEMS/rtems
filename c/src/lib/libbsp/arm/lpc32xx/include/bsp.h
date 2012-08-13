/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_LPC32XX_BSP_H
#define LIBBSP_ARM_LPC32XX_BSP_H

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#include <bsp/lpc32xx.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

struct rtems_bsdnet_ifconfig;

/**
 * @defgroup lpc32xx LPC32XX Support
 *
 * @ingroup bsp_kit
 *
 * @brief LPC32XX support package.
 *
 * @{
 */

/**
 * @brief Network driver attach and detach function.
 */
int lpc_eth_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

/**
 * @brief Standard network driver attach and detach function.
 */
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH lpc_eth_attach_detach

/**
 * @brief Standard network driver name.
 */
#define RTEMS_BSP_NETWORK_DRIVER_NAME "eth0"

/**
 * @brief Optimized idle task.
 *
 * This idle task sets the power mode to idle.  This causes the processor clock
 * to be stopped, while on-chip peripherals remain active.  Any enabled
 * interrupt from a peripheral or an external interrupt source will cause the
 * processor to resume execution.
 *
 * To enable the idle task use the following in the system configuration:
 *
 * @code
 * #include <bsp.h>
 *
 * #define CONFIGURE_INIT
 *
 * #define CONFIGURE_IDLE_TASK_BODY lpc32xx_idle
 *
 * #include <confdefs.h>
 * @endcode
 */
void *lpc32xx_idle(uintptr_t ignored);

#define LPC32XX_STANDARD_TIMER (&lpc32xx.timer_1)

static inline unsigned lpc32xx_timer(void)
{
  volatile lpc_timer *timer = LPC32XX_STANDARD_TIMER;

  return timer->tc;
}

static inline void lpc32xx_micro_seconds_delay(unsigned us)
{
  unsigned start = lpc32xx_timer();
  unsigned delay = us * (LPC32XX_PERIPH_CLK / 1000000);
  unsigned elapsed = 0;

  do {
    elapsed = lpc32xx_timer() - start;
  } while (elapsed < delay);
}

#if LPC32XX_OSCILLATOR_MAIN == 13000000U
  #define LPC32XX_HCLKPLL_CTRL_INIT_VALUE \
    (HCLK_PLL_POWER | HCLK_PLL_DIRECT | HCLK_PLL_M(16 - 1))
  #define LPC32XX_HCLKDIV_CTRL_INIT_VALUE \
    (HCLK_DIV_HCLK(2 - 1) | HCLK_DIV_PERIPH_CLK(16 - 1) | HCLK_DIV_DDRAM_CLK(0))
#else
  #error "unexpected main oscillator frequency"
#endif

bool lpc32xx_start_pll_setup(
  uint32_t hclkpll_ctrl,
  uint32_t hclkdiv_ctrl,
  bool force
);

uint32_t lpc32xx_sysclk(void);

uint32_t lpc32xx_hclkpll_clk(void);

uint32_t lpc32xx_periph_clk(void);

uint32_t lpc32xx_hclk(void);

uint32_t lpc32xx_arm_clk(void);

uint32_t lpc32xx_ddram_clk(void);

typedef enum {
  LPC32XX_NAND_CONTROLLER_NONE,
  LPC32XX_NAND_CONTROLLER_MLC,
  LPC32XX_NAND_CONTROLLER_SLC
} lpc32xx_nand_controller;

void lpc32xx_select_nand_controller(lpc32xx_nand_controller nand_controller);

void bsp_restart(void *addr);

#define BSP_CONSOLE_UART_BASE LPC32XX_BASE_UART_5

/**
 * @brief Begin of magic zero area.
 *
 * A read from this area returns zero.  Writes have no effect.
 */
extern uint32_t lpc32xx_magic_zero_begin [];

/**
 * @brief End of magic zero area.
 *
 * A read from this area returns zero.  Writes have no effect.
 */
extern uint32_t lpc32xx_magic_zero_end [];

/**
 * @brief Size of magic zero area.
 *
 * A read from this area returns zero.  Writes have no effect.
 */
extern uint32_t lpc32xx_magic_zero_size [];

#ifdef LPC32XX_SCRATCH_AREA_SIZE
  /**
   * @rief Scratch area.
   *
   * The usage is application specific.
   */
  extern uint8_t lpc32xx_scratch_area [LPC32XX_SCRATCH_AREA_SIZE]
    __attribute__((aligned(32)));
#endif

#define LPC32XX_DO_STOP_GPDMA \
  do { \
    if ((LPC32XX_DMACLK_CTRL & 0x1) != 0) { \
      if ((lpc32xx.dma.cfg & DMA_CFG_E) != 0) { \
        int i = 0; \
        for (i = 0; i < 8; ++i) { \
          lpc32xx.dma.channels [i].cfg = 0; \
        } \
        lpc32xx.dma.cfg &= ~DMA_CFG_E; \
      } \
      LPC32XX_DMACLK_CTRL = 0; \
    } \
  } while (0)

#define LPC32XX_DO_STOP_ETHERNET \
  do { \
    if ((LPC32XX_MAC_CLK_CTRL & 0x7) == 0x7) { \
      lpc32xx.eth.command = 0x38; \
      lpc32xx.eth.mac1 = 0xcf00; \
      lpc32xx.eth.mac1 = 0; \
      LPC32XX_MAC_CLK_CTRL = 0; \
    } \
  } while (0)

#define LPC32XX_DO_STOP_USB \
  do { \
    if ((LPC32XX_USB_CTRL & 0x010e8000) != 0) { \
      LPC32XX_OTG_CLK_CTRL = 0; \
      LPC32XX_USB_CTRL = 0x80000; \
    } \
  } while (0)

#define LPC32XX_DO_RESTART(addr) \
  do { \
    ARM_SWITCH_REGISTERS; \
    rtems_interrupt_level level; \
    uint32_t ctrl = 0; \
  \
    rtems_interrupt_disable(level); \
  \
    arm_cp15_data_cache_test_and_clean(); \
    arm_cp15_instruction_cache_invalidate(); \
  \
    ctrl = arm_cp15_get_control(); \
    ctrl &= ~(ARM_CP15_CTRL_I | ARM_CP15_CTRL_C | ARM_CP15_CTRL_M); \
    arm_cp15_set_control(ctrl); \
  \
    __asm__ volatile ( \
      ARM_SWITCH_TO_ARM \
      "mov pc, %[addr]\n" \
      ARM_SWITCH_BACK \
      : ARM_SWITCH_OUTPUT \
      : [addr] "r" (addr) \
    ); \
  } while (0)

/** @} */

/**
 * @defgroup lpc LPC Support
 *
 * @ingroup lpc32xx
 *
 * @brief LPC support package.
 */

#endif /* ASM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_BSP_H */
