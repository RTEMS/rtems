/**
 * @file
 *
 * @brief Network Driver
 *
 * This driver is a wrapper for the DWMAC 1000 driver from libchip.
 * The DWMAC 1000 driver is an on-chip Synopsys IP Ethernet controllers
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */


#include <rtems.h>

#if defined(RTEMS_NETWORKING)

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <rtems.h>
#include <rtems/config.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>
#include <bsp/hwlib.h>
#include <bsp/alt_clock_manager.h>
#include <bsp/alt_generalpurpose_io.h>
#include <bsp/nocache-heap.h>
#include "socal/alt_rstmgr.h"
#include "socal/alt_sysmgr.h"
#include "socal/hps.h"
#include "socal/socal.h"
#include <libchip/dwmac.h>

/** @brief Pin mask for the interrupt from the ethernet PHY */
#define NETWORK_PIN_MASK_PHY_INTERRUPT 0x00000040U

/** @brief Ethernet PHY status */
typedef enum {
  /** @brief The ethernet PHY is off */
  NETWORK_IF_PHY_STATUS_OFF,

  /** @brief The ethernet PHY is on an initialized, but stopped */
  NETWORK_IF_PHY_STATUS_STOPPED,

  /** @brief The ethernet PHY is started and will generate requested events */
  NETWORK_IF_PHY_STATUS_STARTED,

  /** @brief Number of ethernet PHY statuses */
  NETWORK_IF_PHY_STATUS_COUNT
} network_if_phy_status;

/** @brief Phy context
 *
 * ethernet PHY device context
 */
typedef struct {
  /** @brief PHY device status */
  network_if_phy_status status;

  /** @brief Interrupt mask which corresponds to currently requested events */
  uint16_t irq_mask;
} network_if_phy_context;

/** @brief Phy context initializer
 *
 * @retval Initialized network PHY context
 */
#define NETWORK_IF_PHY_CONTEXT_INITIALIZER( \
    ) \
  { \
    NETWORK_IF_PHY_STATUS_OFF, \
    0 \
  }

/** @brief Network interface controller ID
 *
 * Identifies the network interface controller handled by the driver
 */
typedef enum {
  /** @brief ID for EMAC0 */
  NETWORK_IF_NIC_ID_EMAC0,

  /** @brief ID for EMAC1 */
  NETWORK_IF_NIC_ID_EMAC1,

  /** @brief Number of network interface controller IDs */
  NETWORK_IF_NIC_ID_COUNT
} network_if_nic_id;

/** @brief Network interface context
 *
 * Context for the micro controller specific part of the DWMAC 1000 n
 * etwork driver
 */
typedef struct {
  /** @brief Network interface controller ID */
  network_if_nic_id nic_id;

  /** @brief Driver context
   *
   * Address of the context of the low level DWMAC 1000 driver from libchip */
  void *driver_context;

  /** @brief Ethernet PHY context */
  network_if_phy_context phy;

  /* TODO: Create network context on micro controller level */
} network_if_context;

/** @brief Network interface context initializer
 *
 * Initializer for the context of a network interface
 * @param nic_id          ID of the network interface controller
 * @param driver_context Address of the context of the low level DWMAC 1000
 *                       driver from libchip
 * @retval Initialized network interface context
 */
#define NETWORK_IF_CONTEXT_INITIALIZER( \
    nic_id, \
    driver_context \
    ) \
  { \
    nic_id, \
    driver_context, \
    NETWORK_IF_PHY_CONTEXT_INITIALIZER() \
  }

/******************************************************************************
* Helper Methods
******************************************************************************/

/** @brief Altera status code to errno
 *
 * Convert Altera status code to errno error number
 * @param status  Altera status code as returned by methods from Alteras HWLib
 * @retval Converted errno error number from errno.h
 */
