/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *
 * Copyright (C) 2024 Kevin Kirspel
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

#ifndef LIBBSP_NIOSV_GENERIC_H
#define LIBBSP_NIOSV_GENERIC_H

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>

#include <bspopts.h>
#include <bsp/default-initial-extension.h>
#include <altera_avalon_hbus_regs.h>
#include <altera_avalon_jtag_uart_regs.h>
#include <altera_avalon_pio_regs.h>
#include <altera_avalon_sysid_qsys_regs.h>
#include <altera_avalon_timer_regs.h>
#include <altera_avalon_epcq_regs.h>
#include <status_led_device_driver.h>
#include <system_device_driver.h>

#include <rtems/devnull.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_FEATURE_IRQ_EXTENSION

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* LIBBSP_NIOSV_GENERIC_H */
