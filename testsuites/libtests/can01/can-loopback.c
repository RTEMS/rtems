/**
 * @file
 *
 * @ingroup CANBus
 *
 * @brief Controller Area Network (CAN) loopback device Implementation
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

struct can_loopback_priv {
  struct can_bus *bus;
};

static bool can_loopback_tx_ready(void *data);
static void can_loopback_int(void *data, bool flag);
static int can_loopback_tx(void *data, struct can_msg *msg);
int can_loopback_init(const char *can_dev_file);

static struct can_dev_ops dev_ops = {
  .dev_tx       = can_loopback_tx,
  .dev_tx_ready = can_loopback_tx_ready,
  .dev_int      = can_loopback_int,
};

static bool can_loopback_tx_ready(void *data)
{
  return true;
}

static void can_loopback_int(void *data, bool flag)
{
  return;
}

static int can_loopback_tx(void *data, struct can_msg *msg)
{
  struct can_loopback_priv *priv = data;

  can_receive(priv->bus, msg);

  return RTEMS_SUCCESSFUL;
}

int can_loopback_init(const char *can_dev_file)
{
  int ret;
  struct can_loopback_priv *priv = NULL;

  struct can_bus *bus = can_bus_alloc_and_init(sizeof(struct can_bus));
  if (bus == NULL) {
    CAN_ERR("can_loopback_init: can_bus_alloc_and_init failed\n");
    return RTEMS_NO_MEMORY;
  }

  priv = (struct can_loopback_priv *)calloc(1, sizeof(struct can_loopback_priv));
  if (priv == NULL) {
    CAN_ERR("can_loopback_init: calloc failed\n");
    ret = RTEMS_NO_MEMORY;
    goto free_bus_return;
  }

  priv->bus = bus;
  bus->priv = priv;

  priv->bus->can_dev_ops = &dev_ops;

  if ((ret = can_bus_register(bus, can_dev_file)) != RTEMS_SUCCESSFUL) {
    CAN_ERR("can_loopback_init: bus register failed\n");
    goto free_priv_return;
  }

  CAN_DEBUG("can_loopback_init: can_loopback driver registered\n");

  return ret;

free_priv_return:
  free(priv);

free_bus_return:
  free(bus);

  return ret;
}