static int network_if_altera_status_code_to_errno( ALT_STATUS_CODE status )
{
  int eno = 0;

  switch ( status ) {
    /*!
     * Indicates a FALSE condition.
     */
    case ALT_E_FALSE:

    /*!
     * Indicates a TRUE condition.
     */
    case ALT_E_TRUE:

      /*! The operation was successful. */
      /* case ALT_E_SUCCESS: */
      eno = 0;
      break;

    /*! The operation failed. */
    case ALT_E_ERROR:

    /*! An invalid option was selected. */
    case ALT_E_INV_OPTION:
      eno = EIO; /* I/O error */
      break;

    /*! FPGA configuration error detected.*/
    case ALT_E_FPGA_CFG:
      eno = ECANCELED; /* Operation canceled */
      break;

    /*! FPGA CRC error detected. */
    case ALT_E_FPGA_CRC:

    /*! An error occurred on the FPGA configuration bitstream input source. */
    case ALT_E_FPGA_CFG_STM:
      eno = EPROTO; /* Protocol error */
      break;

    /*! The FPGA is powered off. */
    case ALT_E_FPGA_PWR_OFF:
      eno = ENODEV; /* No such device */
      break;

    /*! The SoC does not currently control the FPGA. */
    case ALT_E_FPGA_NO_SOC_CTRL:

    /*! The FPGA is not in USER mode. */
    case ALT_E_FPGA_NOT_USER_MODE:
      eno = EPERM; /* Not super-user */
      break;

    /*! An argument violates a range constraint. */
    case ALT_E_ARG_RANGE:

    /*! A bad argument value was passed. */
    case ALT_E_BAD_ARG:

    /*! The argument value is reserved or unavailable. */
    case ALT_E_RESERVED:
      eno = EINVAL; /* Invalid argument */
      break;

    /*! The operation is invalid or illegal. */
    case ALT_E_BAD_OPERATION:

    /*! The version ID is invalid. */
    case ALT_E_BAD_VERSION:
      eno = EFAULT; /* Bad address */
      break;

    /*! An operation or response timeout period expired. */
    case ALT_E_TMO:

    /*! A clock is not enabled or violates an operational constraint. */
    case ALT_E_BAD_CLK:
      eno = ETIME; /* Timer expired */
      break;

    /*! The buffer does not contain enough free space for the operation. */
    case ALT_E_BUF_OVF:
      eno = ENOMEM; /* Not enough core */
      break;
    default:

      /* Unknown case. Implement it! */
      assert( 0 == 1 );
      eno = EIO;
      break;
  }

  return eno;
}

/** @brief PHY check chip ID
 *
 * Check the chip-ID of the KSZ8842 ethernet PHY.
 * @param context Address of the network context
 * @retval  0 if chip could be identified, error code from errno.h otherwise.
 */
static int network_if_phy_check_chip_id( network_if_context *context )
{
  int      eno;
  uint16_t reg;

  eno = dwmac_if_read_from_phy(
    context->driver_context,
    2,
    &reg
    );

  if ( eno == 0 ) {
    if ( reg != 0x0022 ) { /* PHY ID part 1 */
      eno = ENXIO;
    } else {
      eno = dwmac_if_read_from_phy(
        context->driver_context,
        3,
        &reg
        );

      if ( eno == 0 ) {
        /* PHY ID part 2  and
         * Manufacturers model number */
        if ( ( ( reg & 0xFC00 ) >> 10 ) != 0x0005 ) { 
          eno = ENXIO;
        } else if ( ( ( reg & 0x03F0 ) >> 4 ) != 0x0021 ) {
          eno = ENXIO;
        }
      }
    }
  }

  return eno;
}

/******************************************************************************
* Callback Methods
******************************************************************************/

/** @brief NIC enable
 *
 * Enables (powers up) the network interface card.
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_nic_enable( void *arg )
{
  int                 eno               = 0;
  network_if_context *context           = (network_if_context *) arg;
  ALT_CLK_t           module_clk        = ALT_CLK_EMAC0;
  uint32_t            permodrst_clr_msk = ALT_RSTMGR_PERMODRST_EMAC0_CLR_MSK;

  switch ( context->nic_id ) {
    case NETWORK_IF_NIC_ID_EMAC0:
    {
      module_clk        = ALT_CLK_EMAC0;
      permodrst_clr_msk = ALT_RSTMGR_PERMODRST_EMAC0_CLR_MSK;
    }
    break;
    case NETWORK_IF_NIC_ID_EMAC1:
    {
      module_clk        = ALT_CLK_EMAC1;
      permodrst_clr_msk = ALT_RSTMGR_PERMODRST_EMAC1_CLR_MSK;
    }
    break;

    case NETWORK_IF_NIC_ID_COUNT:

      /* Invalid case */
      eno = ENODEV;
      break;
    default:

      /* Unknown case */
      eno = ENOTSUP;
      break;
  }

  /* Pin mux configuration is handled by the Preloader. Thus no
   * pin mux configuration here. */

  /* Enable the clock for the EMAC module */
  if ( eno == 0 ) {
    if ( ALT_E_FALSE == alt_clk_is_enabled( module_clk ) ) {
      ALT_STATUS_CODE status = alt_clk_clock_enable( module_clk );
      eno = network_if_altera_status_code_to_errno( status );
    }
  }

  /* Finish reset for the EMAC module */
  if ( eno == 0 ) {
    uint32_t reg = alt_read_word( ALT_RSTMGR_PERMODRST_ADDR );
    alt_write_word( ALT_RSTMGR_PERMODRST_ADDR, reg & permodrst_clr_msk );
  }

  return eno;
}

