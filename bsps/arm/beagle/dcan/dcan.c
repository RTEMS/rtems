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

#include <bsp.h>
#include <bsp/am335x_dcan.h>
#include <bsp/soc_AM335x.h>
#include <bsp/hw_control_AM335x.h>

#include <bsp/irq.h>
#include <bsp/irq-generic.h>

#include <dev/can/can.h>

#include "hw_dcan.h"
#include "hw_cm_per.h"

/*
 * Pin configurations.
 */
#define AM335x_PIN_IN_OFFSET  (0x138)
#define AM335x_PIN_SLEW_FAST  (0 << 6)
#define AM335x_PIN_SLEW_SLOW  (1 << 6)
#define AM335x_PIN_RX_DISABLE (0 << 5)
#define AM335x_PIN_RX_ENABLE  (1 << 5)
#define AM335x_PIN_PULL_DIS   (1 << 3)
#define AM335x_PIN_PULL_EN    (0 << 3)
#define AM335x_PIN_PULL_UP    ((1 << 4) | AM335x_PIN_PULL_EN)
#define AM335x_PIN_PULL_DOWN  ((0 << 4) | AM335x_PIN_PULL_EN)

#define AM335x_PIN_MODE_0 (0)
#define AM335x_PIN_MODE_1 (1)
#define AM335x_PIN_MODE_2 (2)

static int  dcan_tx(void *, struct can_msg *);
static bool dcan_tx_ready(void *);
static void dcan_int_enable(struct am335x_dcan_priv *);
static void dcan_int_disable(struct am335x_dcan_priv *);
static void dcan_reset(struct am335x_dcan_priv *priv);
static void dcan_clk_config(uint32_t node);
static void dcan_isr(void *data);
static void dcan_int(void *, bool);
static int  dcan_intr_init(struct am335x_dcan_priv *priv);
static void dcan_inval_obj(struct am335x_dcan_priv *priv, uint32_t index);
static void dcan_init_rxobj(struct am335x_dcan_priv *priv);
static void dcan_save_msg(struct am335x_dcan_priv *priv, struct can_msg *msg);
static void dcan_read_obj(struct am335x_dcan_priv *priv, uint32_t index);
static void dcan_bittiming(struct am335x_dcan_priv *priv);
static void am335x_dcan_pinmux(uint32_t index);

/* FIXME: Should be moved to shared beagle */
uint32_t am335x_get_sysclk(void);

static struct can_dev_ops dev_ops = {
  .dev_tx       = dcan_tx,
  .dev_tx_ready = dcan_tx_ready,
  .dev_int      = dcan_int,
};

void dcan_init_ops(struct am335x_dcan_priv *priv)
{
  CAN_DEBUG("dcan_init_ops\n");
  priv->bus->can_dev_ops = &dev_ops;
}

/**
 * @brief Convert Data Length Code (CAN specific) to length
 * of the CAN message.
 * 
 * @param[in] Data Length Code for the CAN message.
 *
 * @retval Corresponding length for the DLC.
 */
static uint16_t can_dlc_to_len(uint16_t dlc)
{
  if (dlc > 8) {
    switch(dlc) {
      case 9: 
        dlc = 12;
      case 10:
        dlc = 16;
      case 11:
        dlc = 20;
      case 12:
        dlc = 24;
      case 13:
        dlc = 32;
      case 14:
        dlc = 48;
      default:
        dlc = 64;
    }
  }
  return dlc;
}
  
/**
 * @brief Convert Length to Data Length Code (CAN specific).
 * 
 * @param[in] Length of the CAN message.
 *
 * @retval Corresponding DLC for the length.
 */
static uint16_t can_len_to_dlc(uint16_t len)
{
  if (len > 8) {
    switch(len) {
      case 12:
        len = 9;
      case 16:
        len = 10;
      case 20:
        len = 11;
      case 24:
        len = 12;
      case 32:
        len = 13;
      case 48:
        len = 14;
      default:
        len = 64;
    }
  }
  return len;
}

/* FIXME: Make DCAN_TXRQ dynamic */
static bool dcan_tx_ready(void *data)
{
  struct am335x_dcan_priv *priv = (struct am335x_dcan_priv *)data;

  CAN_DEBUG("dcan_tx_ready %08x = %08x\n", priv->base_reg + DCAN_TXRQ(1), 
                  can_getreg(priv, DCAN_TXRQ(1)));

  if (can_getreg(priv, DCAN_TXRQ(1)) == 0xffffffff)
  {
    return false;
  }

  return true;
}

