/*
 * Copyright (C) 2026 Kinsey Moore <wkmoore@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/bootcard.h>
#include <bsp/fatal.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>
#include <bsp/utility.h>

#define RTC_CNTL_WDTWPROTECT_REG (uintptr_t) 0xa8
#define RTC_CNTL_WDTWPROTECT_KEY 0x50d83aa1U

#define RTC_CNTL_WDTCONFIG0_REG (uintptr_t) 0x90
#define RTC_CNTL_WDTCONFIG0_EN  BSP_BIT32( 31 )

#define RTC_CNTL_SWD_CONF_REG          (uintptr_t) 0xac
#define RTC_CNTL_SWD_CONF_AUTO_FEED_EN BSP_BIT32( 31 )

#define RTC_CNTL_SWD_WPROTECT_REG (uintptr_t) 0xb0
#define RTC_CNTL_SWD_WPROTECT_KEY 0x8f1d312aU

#define RTC_CNTL_SW_CPU_STALL_REG (uintptr_t) 0xb4

#define RTC_READ( reg )         READ_REG( RTC_CNTL_BASE, reg )
#define RTC_WRITE( reg, value ) WRITE_REG( RTC_CNTL_BASE, reg, value )

#define TIMG_WDTCONFIG0_REG (uintptr_t) 0x48
#define TIMG_WDTCONFIG0_EN  BSP_BIT32( 31 )

#define TIMG_READ( reg )         READ_REG( TIMG_BASE, reg )
#define TIMG_WRITE( reg, value ) WRITE_REG( TIMG_BASE, reg, value )

#define READ_REG( base, reg ) *(volatile uint32_t *) ( base + reg )
#define WRITE_REG( base, reg, value ) \
  *(volatile uint32_t *) ( base + reg ) = value

void bsp_start( void )
{
  uint32_t wdt_config0;
  uint32_t swd_conf;
  uint32_t timg_wdtconfig0;

  /* disable RTC watchdog write protection */
  RTC_WRITE( RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDTWPROTECT_KEY );

  /* disable the RTC watchdog */
  wdt_config0 = RTC_READ( RTC_CNTL_WDTCONFIG0_REG );
  RTC_WRITE( RTC_CNTL_WDTCONFIG0_REG, wdt_config0 & RTC_CNTL_WDTCONFIG0_EN );

  /* disable super watchdog write protection */
  RTC_WRITE( RTC_CNTL_SWD_WPROTECT_REG, RTC_CNTL_SWD_WPROTECT_KEY );

  /* Set SWD auto feed */
  swd_conf = RTC_READ( RTC_CNTL_SWD_CONF_REG );
  RTC_WRITE(
    RTC_CNTL_SWD_CONF_REG,
    swd_conf | RTC_CNTL_SWD_CONF_AUTO_FEED_EN
  );

  /* clear SWD stall register */
  RTC_WRITE( RTC_CNTL_SW_CPU_STALL_REG, 0 );

  /* Disable timergroup 0 watchdog */
  timg_wdtconfig0 = TIMG_READ( TIMG_WDTCONFIG0_REG );
  TIMG_WRITE( TIMG_WDTCONFIG0_REG, timg_wdtconfig0 & TIMG_WDTCONFIG0_EN );

  bsp_interrupt_initialize();
}

/* src is the offset in flash */
BSP_START_TEXT_SECTION static inline void copy_from_flash_offset(
  void       *dest,
  const void *src,
  size_t      n
)
{
  /* The RAM load sections are offset from 0x0, offset from mapped flash base */
  uintptr_t flash_base = 0x3c000000;

  uintptr_t flash_address = ( (uintptr_t) src );
  flash_address += flash_base;
  memcpy( dest, (void *) flash_address, n );
}

BSP_START_TEXT_SECTION void bsp_start_copy_sections( void )
{
  copy_from_flash_offset(
    bsp_section_data_begin,
    bsp_section_data_load_begin,
    (size_t) bsp_section_data_size
  );

  copy_from_flash_offset(
    bsp_section_fast_text_begin,
    bsp_section_fast_text_load_begin,
    (size_t) bsp_section_fast_text_size
  );

  copy_from_flash_offset(
    bsp_section_fast_data_begin,
    bsp_section_fast_data_load_begin,
    (size_t) bsp_section_fast_data_size
  );
}