/** @brief NIC disable.
 *
 * Disables (powers down) the network interface card.
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_nic_disable( void *arg )
{
  int                 eno               = 0;
  network_if_context *context           = (network_if_context *) arg;
  ALT_CLK_t           module_clk        = ALT_CLK_EMAC0;
  uint32_t            permodrst_set_msk = ALT_RSTMGR_PERMODRST_EMAC0_SET_MSK;

  switch ( context->nic_id ) {
    case NETWORK_IF_NIC_ID_EMAC0:
      module_clk        = ALT_CLK_EMAC0;
      permodrst_set_msk = ALT_RSTMGR_PERMODRST_EMAC0_SET_MSK;
      break;
    case NETWORK_IF_NIC_ID_EMAC1:
      module_clk        = ALT_CLK_EMAC1;
      permodrst_set_msk = ALT_RSTMGR_PERMODRST_EMAC1_SET_MSK;
      break;

    case NETWORK_IF_NIC_ID_COUNT:

      /* Invalid case */
      eno = ENODEV;
      break;
    default:

      /* Unknown case */
      eno = ENOTSUP;
      break;
  }

  /* Enter rest status for the EMAC module */
  if ( eno == 0 ) {
    uint32_t reg = alt_read_word( ALT_RSTMGR_PERMODRST_ADDR );
    alt_write_word( ALT_RSTMGR_PERMODRST_ADDR, reg | permodrst_set_msk );
  }

  /* Disable the clock for the EMAC module */
  if ( eno == 0 ) {
    if ( ALT_E_TRUE == alt_clk_is_enabled( module_clk ) ) {
      ALT_STATUS_CODE status = alt_clk_clock_disable( module_clk );
      eno = network_if_altera_status_code_to_errno( status );
    }
  }

  return eno;
}

/** @brief PHY disable.
 *
 * Disables (powers down) the network PHY.
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_phy_disable( void *arg )
{
  int                 eno  = 0;
  network_if_context *self = (network_if_context *) arg;
  uint32_t            mask;
  ALT_STATUS_CODE     status_code;

  /* The power pin of the PHY is hard wire to board power control and
   * the PHY supports interrupts, This means we can not power it down
   * and we don't need to stop a timer for polling PHY events. */

  status_code = alt_gpio_port_int_mask_set(
    ALT_GPIO_PORTB,
    NETWORK_PIN_MASK_PHY_INTERRUPT,
    NETWORK_PIN_MASK_PHY_INTERRUPT
    );
  eno = network_if_altera_status_code_to_errno( status_code );

  if ( eno == 0 ) {
    mask        = alt_gpio_port_int_enable_get( ALT_GPIO_PORTB );
    status_code = alt_gpio_port_int_enable(
      ALT_GPIO_PORTB,
      mask & ~NETWORK_PIN_MASK_PHY_INTERRUPT
      );
    eno = network_if_altera_status_code_to_errno( status_code );
  }

  if ( eno == 0 ) {
    status_code = alt_gpio_port_int_status_clear(
      ALT_GPIO_PORTB,
      NETWORK_PIN_MASK_PHY_INTERRUPT
      );
    eno = network_if_altera_status_code_to_errno( status_code );
  }

  if ( eno == 0 ) {
    self->phy.status = NETWORK_IF_PHY_STATUS_OFF;
  }

  return eno;
}