static void dcan_inval_obj(struct am335x_dcan_priv *priv, uint32_t index)
{
  while (can_getreg(priv, DCAN_IFCMD(2)) & DCAN_IFCMD_BUSY) {
    /* busy wait */
  }

  can_putreg(priv, DCAN_IFARB(2), 0);

  /* Disable rx and tx interrupts, clear transmit request */

  can_putreg(priv, DCAN_IFMCTL(2), DCAN_IFMCTL_EOB & (~DCAN_IFMCTL_INTPND));
  can_putreg(priv, DCAN_IFCMD(2), DCAN_IFCMD_WR_RD |
             DCAN_IFCMD_CLRINTPND | DCAN_IFCMD_CONTROL | DCAN_IFCMD_ARB |
             DCAN_IFCMD_MSG_NUM(index));
}

static void dcan_init_rxobj(struct am335x_dcan_priv *priv)
{
  while (can_getreg(priv, DCAN_IFCMD(2)) & DCAN_IFCMD_BUSY) {
    /* busy wait */
  }

  can_putreg(priv, DCAN_IFMSK(2), DCAN_IFMSK_MXTD | DCAN_IFMSK_MDIR);
  can_putreg(priv, DCAN_IFMCTL(2), DCAN_IFMCTL_DATALENGTHCODE |
             DCAN_IFMCTL_EOB | DCAN_IFMCTL_RXIE | DCAN_IFMCTL_UMASK);

#ifdef CONFIG_CAN_EXTID
  can_putreg(priv, DCAN_IFARB(2),
             DCAN_IFARB_MSGVAL | DCAN_IFARB_XTD);
#else
  can_putreg(priv, DCAN_IFARB(2), DCAN_IFARB_MSGVAL);
#endif

  for (int i = CAN_RX_MSG_OBJ_START_NUM; i <= CAN_RX_MSG_OBJ_END_NUM; i++)
  {
    while (can_getreg(priv, DCAN_IFCMD(2)) & DCAN_IFCMD_BUSY) {
    /* busy wait */
  }
    can_putreg(priv, DCAN_IFCMD(2),
                 DCAN_IFCMD_WR_RD | DCAN_IFCMD_MASK |
                 DCAN_IFCMD_ARB | DCAN_IFCMD_CONTROL |
                 DCAN_IFCMD_CLRINTPND |
                 DCAN_IFCMD_DATAA | DCAN_IFCMD_DATAB |
                 DCAN_IFCMD_MSG_NUM(i));
  }
}

static void dcan_save_msg(struct am335x_dcan_priv *priv, struct can_msg *msg)
{
  uint32_t regval;

  regval = can_getreg(priv, DCAN_IFARB(2));
  CAN_DEBUG("IFARB = %08X\n", regval);

  /* FIXME: Add extid support in ifdefs */
  msg->id = (regval >> 18) & DCAN_IFARB_MSK;

  /* FIXME: This is to handle RTR feature not implemented yet */
  /* msg->rtr   = ((regval & DCAN_IFARB_DIR) != 0); */

#ifdef CONFIG_CAN_EXTID
  msg->flags |= ((regval & DCAN_IFARB_XTD) != 0) ? DCAN_XTD : 0;
#endif

  regval = can_getreg(priv, DCAN_IFMCTL(2));
  msg->len = can_dlc_to_len((regval >> DCAN_IFMCTL_DATALENGTHCODE_SHIFT) & DCAN_IFMCTL_DATALENGTHCODE);

  ((uint32_t *)msg->data)[0] = can_getreg(priv, DCAN_IFDATA(2));
  ((uint32_t *)msg->data)[1] = can_getreg(priv, DCAN_IFDATB(2));
}

static void dcan_read_obj(struct am335x_dcan_priv *priv, uint32_t index)
{
  while (can_getreg(priv, DCAN_IFCMD(2)) & DCAN_IFCMD_BUSY) {
    /* busy wait */
  }

  can_putreg(priv, DCAN_IFCMD(2),
             DCAN_IFCMD_MASK | DCAN_IFCMD_ARB |
             DCAN_IFCMD_CONTROL | DCAN_IFCMD_CLRINTPND | DCAN_IFCMD_DATAA |
             DCAN_IFCMD_DATAB | DCAN_IFCMD_MSG_NUM(index));
}

