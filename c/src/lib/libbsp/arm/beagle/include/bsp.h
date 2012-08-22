/**
 * @file
 *
 * @ingroup beagle
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
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_BEAGLE_BSP_H
#define LIBBSP_ARM_BEAGLE_BSP_H

#include <bspopts.h>

#include <rtems.h>
//#include <rtems/console.h>
//#include <rtems/clockdrv.h>

#include <bsp/beagle.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

struct rtems_bsdnet_ifconfig;

/**
 * @defgroup beagle BEAGLE Support
 *
 * @ingroup bsp_kit
 *
 * @brief BEAGLE support package.
 *
 * @{
 */

/**
 * @brief Network driver attach and detach function.
 */
int beagle_eth_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

/**
 * @brief Standard network driver attach and detach function.
 */
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH beagle_eth_attach_detach

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
 * #define CONFIGURE_IDLE_TASK_BODY beagle_idle
 *
 * #include <confdefs.h>
 * @endcode
 */
void *beagleboard_idle(uintptr_t ignored);

#define BEAGLE_STANDARD_TIMER (&beagle.timer_1)

static inline unsigned beagleboard_timer(void)
{
  volatile beagle_timer *timer = BEAGLE_STANDARD_TIMER;

  return timer->tc;
}

static inline void beagleboard_micro_seconds_delay(unsigned us)
{
  unsigned start = beagleboard_timer();
  unsigned delay = us * (BEAGLE_PERIPH_CLK / 1000000);
  unsigned elapsed = 0;

  do {
    elapsed = beagleboard_timer() - start;
  } while (elapsed < delay);
}

#if BEAGLE_OSCILLATOR_MAIN == 13000000U
  #define BEAGLE_HCLKPLL_CTRL_INIT_VALUE \
    (HCLK_PLL_POWER | HCLK_PLL_DIRECT | HCLK_PLL_M(16 - 1))
  #define BEAGLE_HCLKDIV_CTRL_INIT_VALUE \
    (HCLK_DIV_HCLK(2 - 1) | \
    HCLK_DIV_PERIPH_CLK(16 - 1) | \
    HCLK_DIV_DDRAM_CLK(1))
#else
  #error "unexpected main oscillator frequency"
#endif

bool beagleboard_start_pll_setup(
  uint32_t hclkpll_ctrl,
  uint32_t hclkdiv_ctrl,
  bool force
);

uint32_t beagleboard__sysclk(void);

uint32_t beagleboard_hclkpll_clk(void);

uint32_t beagleboard_periph_clk(void);

uint32_t beagleboard_hclk(void);

uint32_t beagleboard_arm_clk(void);

uint32_t beagleboard_dram_clk(void);

void bsp_restart(void *addr);

#define BSP_CONSOLE_UART_BASE BEAGLE_BASE_UART_5

/**
 * @brief Begin of magic zero area.
 *
 * A read from this area returns zero.  Writes have no effect.
 */
//extern uint32_t beagle_magic_zero_begin [];

/**
 * @brief End of magic zero area.
 *
 * A read from this area returns zero.  Writes have no effect.
 */
//extern uint32_t beagle_magic_zero_end [];

/**
 * @brief Size of magic zero area.
 *
 * A read from this area returns zero.  Writes have no effect.
 */
//extern uint32_t beagle_magic_zero_size [];

#ifdef BEAGLE_SCRATCH_AREA_SIZE
  /**
   * @rief Scratch area.
   *
   * The usage is application specific.
   */
  //extern uint8_t beagle_scratch_area [BEAGLE_SCRATCH_AREA_SIZE];
#endif

#define BEAGLE_DO_STOP_GPDMA \
  do { \
    if ((BEAGLE_DMACLK_CTRL & 0x1) != 0) { \
      if ((beagle.dma.cfg & DMA_CFG_E) != 0) { \
        int i = 0; \
        for (i = 0; i < 8; ++i) { \
          beagle.dma.channels [i].cfg = 0; \
        } \
        beagle.dma.cfg &= ~DMA_CFG_E; \
      } \
      BEAGLE_DMACLK_CTRL = 0; \
    } \
  } while (0)

#define BEAGLE_DO_STOP_ETHERNET \
  do { \
    if ((BEAGLE_MAC_CLK_CTRL & 0x7) == 0x7) { \
      beagle.eth.command = 0x38; \
      beagle.eth.mac1 = 0xcf00; \
      beagle.eth.mac1 = 0; \
      BEAGLE_MAC_CLK_CTRL = 0; \
    } \
  } while (0)

#define BEAGLE_DO_STOP_USB \
  do { \
    if ((BEAGLE_USB_CTRL & 0x010e8000) != 0) { \
      BEAGLE_OTG_CLK_CTRL = 0; \
      BEAGLE_USB_CTRL = 0x80000; \
    } \
  } while (0)

#define BEAGLE_DO_RESTART(addr) \
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
 * @defgroup beagle BEAGLE Support
 *
 * @ingroup beagle
 *
 * @brief BEAGLE support package.
 */

#endif /* ASM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_BSP_H */