/** @brief PHY enable.
 *
 * Enables (powers up) the network PHY.
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_phy_enable( void *arg )
{
  int                 eno  = 0;
  network_if_context *self = (network_if_context *) arg;
  ALT_STATUS_CODE     status_code;

  /* The power pin of the PHY is hard wire to board power control and
   * the PHY supports interrupts, This means we can not power it up
   * and we don't need to start a timer for polling PHY events. */

  eno = network_if_phy_check_chip_id( self );

  if ( eno == 0 ) {
    /* The phy is already enabled and we will reset it */
    eno = network_if_phy_disable( self );

    if ( eno == 0 ) {
      rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 100 );
    }
  } else {
    eno = 0;
  }

  status_code = alt_gpio_port_datadir_set(
    ALT_GPIO_PORTB,
    NETWORK_PIN_MASK_PHY_INTERRUPT,
    0
    );
  eno = network_if_altera_status_code_to_errno( status_code );

  if ( eno == 0 ) {
    status_code = alt_gpio_port_int_type_set(
      ALT_GPIO_PORTB,
      NETWORK_PIN_MASK_PHY_INTERRUPT,
      0
      );
    eno = network_if_altera_status_code_to_errno( status_code );
  }

  if ( eno == 0 ) {
    status_code = alt_gpio_port_int_pol_set(
      ALT_GPIO_PORTB,
      NETWORK_PIN_MASK_PHY_INTERRUPT,
      1
      );
    eno = network_if_altera_status_code_to_errno( status_code );
  }

  if ( eno == 0 ) {
    status_code = alt_gpio_port_debounce_set(
      ALT_GPIO_PORTB,
      NETWORK_PIN_MASK_PHY_INTERRUPT,
      0
      );
    eno = network_if_altera_status_code_to_errno( status_code );
  }

  if ( eno == 0 ) {
    status_code = alt_gpio_port_int_enable(
      ALT_GPIO_PORTB,
      NETWORK_PIN_MASK_PHY_INTERRUPT
      );
    eno = network_if_altera_status_code_to_errno( status_code );
  }

  if ( eno == 0 ) {
    status_code = alt_gpio_port_int_mask_set(
      ALT_GPIO_PORTB,
      NETWORK_PIN_MASK_PHY_INTERRUPT,
      0
      );
    eno = network_if_altera_status_code_to_errno( status_code );
  }

  if ( eno == 0 ) {
    status_code = alt_gpio_port_int_status_clear(
      ALT_GPIO_PORTB,
      NETWORK_PIN_MASK_PHY_INTERRUPT
      );
    eno = network_if_altera_status_code_to_errno( status_code );
  }

  if ( eno == 0 ) {
    self->phy.status = NETWORK_IF_PHY_STATUS_STOPPED;
  }

  return eno;
}

/** @brief Clear phy event status.
 *
 * Clears all PHY event statuses.
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_phy_event_status_clear( void *arg )
{
  int             eno;
  ALT_STATUS_CODE status_code;

  (void) arg;

  /* Clear the interrupt status */
  status_code = alt_gpio_port_int_status_clear(
    ALT_GPIO_PORTB,
    NETWORK_PIN_MASK_PHY_INTERRUPT
    );
  eno = network_if_altera_status_code_to_errno( status_code );

  if ( eno == 0 ) {
    /* Unmask the interrupt (was masked within interrupt handler) */
    status_code = alt_gpio_port_int_mask_set(
      ALT_GPIO_PORTB,
      NETWORK_PIN_MASK_PHY_INTERRUPT,
      0
      );
    eno = network_if_altera_status_code_to_errno( status_code );
  }

  return eno;
}

/**
 * @brief Get the PHY event status.
 *
 * Reports status on PHY events (e.g. PHY interrupts).
 * @param[in]  arg        The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @param[out] event_set  Pointer to a buffer for a set of events for which a
 *                        PHY status change was detected.
 *                        For events see @see dwmac_phy_event.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_phy_events_status_get(
  void            *arg,
  dwmac_phy_event *event_set )
{
  int                 eno  = 0;
  uint16_t            reg  = 0;
  network_if_context *self = (network_if_context *) arg;

  eno = dwmac_if_read_from_phy(
    self->driver_context,
    0x1B,
    &reg
    );

  if ( eno == 0 ) {
    if ( ( reg & 0x80 ) != 0 ) {
      *event_set |= PHY_EVENT_JABBER;
    }

    if ( ( reg & 0x40 ) != 0 ) {
      *event_set |= PHY_EVENT_RECEIVE_ERROR;
    }

    if ( ( reg & 0x20 ) != 0 ) {
      *event_set |= PHY_EVENT_PAGE_RECEIVE;
    }

    if ( ( reg & 0x10 ) != 0 ) {
      *event_set |= PHY_EVENT_PARALLEL_DETECT_FAULT;
    }

    if ( ( reg & 0x08 ) != 0 ) {
      *event_set |= PHY_EVENT_LINK_PARTNER_ACK;
    }

    if ( ( reg & 0x04 ) != 0 ) {
      *event_set |= PHY_EVENT_LINK_DOWN;
    }

    if ( ( reg & 0x02 ) != 0 ) {
      *event_set |= PHY_EVENT_REMOTE_FAULT;
    }

    if ( ( reg & 0x01 ) != 0 ) {
      *event_set |= PHY_EVENT_LINK_UP;
    }
  }

  return eno;
}

/** @brief Network PHY interrupt handler
 *
 * Handling method for interrupts from the PHY.
 * @param arg Address of the network interface context
 */