static void dcan_isr(void *data)
{
  struct am335x_dcan_priv *priv = data;
  CAN_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> dcan_isr entry node = %d\n", priv->node);

  uint32_t interrupt = can_getreg(priv, DCAN_INT) & DCAN_INT_INT0ID;
  uint32_t stat = can_getreg(priv, DCAN_ES);
  uint32_t regval;

  CAN_DEBUG("DCAN_INT = %08x DCAN_ES = %08x\n", interrupt, stat);

  if (interrupt & 0x8000) {
    /* Clear all warning/error states except RXOK/TXOK */

    regval = can_getreg(priv, DCAN_ES);
    regval &= DCAN_ES_RXOK | DCAN_ES_TXOK;
    can_putreg(priv, DCAN_ES, regval);

    if (stat & (DCAN_ES_BOFF | DCAN_ES_EPASS | DCAN_ES_EWARN)) {
      CAN_ERR("CAN: dcan_isr: Error state\n");
      REG(priv->base_reg + DCAN_CTL) |= (DCAN_CTL_ABO);
      dcan_reset(priv);
    }
  }

  do {
    if (interrupt != 0 && (interrupt & 0x8000) == 0) {
      uint32_t msgindex = interrupt & 0x7fff;
      CAN_DEBUG("msgindex %d\n", msgindex);
  
      /* if no error detected */
      
      if (((stat & DCAN_ES_LEC) == 0) ||
            ((stat & DCAN_ES_LEC) == DCAN_ES_LEC)) { 
        if (msgindex <= CAN_RX_MSG_OBJ_END_NUM) { 
          CAN_DEBUG("rx interrupt msgobj = %u\n", msgindex);
          struct can_msg msg;
  
          regval = can_getreg(priv, DCAN_ES);
          regval &= ~DCAN_ES_RXOK;
          can_putreg(priv, DCAN_ES, regval);
  
          dcan_read_obj(priv, msgindex);
          dcan_save_msg(priv, &msg);
#ifdef CAN_DEBUG_ISR
          can_print_msg(&msg);
#endif /* CAN_DEBUG_ISR */
          dcan_inval_obj(priv, msgindex);
  
          can_receive(priv->bus, &msg);
        } else {
          CAN_DEBUG("tx interrupt msgobj = %u\n", msgindex);
  
          regval = can_getreg(priv, DCAN_ES);
          regval &= ~DCAN_ES_TXOK;
          can_putreg(priv, DCAN_ES, regval);
  
          dcan_inval_obj(priv, msgindex);
        }
      } else {
        dcan_inval_obj(priv, msgindex);
      }
    
      can_putreg(priv, DCAN_ES, 0);
    
      if (msgindex == CAN_RX_MSG_OBJ_END_NUM) {
        dcan_init_rxobj(priv);
      }
    }

    interrupt = can_getreg(priv, DCAN_INT);
    CAN_DEBUG("DCAN_INT = %08x\n", interrupt);
  } while (interrupt != 0);

  can_tx_done(priv->bus);

  CAN_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> dcan_isr exit\n");
}

static void dcan_int_enable(struct am335x_dcan_priv *priv)
{
  CAN_DEBUG("dcan_int_enable\n");
  bsp_interrupt_vector_enable(priv->irq.dcan_intr0);
  bsp_interrupt_vector_enable(priv->irq.dcan_intr1);
  bsp_interrupt_vector_enable(priv->irq.dcan_parity);

  REG(priv->base_reg + DCAN_CTL) |= DCAN_CTL_EIE | DCAN_CTL_SIE;
  REG(priv->base_reg + DCAN_CTL) |= DCAN_CTL_IE1 | DCAN_CTL_IE0;

  CAN_DEBUG("DCAN_CTL = 0x%08X\n", can_getreg(priv, DCAN_CTL));
}

static void dcan_int_disable(struct am335x_dcan_priv *priv)
{
  CAN_DEBUG("dcan_int_disable\n");
  bsp_interrupt_vector_disable(priv->irq.dcan_intr0);
  bsp_interrupt_vector_disable(priv->irq.dcan_intr1);
  bsp_interrupt_vector_disable(priv->irq.dcan_parity);

  REG(priv->base_reg + DCAN_CTL) &= (~(DCAN_CTL_EIE | DCAN_CTL_SIE));
  REG(priv->base_reg + DCAN_CTL) &= (~(DCAN_CTL_IE1 | DCAN_CTL_IE0));

  CAN_DEBUG("DCAN_CTL = 0x%08X\n", can_getreg(priv, DCAN_CTL));
}

