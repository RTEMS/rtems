/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#include <bsp/fdt.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>

#include <inttypes.h>
#include <libfdt.h>
#include <stdio.h>

#include <chip.h>
#include <fsl_lpuart.h>

#define LPUART_MAX_INSTANCES 8

#define LPUART_DATA_RT (LPUART_DATA_R0T0_MASK | LPUART_DATA_R1T1_MASK | \
                        LPUART_DATA_R2T2_MASK | LPUART_DATA_R3T3_MASK | \
                        LPUART_DATA_R4T4_MASK | LPUART_DATA_R5T5_MASK | \
                        LPUART_DATA_R6T6_MASK | LPUART_DATA_R7T7_MASK | \
                        LPUART_DATA_R8T8_MASK | LPUART_DATA_R9T9_MASK)

typedef struct {
  rtems_termios_device_context base;
  volatile LPUART_Type *regs;
  rtems_vector_number irq;
  const char *path;
  clock_ip_name_t clock_ip;
  uint32_t src_clock_hz;
  lpuart_config_t config;
} imxrt_lpuart_context;

/* Static memory for the console UART because it might is initialized early. */
static imxrt_lpuart_context imxrt_lpuart_console_instance;
static imxrt_lpuart_context *imxrt_lpuart_console;

static void imxrt_output_char(char c);
static int imxrt_poll_char(void);

static imxrt_lpuart_context *imxrt_lpuart_get_context(
  rtems_termios_device_context *base
)
{
  return RTEMS_CONTAINER_OF(base, imxrt_lpuart_context, base);
}

static void imxrt_lpuart_write_polled(
    rtems_termios_device_context *base,
    char c
)
{
  imxrt_lpuart_context *ctx = imxrt_lpuart_get_context(base);
  volatile LPUART_Type *regs = ctx->regs;
  rtems_interrupt_level isr_cookie;
  uint32_t ctrl;

  rtems_interrupt_disable(isr_cookie);
  ctrl = ctx->regs->CTRL;
  ctx->regs->CTRL = ctrl & ~LPUART_CTRL_TIE_MASK;
  rtems_interrupt_enable(isr_cookie);

  while ((regs->STAT & LPUART_STAT_TDRE_MASK) == 0) {
    /* Wait */
  }

  regs->DATA = c;

  if ((ctrl & LPUART_CTRL_TIE_MASK) != 0) {
    ctx->regs->CTRL |= LPUART_CTRL_TIE_MASK;
  }
}

static int imxrt_lpuart_read_polled(rtems_termios_device_context *base)
{
  uint32_t data;
  imxrt_lpuart_context *ctx = imxrt_lpuart_get_context(base);
  volatile LPUART_Type *regs = ctx->regs;

  data = regs->DATA;

  if ( data & (LPUART_DATA_PARITYE_MASK | LPUART_DATA_FRETSC_MASK |
      LPUART_DATA_RXEMPT_MASK) ) {
    return -1;
  } else {
    return data & LPUART_DATA_RT;
  }
}

static bool imxrt_lpuart_set_attributes(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  imxrt_lpuart_context *ctx = imxrt_lpuart_get_context(base);

  switch (term->c_cflag & CSIZE) {
    case CS7:
      ctx->config.dataBitsCount = kLPUART_SevenDataBits;
      break;
    case CS8:
      ctx->config.dataBitsCount = kLPUART_EightDataBits;
      break;
    default:
      return false;
      break;
  }

  ctx->config.baudRate_Bps = rtems_termios_baud_to_number(term->c_ospeed);

  if ((term->c_cflag & CSTOPB) != 0) {
    ctx->config.stopBitCount = kLPUART_TwoStopBit;
  } else {
    ctx->config.stopBitCount = kLPUART_OneStopBit;
  }

  if ((term->c_cflag & PARENB) != 0) {
    if ((term->c_cflag & PARODD) != 0) {
      ctx->config.parityMode = kLPUART_ParityOdd;
    } else {
      ctx->config.parityMode = kLPUART_ParityEven;
    }
  } else {
    ctx->config.parityMode = kLPUART_ParityDisabled;
  }

  if ((term->c_cflag & CREAD) != 0) {
    ctx->config.enableRx = true;
  } else {
    ctx->config.enableRx = false;
  }

  if ((term->c_cflag & CCTS_OFLOW) != 0) {
    ctx->config.enableTxCTS = true;
  } else {
    ctx->config.enableTxCTS = false;
  }

  if ((term->c_cflag & CRTS_IFLOW) != 0) {
    ctx->config.enableRxRTS = true;
  } else {
    ctx->config.enableRxRTS = false;
  }

  (void) LPUART_Init((LPUART_Type *)ctx->regs, &ctx->config,
      ctx->src_clock_hz, false);

  return true;
}