static void network_if_phy_interrupt_handler( void *arg )
{
  int                 eno  = 0;
  network_if_context *self = (network_if_context *) arg;
  uint32_t            reg;

  reg = alt_gpio_port_int_status_get( ALT_GPIO_PORTB );

  if ( reg & NETWORK_PIN_MASK_PHY_INTERRUPT ) {
    /* We have a level interrupt and we expect the driver to do
     * the actual interrupt handling from within a task context.
     * Thus we have to mask the interrupt. A call to
     * network_if_phy_event_status_clear() will unmask it */
    ALT_STATUS_CODE status_code = alt_gpio_port_int_mask_set(
      ALT_GPIO_PORTB,
      NETWORK_PIN_MASK_PHY_INTERRUPT,
      NETWORK_PIN_MASK_PHY_INTERRUPT
      );
    eno = network_if_altera_status_code_to_errno( status_code );

    if ( eno == 0 ) {
      eno = dwmac_if_handle_phy_event(
        self->driver_context
        );
    }
  }

  assert( eno == 0 );
}

/** @brief PHY event enable.
 *
 * Enables generation of events for an event set.
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @param[in] event_set   Set of events. For these events shall get generated
 *                        upon PHY status change.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_phy_event_enable(
  void                 *arg,
  const dwmac_phy_event event_set )
{
  int                   eno        = 0;
  uint16_t              reg        = 0;
  network_if_context   *self       = (network_if_context *) arg;
  network_if_phy_status old_status = self->phy.status;

  if ( arg != NULL ) {
    switch ( self->phy.status ) {
      case NETWORK_IF_PHY_STATUS_OFF:
        break;
      case NETWORK_IF_PHY_STATUS_STOPPED:
        break;
      case NETWORK_IF_PHY_STATUS_STARTED:
      {
        eno = dwmac_if_read_from_phy(
          self->driver_context,
          0x1B,
          &reg
          );

        if ( eno == 0 ) {
          /* Disable all interrupts, but leave interrupt status 
           * bits untouched */
          reg &= 0x00FF;
          eno  = dwmac_if_write_to_phy(
            self->driver_context,
            0x1B,
            reg
            );
        }

        if ( eno == 0 ) {
          self->phy.irq_mask = 0;
          self->phy.status   = NETWORK_IF_PHY_STATUS_STOPPED;
        }
      }
      break;
      case NETWORK_IF_PHY_STATUS_COUNT:
      {
        /* Invalid case */
        assert( self->phy.status != NETWORK_IF_PHY_STATUS_COUNT );
        eno = ENOTSUP;
      }
      break;
      default:
      {
        /* Unknown case */
        assert(
          self->phy.status == NETWORK_IF_PHY_STATUS_OFF
          || self->phy.status == NETWORK_IF_PHY_STATUS_STOPPED
          || self->phy.status == NETWORK_IF_PHY_STATUS_STARTED
          );
        eno = ENOTSUP;
      }
      break;
    }

    if ( eno == 0 ) {
      /* Select interrupts to enable */
      if ( ( event_set & PHY_EVENT_JABBER ) != 0 ) {
        reg |= 0x8000;
      }

      if ( ( event_set & PHY_EVENT_RECEIVE_ERROR ) != 0 ) {
        reg |= 0x4000;
      }

      if ( ( event_set & PHY_EVENT_PAGE_RECEIVE ) != 0 ) {
        reg |= 0x2000;
      }

      if ( ( event_set & PHY_EVENT_PARALLEL_DETECT_FAULT ) != 0 ) {
        reg |= 0x1000;
      }

      if ( ( event_set & PHY_EVENT_LINK_PARTNER_ACK ) != 0 ) {
        reg |= 0x0800;
      }

      if ( ( event_set & PHY_EVENT_LINK_DOWN ) != 0 ) {
        reg |= 0x0400;
      }

      if ( ( event_set & PHY_EVENT_REMOTE_FAULT ) != 0 ) {
        reg |= 0x0200;
      }

      if ( ( event_set & PHY_EVENT_LINK_UP ) != 0 ) {
        reg |= 0x100;
      }

      switch ( old_status ) {
        case NETWORK_IF_PHY_STATUS_OFF:
          break;
        case NETWORK_IF_PHY_STATUS_STOPPED:
          break;
        case NETWORK_IF_PHY_STATUS_STARTED:
        {
          if ( eno == 0 ) {
            /* Modify interrupt enable bits, but leave
             * interrupt status bits untouched */
            eno = dwmac_if_write_to_phy(
              self->driver_context,
              0x1B,
              reg & 0xFF00
              );
          }
        }
        break;
        case NETWORK_IF_PHY_STATUS_COUNT:
        {
          /* Invalid case */
          assert( self->phy.status != NETWORK_IF_PHY_STATUS_COUNT );
          eno = ENOTSUP;
        }
        break;
        default:
        {
          /* Unknown case */
          assert(
            self->phy.status == NETWORK_IF_PHY_STATUS_OFF
            || self->phy.status == NETWORK_IF_PHY_STATUS_STOPPED
            || self->phy.status == NETWORK_IF_PHY_STATUS_STARTED
            );
          eno = ENOTSUP;
        }
        break;
      }

      if ( eno == 0 ) {
        self->phy.irq_mask = reg & 0xFF00;
      }
    }
  }

  return eno;
}