static void dcan_int(void *data, bool flag)
{
  if (flag == true) {
    dcan_int_enable(data);
  } else {
    dcan_int_disable(data);
  }
}

static int dcan_tx(void *data, struct can_msg *msg)
{
  uint32_t regval;
  uint32_t num;
  uint32_t id;
  uint32_t dlc;
  uint8_t txobj;

  struct am335x_dcan_priv *priv = (struct am335x_dcan_priv *)data;

  CAN_DEBUG("dcan_tx Entry\n");

  regval = can_getreg(priv, DCAN_TXRQ(1));
  CAN_DEBUG("DCAN_TXRQ = 0x%08X\n", regval);

  for (num = 0; num < 32; num++) {
    if ((regval & (1 << num)) == 0) { 
      break;
    }
  }

  if (num == 32) { 
    return RTEMS_NO_MEMORY;
  }

  txobj = CAN_TX_MSG_OBJ_START_NUM + num;
  if (txobj > CAN_TX_MSG_OBJ_END_NUM) {
    CAN_DEBUG("dcan_tx Calculated txobj num exceeds the CAN_TX_MSG_OBJ_END_NUM\n")
    return RTEMS_INTERNAL_ERROR;
  }
  CAN_DEBUG("msgobj num = %u\n", txobj);

  id = msg->id;
  dlc = can_len_to_dlc(msg->len);

  CAN_DEBUG("CAN-%d ID: %d LEN: %d\n",
          priv->node, msg->id, msg->len);

  /* FIXME: Add support for EXT ID */
  can_putreg(priv, DCAN_IFMSK(1), 0xffff);

  regval = ((dlc & DCAN_IFMCTL_DATALENGTHCODE) |
             DCAN_IFMCTL_EOB | DCAN_IFMCTL_TXRQST |
             DCAN_IFMCTL_TXIE);
  can_putreg(priv, DCAN_IFMCTL(1), regval);

  /* Write data to IF1 data registers */
  regval = msg->data[0] + (msg->data[1] << 8) +
           (msg->data[2] << 16) + (msg->data[3] << 24);
  can_putreg(priv, DCAN_IFDATA(1), regval);

  regval = msg->data[4] + (msg->data[5] << 8) +
           (msg->data[6] << 16) + (msg->data[7] << 24);
  can_putreg(priv, DCAN_IFDATB(1), regval);

#ifdef CONFIG_CAN_EXTID
  can_putreg(priv,
             DCAN_IFARB(1),
             DCAN_IFARB_DIR | DCAN_IFARB_MSGVAL |
             DCAN_IFARB_XTD | (id << DCAN_IFARB_MSK_SHIFT));
#else
  can_putreg(priv,
             DCAN_IFARB(1),
             DCAN_IFARB_DIR | DCAN_IFARB_MSGVAL |
             (id << 18));
#endif

  /* Write to Message RAM */
  regval = (DCAN_IFCMD_WR_RD | DCAN_IFCMD_MASK |
            DCAN_IFCMD_ARB | DCAN_IFCMD_CONTROL |
            DCAN_IFCMD_CLRINTPND | DCAN_IFCMD_TXRQST_NEWDAT |
            DCAN_IFCMD_DATAA | DCAN_IFCMD_DATAB |
            DCAN_IFCMD_MSG_NUM(txobj));
  can_putreg(priv, DCAN_IFCMD(1), regval);

  regval = can_getreg(priv, DCAN_TXRQ(1));

  CAN_DEBUG("msgobj = %u DCAN_TXRQ(1) = 0x%08X\n", txobj, regval);

  CAN_DEBUG("dcan_tx Exit\n");

  return RTEMS_SUCCESSFUL;
}