static uint32_t imxrt_lpuart_get_src_freq(clock_ip_name_t clock_ip)
{
  uint32_t freq;
#if IMXRT_IS_MIMXRT10xx
  uint32_t mux;
  uint32_t divider;

  (void) clock_ip; /* Not necessary for i.MXRT1050 */

  mux = CLOCK_GetMux(kCLOCK_UartMux);
  divider = 1;

  switch (mux) {
  case 0: /* pll3_sw_clk */
    freq = CLOCK_GetFreq(kCLOCK_Usb1PllClk);
    divider = 6;
    break;
  case 1: /* OSC */
    freq = CLOCK_GetFreq(kCLOCK_OscClk);
    break;
  default:
    freq = 0;
  }

  divider *= CLOCK_GetDiv(kCLOCK_UartDiv) + 1U;
  freq /= divider;
#elif IMXRT_IS_MIMXRT11xx
  /*
   * FIXME: A future version of the mcux_sdk might provide a better method to
   * get the clock instead of this hack.
   */
  clock_root_t clock_root = clock_ip + kCLOCK_Root_Lpuart1 - kCLOCK_Lpuart1;

  freq = CLOCK_GetRootClockFreq(clock_root);
#else
  #error Getting UART clock frequency is not implemented for this chip
#endif

  return freq;
}

static clock_ip_name_t imxrt_lpuart_clock_ip(volatile LPUART_Type *regs)
{
  LPUART_Type *const base_addresses[] = LPUART_BASE_PTRS;
  static const clock_ip_name_t lpuart_clocks[] = LPUART_CLOCKS;
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE(base_addresses); ++i) {
    if (base_addresses[i] == regs) {
      return lpuart_clocks[i];
    }
  }

  return kCLOCK_IpInvalid;
}

static void imxrt_lpuart_init_hardware(imxrt_lpuart_context *ctx)
{
  (void) LPUART_Init((LPUART_Type *)ctx->regs, &ctx->config,
      ctx->src_clock_hz, true);
}

#ifdef BSP_CONSOLE_USE_INTERRUPTS
static void imxrt_lpuart_interrupt(void *arg)
{
  rtems_termios_tty *tty = arg;
  rtems_termios_device_context *base = rtems_termios_get_device_context(tty);
  imxrt_lpuart_context *ctx = imxrt_lpuart_get_context(base);
  uint32_t stat;
  uint32_t data;

  stat = ctx->regs->STAT;

  if ((stat & LPUART_STAT_RDRF_MASK) != 0) {
    do {
      char c;
      data = ctx->regs->DATA;

      if ((data & (LPUART_DATA_PARITYE_MASK | LPUART_DATA_FRETSC_MASK |
          LPUART_DATA_RXEMPT_MASK)) == 0) {
        c = data & LPUART_DATA_RT;
        rtems_termios_enqueue_raw_characters(tty, &c, 1);
      }
    } while ((data & LPUART_DATA_RXEMPT_MASK) == 0);
  }

  if ((ctx->regs->CTRL & LPUART_CTRL_TIE_MASK) != 0
      && (stat & LPUART_STAT_TDRE_MASK) != 0) {
    /* Note: This will call imxrt_lpuart_write */
    rtems_termios_dequeue_characters(tty, 1);
  }
}
#endif

static bool imxrt_lpuart_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  rtems_status_code sc;
#endif
  imxrt_lpuart_context *ctx = imxrt_lpuart_get_context(base);
  rtems_termios_set_initial_baud(tty, BSP_CONSOLE_BAUD);

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  sc = rtems_interrupt_handler_install(
    ctx->irq,
    "LPUART",
    RTEMS_INTERRUPT_SHARED,
    imxrt_lpuart_interrupt,
    tty
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }
#endif

  imxrt_lpuart_init_hardware(ctx);

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  ctx->regs->CTRL |= LPUART_CTRL_RIE_MASK;
#endif

  imxrt_lpuart_set_attributes(base, term);

  return true;
}

static void imxrt_lpuart_last_close(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  rtems_libio_open_close_args_t *args
)
{
  imxrt_lpuart_context *ctx = imxrt_lpuart_get_context(base);

  LPUART_Deinit((LPUART_Type *)ctx->regs);

#ifdef BSP_CONSOLE_USE_INTERRUPTS
  (void) rtems_interrupt_handler_remove(
    ctx->irq,
    imxrt_lpuart_interrupt,
    tty
  );
#endif
}

static void imxrt_lpuart_write(
  rtems_termios_device_context *base,
  const char *buf,
  size_t len
)
{
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  imxrt_lpuart_context *ctx = imxrt_lpuart_get_context(base);

  if (len > 0) {
    ctx->regs->DATA = (uint8_t) buf[0];
    ctx->regs->CTRL |= LPUART_CTRL_TIE_MASK;
  } else {
    ctx->regs->CTRL &= ~LPUART_CTRL_TIE_MASK;
  }
#else
  size_t i;

  for (i = 0; i < len; ++i) {
    imxrt_lpuart_write_polled(base, buf[i]);
  }
#endif
}