/**
 * @brief Stop the network PHY.
 *
 * Do anything necessary to stop the network PHY (stop generating events, ...).
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_phy_stop( void *arg )
{
  int                 eno  = 0;
  rtems_status_code   sc   = RTEMS_SUCCESSFUL;
  network_if_context *self = (network_if_context *) arg;

  switch ( self->phy.status ) {
    case NETWORK_IF_PHY_STATUS_OFF:
      break;
    case NETWORK_IF_PHY_STATUS_STOPPED:
      break;
    case NETWORK_IF_PHY_STATUS_STARTED:
    {
      /* Remove any selected interrupts */
      eno = dwmac_if_write_to_phy(
        self->driver_context,
        0x1B,
        0
        );

      if ( eno == 0 ) {
        uint16_t reg;

        /* Clear interrupt status bits by reading them */
        eno = dwmac_if_read_from_phy(
          self->driver_context,
          0x1B,
          &reg
          );
      }

      if ( eno == 0 ) {
        self->phy.irq_mask = 0;

        /* Remove interrupt handler */
        sc = rtems_interrupt_handler_remove(
          ALT_INT_INTERRUPT_GPIO1,
          network_if_phy_interrupt_handler,
          self
          );
        eno = rtems_status_code_to_errno( sc );

        if ( eno == 0 ) {
          self->phy.status = NETWORK_IF_PHY_STATUS_STOPPED;
        }
      }
    }
    break;
    case NETWORK_IF_PHY_STATUS_COUNT:
    {
      /* Invalid case */
      assert( self->phy.status != NETWORK_IF_PHY_STATUS_COUNT );
      eno = ENOTSUP;
    }
    break;
    default:
    {
      /* Unknown case */
      assert(
        self->phy.status == NETWORK_IF_PHY_STATUS_OFF
        || self->phy.status == NETWORK_IF_PHY_STATUS_STOPPED
        || self->phy.status == NETWORK_IF_PHY_STATUS_STARTED
        );
      eno = ENOTSUP;
    }
    break;
  }

  return eno;
}