static void am335x_dcan_pinmux(uint32_t index)
{
  CAN_DEBUG("am335x_dcan_pinmux for node = 0x%08X\n", index);

  /* FIXME: Add a common way of configuring control module */
  if (index == 1) {
    REG(AM335X_PADCONF_BASE + AM335X_CONF_UART1_RXD) = AM335x_PIN_PULL_UP 
        | AM335x_PIN_RX_DISABLE | AM335x_PIN_MODE_2;
    REG(AM335X_PADCONF_BASE + AM335X_CONF_UART1_TXD) = AM335x_PIN_PULL_UP 
        | AM335x_PIN_RX_ENABLE | AM335x_PIN_MODE_2;
  } else if (index == 0) {
    REG(AM335X_PADCONF_BASE + AM335X_CONF_UART1_CTSN) = AM335x_PIN_PULL_UP
        | AM335x_PIN_RX_DISABLE | AM335x_PIN_MODE_2;
    REG(AM335X_PADCONF_BASE + AM335X_CONF_UART1_RTSN) = AM335x_PIN_PULL_UP
        | AM335x_PIN_RX_ENABLE | AM335x_PIN_MODE_2;
  }
}

uint32_t am335x_get_sysclk(void)
{
  uint32_t reg_val = REG(SOC_CONTROL_REGS + CONTROL_STATUS);

  switch(reg_val & CONTROL_STATUS_SYSBOOT1_MASK) {
    case CONTROL_STATUS_SYSBOOT1_19p2MHZ:
      return 19200000;
    case CONTROL_STATUS_SYSBOOT1_24MHZ:
      return 24000000;
    case CONTROL_STATUS_SYSBOOT1_25MHZ:
      return 25000000;
    case CONTROL_STATUS_SYSBOOT1_26MHZ:
      return 26000000;
    default:
      CAN_ERR("DCANGetSysCLK: failed\n");
  }
  return 0;
}

static void dcan_bittiming(struct am335x_dcan_priv *priv)
{
  uint32_t ts1 = CONFIG_AM335X_CAN_TSEG1;
  uint32_t ts2 = CONFIG_AM335X_CAN_TSEG2;
  uint32_t sjw = 1;
  uint32_t brp = CAN_CLOCK_FREQUENCY / (priv->baudrate * CAN_BIT_QUANTA);

  uint32_t regval;

  CAN_DEBUG("CAN%d PCLK: %d baud: %d\n",
         priv->node, CAN_CLOCK_FREQUENCY, priv->baudrate);
  CAN_DEBUG("TS1: %d TS2: %d BRP: %d SJW= %d\n", ts1, ts2, brp, sjw);
  /* Start configuring bit timing */

  regval = REG(priv->base_reg + DCAN_CTL);
  regval |= DCAN_CTL_CCE;
  can_putreg(priv, DCAN_CTL, regval);

  regval = (((brp - 1) << DCAN_BTR_BRP_SHIFT)
         | ((ts1 - 1) << DCAN_BTR_TSEG1_SHIFT)
         | ((ts2 - 1) << DCAN_BTR_TSEG2_SHIFT)
         | ((sjw - 1) << DCAN_BTR_SJW_SHIFT));

  CAN_DEBUG("Setting CANxBTR= 0x%08x\n", regval);

  /* Set bit timing */
  can_putreg(priv, DCAN_BTR, regval);

  /* Stop configuring bit timing */
  regval = can_getreg(priv, DCAN_CTL);
  regval &= ~DCAN_CTL_CCE;
  can_putreg(priv, DCAN_CTL, regval);
}

static int dcan_intr_init(struct am335x_dcan_priv *priv)
{
  int ret;

  if ((ret = rtems_interrupt_handler_install(priv->irq.dcan_intr0, "can-intr-0", 
      RTEMS_INTERRUPT_UNIQUE, dcan_isr, (void *)priv)) != RTEMS_SUCCESSFUL) {
    CAN_ERR("interrupt registration failed irq = %u\n", priv->irq.dcan_intr0);
    return ret;
  }

  CAN_DEBUG("Interrupt registration successful intr 0 = %u\n", priv->irq.dcan_intr0);

  if ((ret = rtems_interrupt_handler_install(priv->irq.dcan_intr1, "can-intr-1", 
      RTEMS_INTERRUPT_UNIQUE, dcan_isr, (void *)priv)) != RTEMS_SUCCESSFUL) {
    CAN_ERR("interrupt registration failed irq = %u\n", priv->irq.dcan_intr1);
    return ret;
  }

  CAN_DEBUG("Interrupt registration successful intr 1 = %u\n", priv->irq.dcan_intr1);

  if ((ret = rtems_interrupt_handler_install(priv->irq.dcan_parity, "can-intr-parity", 
      RTEMS_INTERRUPT_UNIQUE, dcan_isr, (void *)priv)) != RTEMS_SUCCESSFUL) {
    CAN_ERR("interrupt registration failed irq = %u\n", priv->irq.dcan_parity);
    return ret;
  }

  CAN_DEBUG("Interrupt registration successful intr parity = %u\n", priv->irq.dcan_parity);

  return ret;
}

