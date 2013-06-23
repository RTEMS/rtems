/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#include <rtems/framebuffer.h>
#include <rtems/fb.h>
#include <rtems/libio.h>

#include <bsp.h>
#include <bsp/arm-pl111-fb.h>

typedef struct {
  rtems_id semaphore;
  void *frame_buffer;
} pl111_fb_context;

static pl111_fb_context pl111_fb_instance;

static const uint8_t pl111_bits_per_pixel[] = { 1, 2, 4, 8, 16, 24, 16, 12 };

static uint32_t pl111_fb_get_width(const pl111_fb_config *cfg)
{
  return 16U * (PL111_LCD_TIMING0_PPL_GET(cfg->timing0) + 1U);
}

static uint32_t pl111_fb_get_height(const pl111_fb_config *cfg)
{
  return PL111_LCD_TIMING1_LPP_GET(cfg->timing1) + 1U;
}

static uint32_t pl111_fb_get_bits_per_pixel(const pl111_fb_config *cfg)
{
  return pl111_bits_per_pixel[PL111_LCD_CONTROL_LCD_BPP_GET(cfg->control)];
}

static uint32_t pl111_fb_get_line_length_in_bytes(const pl111_fb_config *cfg)
{
  uint32_t width = pl111_fb_get_width(cfg);
  uint32_t bits_per_pixel = pl111_fb_get_bits_per_pixel(cfg);

  return width * ((bits_per_pixel + 7U) / 8U);
}

static uint32_t pl111_fb_get_frame_buffer_size(const pl111_fb_config *cfg)
{
  uint32_t line_length_in_bytes = pl111_fb_get_line_length_in_bytes(cfg);
  uint32_t height = pl111_fb_get_height(cfg);

  return height * line_length_in_bytes;
}

static void pl111_fb_power_delay(const pl111_fb_config *cfg)
{
  rtems_interval delay = (cfg->power_delay_in_us + 1)
    / rtems_configuration_get_microseconds_per_tick();
  rtems_status_code sc = rtems_task_wake_after(delay);
  assert(sc == RTEMS_SUCCESSFUL);
}

static rtems_status_code pl111_fb_initialize(pl111_fb_context *ctx)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  const pl111_fb_config *cfg = arm_pl111_fb_get_config();

  ctx->frame_buffer = calloc(1, pl111_fb_get_frame_buffer_size(cfg));
  if (ctx->frame_buffer != NULL) {
    volatile pl111 *regs = cfg->regs;

    regs->lcd.upbase = (uint32_t) ctx->frame_buffer;

    regs->lcd.timing0 = cfg->timing0;
    regs->lcd.timing1 = cfg->timing1;
    regs->lcd.timing2 = cfg->timing2;
    regs->lcd.timing3 = cfg->timing3;
    regs->lcd.control = cfg->control;

    arm_pl111_fb_pins_set_up(cfg);

    regs->lcd.control = cfg->control
      | PL111_LCD_CONTROL_LCD_EN;

    pl111_fb_power_delay(cfg);

    regs->lcd.control = cfg->control
      | PL111_LCD_CONTROL_LCD_EN
      | PL111_LCD_CONTROL_LCD_PWR;
  } else {
    sc = RTEMS_NO_MEMORY;
  }

  return sc;
}

static void pl111_fb_destroy(const pl111_fb_context *ctx)
{
  const pl111_fb_config *cfg = arm_pl111_fb_get_config();
  volatile pl111 *regs = cfg->regs;

  free(ctx->frame_buffer);

  regs->lcd.control = cfg->control
    | PL111_LCD_CONTROL_LCD_EN;

  pl111_fb_power_delay(cfg);

  regs->lcd.control = cfg->control;

  arm_pl111_fb_pins_tear_down(cfg);
}

static void pl111_fb_get_fix_screen_info(struct fb_fix_screeninfo *info)
{
  const pl111_fb_config *cfg = arm_pl111_fb_get_config();
  const pl111_fb_context *ctx = &pl111_fb_instance;

  memset(info, 0, sizeof(*info));

  info->smem_start = ctx->frame_buffer;
  info->smem_len = pl111_fb_get_frame_buffer_size(cfg);
  info->type = FB_TYPE_PACKED_PIXELS;
  info->visual = FB_VISUAL_TRUECOLOR;
  info->line_length = pl111_fb_get_line_length_in_bytes(cfg);
}

static void pl111_fb_get_var_screen_info(struct fb_var_screeninfo *info)
{
  const pl111_fb_config *cfg = arm_pl111_fb_get_config();

  memset(info, 0, sizeof(*info));

  info->xres = pl111_fb_get_width(cfg);
  info->yres = pl111_fb_get_height(cfg);
  info->bits_per_pixel = pl111_fb_get_bits_per_pixel(cfg);
}

static void pl111_fb_release(const pl111_fb_context *ctx)
{
  rtems_status_code sc = rtems_semaphore_release(ctx->semaphore);
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal(
      RTEMS_FATAL_SOURCE_BSP_SPECIFIC,
      BSP_ARM_PL111_FATAL_SEM_RELEASE
    );
  }
}

rtems_device_driver frame_buffer_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  pl111_fb_context *ctx = &pl111_fb_instance;

  sc = rtems_io_register_name(FRAMEBUFFER_DEVICE_0_NAME, major, 0);
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal(
      RTEMS_FATAL_SOURCE_BSP_SPECIFIC,
      BSP_ARM_PL111_FATAL_REGISTER_DEV
    );
  }

  sc = rtems_semaphore_create(
    rtems_build_name('F', 'B', ' ', ' '),
    1,
    RTEMS_COUNTING_SEMAPHORE,
    0,
    &ctx->semaphore
  );
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_fatal(
      RTEMS_FATAL_SOURCE_BSP_SPECIFIC,
      BSP_ARM_PL111_FATAL_SEM_CREATE
    );
  }

  return sc;
}

rtems_device_driver frame_buffer_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  pl111_fb_context *ctx = &pl111_fb_instance;

  sc = rtems_semaphore_obtain(
    ctx->semaphore,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  if (sc == RTEMS_SUCCESSFUL) {
    sc = pl111_fb_initialize(ctx);
    if (sc != RTEMS_SUCCESSFUL) {
      pl111_fb_release(ctx);
    }
  }

  return sc;
}

rtems_device_driver frame_buffer_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  const pl111_fb_context *ctx = &pl111_fb_instance;

  pl111_fb_destroy(ctx);
  pl111_fb_release(ctx);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver frame_buffer_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return RTEMS_IO_ERROR;
}

rtems_device_driver frame_buffer_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return RTEMS_IO_ERROR;
}

rtems_device_driver frame_buffer_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_ioctl_args_t *ioctl_arg = arg;
  int eno = 0;

  switch (ioctl_arg->command) {
    case FBIOGET_FSCREENINFO:
      pl111_fb_get_fix_screen_info(ioctl_arg->buffer);
      break;
    case FBIOGET_VSCREENINFO:
      pl111_fb_get_var_screen_info(ioctl_arg->buffer);
      break;
    default:
      eno = EINVAL;
      break;
  }

  if (eno == 0) {
    ioctl_arg->ioctl_return = 0;
  } else {
    ioctl_arg->ioctl_return = -1;
    errno = eno;
  }

  return RTEMS_SUCCESSFUL;
}