/**
 * @brief Start the network PHY.
 *
 * Do anything necessary to start the network PHY (start generating
 * events, ...).
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_phy_start( void *arg )
{
  int                 eno  = 0;
  rtems_status_code   sc   = RTEMS_SUCCESSFUL;
  network_if_context *self = (network_if_context *) arg;

  switch ( self->phy.status ) {
    case NETWORK_IF_PHY_STATUS_OFF:
    {
      assert( self->phy.status != NETWORK_IF_PHY_STATUS_OFF );
      eno = EHOSTDOWN;
    }
    break;
    case NETWORK_IF_PHY_STATUS_STOPPED:

      /* Disable all interrupts */
      eno = dwmac_if_write_to_phy(
        self->driver_context,
        0x1B,
        0
        );

      if ( eno == 0 ) {
      }

      if ( eno == 0 ) {
        /* Install interrupt handler */
        sc = rtems_interrupt_handler_install(
          ALT_INT_INTERRUPT_GPIO1,
          NULL,
          RTEMS_INTERRUPT_SHARED,
          network_if_phy_interrupt_handler,
          self
          );
        eno = rtems_status_code_to_errno( sc );
      }

      if ( eno == 0 ) {
        /* Enable interrupts */
        eno = dwmac_if_write_to_phy(
          self->driver_context,
          0x1B,
          self->phy.irq_mask
          );
      }

      break;
    case NETWORK_IF_PHY_STATUS_STARTED:
    {
      uint16_t irq_mask = self->phy.irq_mask;

      /* Re-start the phy */
      eno = network_if_phy_stop( self );

      if ( eno == 0 ) {
        self->phy.irq_mask = irq_mask;

        eno                = network_if_phy_start(
          self
          );
      }
    }
    break;

    case NETWORK_IF_PHY_STATUS_COUNT:
    {
      /* Invalid case */
      assert( self->phy.status != NETWORK_IF_PHY_STATUS_COUNT );
      eno = ENOTSUP;
    }
    break;
    default:
    {
      /* Unknown case */
      assert(
        self->phy.status == NETWORK_IF_PHY_STATUS_OFF
        || self->phy.status == NETWORK_IF_PHY_STATUS_STOPPED
        || self->phy.status == NETWORK_IF_PHY_STATUS_STARTED
        );
      eno = ENOTSUP;
    }
    break;
  }

  if ( eno == 0 ) {
    self->phy.status = NETWORK_IF_PHY_STATUS_STARTED;
  }

  return eno;
}

/**
 * @brief Allocate nocache RAM.
 *
 * Allocate uncached RAM.
 * @param[in]  arg        The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @param[out] memory     Pointer of a buffer to write the address of the
 *                        allocated memory to.
 * @param[in]  size       Number of bytes to be allocated
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_mem_alloc_nocache(
  void        *arg,
  void       **memory,
  const size_t size )
{
  int eno = EINVAL;

  (void) arg;

  assert( memory != NULL );

  if ( memory != NULL ) {
    *memory = altera_cyclone_v_nocache_malloc( size );

    if ( *memory != NULL ) {
      eno = 0;
    } else {
      eno = ENOMEM;
    }
  }

  return eno;
}

/**
 * @brief Free nocache RAM.
 *
 * Release uncached RAM.
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @param[in] memory      Pointer to the memory to be freed.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_mem_free_nocache(
  void *arg,
  void *memory )
{
  int eno = EINVAL;

  (void) arg;

  assert( memory != NULL );

  if ( memory != NULL ) {
    altera_cyclone_v_nocache_free( memory );
    eno = 0;
  }

  return eno;
}

/**
 * @brief Bus setup.
 *
 * Callback method for setting up the system bus for the network driver.
 * @param[in] arg         The void pointer argument passed to
 *                        dwmac_network_if_attach_detach.
 * @retval 0 on success, error code from errno.h on failure.
 */
static int network_if_bus_setup( void *arg )
{
  volatile uint32_t reg = alt_read_word( ALT_SYSMGR_EMAC_L3MST_ADDR );

  (void) arg;

  reg &= ALT_SYSMGR_EMAC_L3MST_AWCACHE_1_CLR_MSK;
  reg &= ALT_SYSMGR_EMAC_L3MST_ARCACHE_1_CLR_MSK;
  reg |= ALT_SYSMGR_EMAC_L3MST_AWCACHE_1_SET(
    ALT_SYSMGR_EMAC_L3MST_AWCACHE_1_E_CACHE_WRBACK_ALLOC
  );
  reg |= ALT_SYSMGR_EMAC_L3MST_ARCACHE_1_SET(
    ALT_SYSMGR_EMAC_L3MST_ARCACHE_1_E_CACHE_WRBACK_ALLOC
  );

  alt_write_word( ALT_SYSMGR_EMAC_L3MST_ADDR, reg );

  return 0;
}

/*****************************************************************************
* Context Data
******************************************************************************/