static void dcan_clk_config(uint32_t node)
{ 
  if (node == 1) {
    CAN_DEBUG("CLK for DCAN1\n");
    REG(SOC_CM_PER_REGS + CM_PER_DCAN1_CLKCTRL) =
                                  CM_PER_DCAN1_CLKCTRL_MODULEMODE_ENABLE;

    while((REG(SOC_CM_PER_REGS + CM_PER_DCAN1_CLKCTRL) & 
                         CM_PER_DCAN1_CLKCTRL_MODULEMODE) !=
                         CM_PER_DCAN1_CLKCTRL_MODULEMODE_ENABLE);
  } else if (node == 0) {
    CAN_DEBUG("CLK for DCAN0\n");
    REG(SOC_CM_PER_REGS + CM_PER_DCAN0_CLKCTRL) =
                                  CM_PER_DCAN0_CLKCTRL_MODULEMODE_ENABLE;

    while((REG(SOC_CM_PER_REGS + CM_PER_DCAN0_CLKCTRL) &
                         CM_PER_DCAN0_CLKCTRL_MODULEMODE) !=
                         CM_PER_DCAN0_CLKCTRL_MODULEMODE_ENABLE);
  } else {
    CAN_ERR("dcan_clk_config: unsupported node\n");
  }
} 

int dcan_init(struct am335x_dcan_priv *priv)
{
  uint32_t regval;
  int ret;

  CAN_DEBUG("DCAN Node %d\n", priv->node);

  dcan_clk_config(priv->node);

  am335x_dcan_pinmux(priv->node);

  REG(SOC_CONTROL_REGS + CONTROL_DCAN_RAMINIT) |= 
              CONTROL_DCAN_RAMINIT_DCAN1_RAMINIT_START;
  while ((REG(SOC_CONTROL_REGS + CONTROL_DCAN_RAMINIT) & 
            CONTROL_DCAN_RAMINIT_DCAN1_RAMINIT_START) == 0);

  dcan_reset(priv);

  dcan_bittiming(priv);

#ifdef CAN_DEBUG
  dcan_tx_ready(priv);
#endif /* CAN_DEBUG */

  for (int i = 0; i < CAN_NUM_OF_MSG_OBJS; i++) {
    dcan_inval_obj(priv, i);
  }

#ifdef CAN_DEBUG
  dcan_tx_ready(priv);

  /* FIXME: This is for Enabling loopback and ABO, configure it from ioctl. */
  REG(priv->base_reg + DCAN_CTL) |= DCAN_CTL_ABO | DCAN_CTL_TEST;
#endif /* CAN_DEBUG */

  regval  = can_getreg(priv, DCAN_CTL);
  regval &= ~DCAN_CTL_INIT;
  can_putreg(priv, DCAN_CTL, regval);

  while (can_getreg(priv, DCAN_CTL) & DCAN_CTL_INIT);

/* FIXME: This is for enabling Loopback, should be configured from ioctl */
/*
  CAN_DEBUG("Enabling Loopback\n");
  REG(priv->base_reg + DCAN_CTL) |= DCAN_CTL_TEST;
  REG(priv->base_reg + DCAN_TEST) |= DCAN_TEST_LBACK;
*/

  dcan_init_rxobj(priv);

  if ((ret = dcan_intr_init(priv)) != RTEMS_SUCCESSFUL) {
    return ret;
  }

  regval = can_getreg(priv, DCAN_CTL);
  regval |= (DCAN_CTL_IE0 | DCAN_CTL_SIE | DCAN_CTL_EIE);
  can_putreg(priv, DCAN_CTL, regval);

  return ret;
}

static void dcan_reset(struct am335x_dcan_priv *priv)
{
  can_putreg(priv, DCAN_CTL, DCAN_CTL_INIT | DCAN_CTL_SWR);
  while (can_getreg(priv, DCAN_CTL) & DCAN_CTL_SWR);
}
