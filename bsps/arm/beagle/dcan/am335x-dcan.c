/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CANBus
 *
 * @brief Controller Area Network (CAN) Bus Implementation
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

#include <dev/can/can.h>

#include <bsp/am335x_dcan.h>
#include <bsp/soc_AM335x.h>

#define IF_NAME_SIZE_MAX  (12)

void beagle_can_init(void *node)
{
  /* FIXME: Remove this method, Check if the device node belongs to CAN
   * Will be removed once device tree support is added
   */
  static int init = 0;

  if (init != 0) {
    return;
  }

  init = 1;

  CAN_DEBUG("beagle_can_init\n");

  char if_name[IF_NAME_SIZE_MAX];

  struct am335x_dcan_priv *priv = NULL;

  /* FIXME: Get hardware specific information from device tree */
  struct am335x_dcan_irq dcan_irq_nums[2] = {{52, 53, 54}, {55, 56, 57}};
  uint32_t dcan_base_reg_addr[] = {SOC_DCAN_0_REGS, SOC_DCAN_1_REGS};

  for (int i = 0; i < CAN_NODES; i++) {
    priv = (struct am335x_dcan_priv *)calloc(1, sizeof(struct am335x_dcan_priv));

    if (priv == NULL) {
      CAN_ERR("beagle_can_init: calloc failed: cannot allocate memory\n");
      return;
    }

    /* FIXME: Get hardware specific information from device tree */
    priv->node = i;
    priv->base_reg = dcan_base_reg_addr[i];
    priv->irq = dcan_irq_nums[i];
    priv->baudrate = 1000000;

    if (dcan_init(priv) < 0) {
      CAN_ERR("beagle_can_init: CAN controller %d initialization failed\n", priv->node);
      free(priv);
      continue;
    }

    struct can_bus *bus = can_bus_alloc_and_init(sizeof(struct can_bus));

    priv->bus = bus;

    snprintf(if_name, IF_NAME_SIZE_MAX, "/dev/can%d", i);

    bus->priv = priv;
    dcan_init_ops(priv);

    if (can_bus_register(bus, if_name) != 0) {
      CAN_ERR("beagle_can_init: bus register failed\n");
      free(priv);
      return;
    }

    CAN_DEBUG("beagle_can_init: can_bus_registered %s\n", if_name);
  }
}