/** @brief Network interface context */
static network_if_context context = NETWORK_IF_CONTEXT_INITIALIZER(

  /** @brief Use EMAC1 */
  NETWORK_IF_NIC_ID_EMAC1,

  /** @brief Address of driver context
   *
   * To be assigned with return value of dwmac_network_if_attach_detach */
  NULL
  );

/** @brief Network driver DMA configuration data */
static const dwmac_dma_cfg NETWORK_DMA_CFG = DWMAC_DMA_CFG_INITIALIZER(

  /** @brief Bus mode burst length */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_64,

  /** @brief Bus mode fixed burst? */
  DWMAC_DMA_CFG_BUS_MODE_BURST_MODE_SINGLE_OR_INCR,

  /** @brief Bus mode mixed burst? */
  DWMAC_DMA_CFG_BUS_MODE_BURST_NOT_MIXED,

  /** @brief Permit AXI burst length of 4 */
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_4_NOT_SUPPORTED,

  /** @brief Permit AXI burst length of 8 */
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_8_NOT_SUPPORTED,

  /** @brief Permit AXI burst length of 16 */
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_16_NOT_SUPPORTED,

  /** @brief Select boundary crossing mode */
  DWMAC_DMA_CFG_AXI_BURST_BOUNDARY_4_KB
  );

/** @brief Network driver configuration data */
static const dwmac_cfg NETWORK_DRIVER_CFG = DWMAC_CFG_INITIALIZER(

  /** @brief GMII clock rate */
  75000000,

  /** @brief Address of the GMAC registers for emac1 */
  ALT_EMAC1_GMACGRP_ADDR,

  /** @brief Address of the DMA registers for emac1 */
  ALT_EMAC1_DMAGRP_ADDR,

  /** @brief Address of the PHY on the mdio bus */
  4,

  /** @brief Bytes per L1 cache line */
  32,

  /** @brief Interrupt number for the EMAC */
  ALT_INT_INTERRUPT_EMAC1_IRQ,

  /** @brief DMA configuration */
  &NETWORK_DMA_CFG,

  /** @brief Callback method for enabling the network controller */
  network_if_nic_enable,

  /** @brief Callback method for disabling the network controller */
  network_if_nic_disable,

  /** @brief Callback method for enabling the PHY */
  network_if_phy_enable,

  /** @brief Callback method for disabling the PHY */
  network_if_phy_disable,

  /** @brief Callback which enables PHY events for forwarding to driver */
  network_if_phy_event_enable,

  /** @brief Callback method for clearing PHY interrupt status */
  network_if_phy_event_status_clear,

  /** @brief Callback method for getting a set of events from the PHY */
  network_if_phy_events_status_get,

  /** @brief Callback method for making the PHY start forwarding events */
  network_if_phy_start,

  /** @brief Callback method for making the PHY stop forwarding events */
  network_if_phy_stop,

  /** @brief Callback method for allocating uncached memory */
  network_if_mem_alloc_nocache,

  /** @brief Callback method for freeing uncached memory */
  network_if_mem_free_nocache,

  /** @brief Callback method for setting up bus upon driver startup */
  network_if_bus_setup,

  /** @brief Operations for the ethernet mac 1000 */
  &DWMAC_1000_ETHERNET_MAC_OPS,

  /** @brief Operations for the enhanced DMA descriptors */
  &DWMAC_DESCRIPTOR_OPS_ENHANCED
  );

/******************************************************************************
* API
******************************************************************************/

/** @brief Network interface attach/detach
 *
 * The standard attach/detach method for network interfaces.
 * This methods will call the DWMAC 1000 attach/detach method and pass
 * configuration data and callback methods into it. Via these  the general
 * DWMAC 1000 driver is able the handle micro controller specific things.
 * NOTE: Detaching is NOT supported!
 * @param config    Configuration parameters from and for the BSD network
 *                  stack.
 * @param attaching True if attaching the driver to the network stack, false if
 *                  detaching.
 * @retval 0 if successful, error code from errno.h if not.
 */
int altera_cyclone_v_network_if_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int                           attaching )
{
  int eno = 0;

  assert( config != NULL );

  /* Network controller initialization */
  context.driver_context = dwmac_network_if_attach_detach(
    config,
    &NETWORK_DRIVER_CFG,
    &context,
    attaching
    );

  if ( context.driver_context == NULL ) {
    eno = EFAULT;
  }

  return eno;
}

#endif /* defined(RTEMS_NETWORKING) */
