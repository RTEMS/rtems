/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic MPC5200 BSP.
 *
 * This file declares functions to use the slice timer module.
 */

/*
 * Copyright (c) 2005 embedded brains GmbH & Co. KG
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

#ifndef __SLICETIMER_H__
#define __SLICETIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SLT0                          0
#define SLT1                          1

#define SLT0_INT_FREQUENCY            10000
#define SLT1_INT_FREQUENCY            0

#define SLT_CNTRL_RW                  (1 << 26)
#define SLT_CNTRL_INTEN               (1 << 25)
#define SLT_CNTRL_TIMEN               (1 << 24)

#define SLT_TSR_ST                    (1 << 24)

#define SLT_TSR_COUNT(freq)           ((freq) ? ((IPB_CLOCK)/(freq)) : (0xFFFFFF))

rtems_device_driver slt_initialize( rtems_device_major_number,
				                      rtems_device_minor_number,
				                      void *
				                    );

#define SLTIME_DRIVER_TABLE_ENTRY \
  { slt_initialize, NULL, NULL, \
    NULL, NULL, NULL }

void mpc5200_slt_isr(uint32_t);
rtems_isr mpc5200_slt0_isr(rtems_irq_hdl_param);
rtems_isr mpc5200_slt1_isr(rtems_irq_hdl_param);
void mpc5200_init_slt(uint32_t);
void mpc5200_set_slt_count(uint32_t);
void mpc5200_enable_slt_int(uint32_t);
void mpc5200_disable_slt_int(uint32_t);
uint32_t mpc5200_check_slt_status(uint32_t);
/*void sltOn(const rtems_irq_connect_data *);
void sltOff(const rtems_irq_connect_data *);
int sltIsOn(const rtems_irq_connect_data *);*/
void Install_slt(rtems_device_minor_number);

#ifdef __cplusplus
}
#endif

#endif /* __SLICETIMER_H__ */
