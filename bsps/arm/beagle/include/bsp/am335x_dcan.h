/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CANBus
 *
 * @brief Controller Area Network (DCAN) Controller Implementation
 *
 */

/*
 * Copyright (C) 2022 Prashanth S (fishesprashanth@gmail.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
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

#ifndef _BSP_AM335X_CAN_H
#define _BSP_AM335X_CAN_H

#include <stdio.h>

#include <dev/can/can.h>
#include <bsp.h>

#define CAN_NODES 2

#define CAN_NUM_OF_MSG_OBJS (0x40)
#define CAN_RX_MSG_OBJ_NUM (0x20)
#define CAN_TX_MSG_OBJ_NUM (0x20)

#define CAN_EXT_MSG (0x00000001)

#define CAN_TX_MSG_OBJ_START_NUM ((CAN_NUM_OF_MSG_OBJS / 2) + 1)
#define CAN_TX_MSG_OBJ_END_NUM   (CAN_NUM_OF_MSG_OBJS)

#define CAN_RX_MSG_OBJ_START_NUM (1)
#define CAN_RX_MSG_OBJ_END_NUM   (CAN_NUM_OF_MSG_OBJS / 2)

#define DCAN_IFCMD_MSG_NUM_SHIFT  (0)  /* Bits 0-7:  Number of message object in message RAM which is used for data transfer */
#define DCAN_IFCMD_MSG_NUM_MASK   (0xff << DCAN_IFCMD_MSG_NUM_SHIFT)
#define DCAN_IFCMD_MSG_NUM(n)     (((unsigned int)(n) & DCAN_IFCMD_MSG_NUM_MASK) << DCAN_IFCMD_MSG_NUM_SHIFT)

#define CONTROL_STATUS_SYSBOOT1_MASK         (3 << CONTROL_STATUS_SYSBOOT1_SHIFT)
#define CONTROL_STATUS_SYSBOOT1_19p2MHZ      (0 << CONTROL_STATUS_SYSBOOT1_SHIFT)
#define CONTROL_STATUS_SYSBOOT1_24MHZ        (1 << CONTROL_STATUS_SYSBOOT1_SHIFT)
#define CONTROL_STATUS_SYSBOOT1_25MHZ        (2 << CONTROL_STATUS_SYSBOOT1_SHIFT)
#define CONTROL_STATUS_SYSBOOT1_26MHZ        (3 << CONTROL_STATUS_SYSBOOT1_SHIFT)

#define CONFIG_AM335X_CAN_TSEG1 6
#define CONFIG_AM335X_CAN_TSEG2 1
#define CAN_BIT_QUANTA (CONFIG_AM335X_CAN_TSEG1 + CONFIG_AM335X_CAN_TSEG2 + 1)
#define CAN_CLOCK_FREQUENCY (am335x_get_sysclk())

struct am335x_dcan_irq {
  uint32_t dcan_intr0;
  uint32_t dcan_intr1;
  uint32_t dcan_parity;
};

struct am335x_dcan_priv {
  uint32_t node;
  uint32_t base_reg;
  uint32_t baudrate;
	struct can_bus *bus;
  struct am335x_dcan_irq irq;
};

int dcan_init(struct am335x_dcan_priv *dcan_priv);
void beagle_can_init(void *node);

void dcan_init_ops(struct am335x_dcan_priv *priv);

#define can_putreg(priv, off, val) \
                    do {           \
                         REG(priv->base_reg + off) = val; \
                         CAN_DEBUG_REG("%s:%d %08x<-%08x\n", __FILE__, __LINE__, priv->base_reg + off, val); \
                       } while (0)

static inline uint32_t can_getreg(struct am335x_dcan_priv *priv, uint32_t off)
{
  CAN_DEBUG_REG("can_getreg 0x%08x = 0x%08x\n", priv->base_reg + off, 
                                REG(priv->base_reg + off));
  return REG(priv->base_reg + off);
}

#endif /* _BSP_AM335X_CAN_H */
