/**
 * @file
 *
 * @ingroup raspberrypi_fb
 *
 * @brief framebuffer support and setup
 *
 */

/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_FB_H
#define LIBBSP_ARM_RASPBERRYPI_FB_H

#include <rtems/fb.h>

/**
 * @defgroup raspberrypi_fb
 *
 * @ingroup RTEMSBSPsARMRaspberryPi
 *
 * @brief Framebuffer support and setup
 *
 * @{
 */

enum rpi_fb_init_result {
  RPI_FB_INIT_OK = 0,
  RPI_FB_INIT_ALREADY_INITIALIZED = 1,
  RPI_FB_INIT_GENERIC_ERROR = -1,
  RPI_FB_INIT_NO_DISPLAY = -2,
  RPI_FB_INIT_CMDLINE_BAD_FORMAT = -3,
  RPI_FB_INIT_CMDLINE_DONT_INIT = -4,
  RPI_FB_INIT_CMDLINE_NO_MODE_REQ = -5,
  RPI_FB_INIT_MODE_PROBE_ERROR = -6,
  RPI_FB_INIT_SETUP_FAILED = -7,
  RPI_FB_INIT_START_ADDR_UNKNOWN = -8,
};

int rpi_fb_init( void );
int rpi_get_fix_screen_info( struct fb_fix_screeninfo *info );
int rpi_get_var_screen_info( struct fb_var_screeninfo *info );
bool rpi_fb_hdmi_is_present( void );

/** @} */

#endif /* LIBBSP_ARM_RASPBERRYPI_FB_H */