static const rtems_termios_device_handler imxrt_lpuart_handler = {
  .first_open = imxrt_lpuart_first_open,
  .last_close = imxrt_lpuart_last_close,
  .write = imxrt_lpuart_write,
  .set_attributes = imxrt_lpuart_set_attributes,
#ifdef BSP_CONSOLE_USE_INTERRUPTS
  .mode = TERMIOS_IRQ_DRIVEN,
#else
  .poll_read = imxrt_lpuart_read_polled,
  .mode = TERMIOS_POLLED,
#endif
};

static int imxrt_lpuart_get_stdout_node(const void *fdt)
{
  int node;
  const char *console;

  node = fdt_path_offset(fdt, "/chosen");
  if (node < 0) {
    bsp_fatal(IMXRT_FATAL_NO_CONSOLE);
  }

  console = fdt_getprop(fdt, node, "stdout-path", NULL);
  if (console == NULL) {
    bsp_fatal(IMXRT_FATAL_NO_CONSOLE);
  }

  node = fdt_path_offset(fdt, console);
  if (node < 0) {
    bsp_fatal(IMXRT_FATAL_NO_CONSOLE);
  }

  return node;
}

static void imxrt_lpuart_init_context_from_fdt(
  imxrt_lpuart_context *ctx,
  const void *fdt,
  int node
)
{
  memset(&ctx->base, 0, sizeof(ctx->base));

  ctx->regs = imx_get_reg_of_node(fdt, node);
  if (ctx->regs == NULL) {
    bsp_fatal(IMXRT_FATAL_LPUART_INVALID_FDT);
  }

  ctx->irq = imx_get_irq_of_node(fdt, node, 0);
  if (ctx->irq == BSP_INTERRUPT_VECTOR_INVALID) {
    bsp_fatal(IMXRT_FATAL_LPUART_INVALID_FDT);
  }

  ctx->path = fdt_getprop(fdt, node, "rtems,path", NULL);
  if (ctx->path == NULL) {
    bsp_fatal(IMXRT_FATAL_LPI2C_INVALID_FDT);
  }

  ctx->clock_ip = imxrt_lpuart_clock_ip(ctx->regs);
  ctx->src_clock_hz = imxrt_lpuart_get_src_freq(ctx->clock_ip);

  LPUART_GetDefaultConfig(&ctx->config);
  ctx->config.enableTx = true;
  ctx->config.enableRx = true;

  rtems_termios_device_context_initialize(&ctx->base, "LPUART");
}

static void imxrt_lpuart_console_probe_early(void)
{
  int node;
  const void *fdt;

  imxrt_lpuart_console = NULL;

  fdt = bsp_fdt_get();
  node = imxrt_lpuart_get_stdout_node(fdt);
  imxrt_lpuart_init_context_from_fdt(&imxrt_lpuart_console_instance, fdt, node);
  (void) LPUART_Init(
    (LPUART_Type *)imxrt_lpuart_console_instance.regs,
    &imxrt_lpuart_console_instance.config,
    imxrt_lpuart_console_instance.src_clock_hz,
    true
  );
  imxrt_lpuart_console = &imxrt_lpuart_console_instance;

  BSP_output_char = imxrt_output_char;
  BSP_poll_char = imxrt_poll_char;
}

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  const void *fdt;
  int stdout_node;
  int node;
  rtems_status_code sc;

  fdt = bsp_fdt_get();
  stdout_node = imxrt_lpuart_get_stdout_node(fdt);
  node = -1;

  rtems_termios_initialize();

  do {
    node = fdt_node_offset_by_compatible(fdt, node, "nxp,imxrt-lpuart");

    if (node >= 0 && imxrt_fdt_node_is_enabled(fdt, node)) {
      imxrt_lpuart_context *ctx;

      if (node != stdout_node) {
        ctx = calloc(1, sizeof(imxrt_lpuart_context));
        if (ctx == NULL) {
          bsp_fatal(IMXRT_FATAL_LPUART_ALLOC_FAILED);
        }

        imxrt_lpuart_init_context_from_fdt(ctx, fdt, node);

      } else {
        ctx = imxrt_lpuart_console;
        if (ctx == NULL) {
          imxrt_lpuart_console_probe_early();
          ctx = imxrt_lpuart_console;
        }
      }

      sc = rtems_termios_device_install(
        ctx->path,
        &imxrt_lpuart_handler,
        NULL,
        &ctx->base
      );
      if (sc != RTEMS_SUCCESSFUL) {
        bsp_fatal(IMXRT_FATAL_LPUART_INSTALL_FAILED);
      }

      if (node == stdout_node) {
        link(ctx->path, CONSOLE_DEVICE_NAME);
      }
    }
  } while (node >= 0);

  return RTEMS_SUCCESSFUL;
}

static void imxrt_output_char(char c)
{
  if (imxrt_lpuart_console != NULL) {
    imxrt_lpuart_write_polled(&imxrt_lpuart_console->base, c);
  }
}

static int imxrt_poll_char(void)
{
  return imxrt_lpuart_read_polled(&imxrt_lpuart_console->base);
}

static void imxrt_output_char_init(char c)
{
  imxrt_lpuart_console_probe_early();
  imxrt_output_char(c);
}

BSP_output_char_function_type BSP_output_char = imxrt_output_char_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
