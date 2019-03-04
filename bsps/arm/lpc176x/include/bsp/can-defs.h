/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief CAN controller for the lpc176x controller.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 * @author  Daniel Chicco  (daniel.chicco@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LPC176X_TIMER_DEFS_H
#define LPC176X_TIMER_DEFS_H

#include <bsp/common-types.h>
#include <bsp/can.h>

/* CAN ACCEPTANCE FILTER */
#define CAN_ACCEPT_BASE_ADDR 0x4003C000

#define CAN_DRIVER_IS_MINOR_VALID( minor ) ( minor < CAN_DEVICES_NUMBER )
#define CAN_DEFAULT_BAUD_RATE 1000000u
#define CAN_DEFAULT_INTERRUPT_CONFIGURATION 0

#define MAX_NUMBER_OF_CAN_BITS 25u
#define MIN_NUMBER_OF_CAN_BITS 4u
#define CAN_BRP_EXTRA_BIT 1u
#define CAN_TSEG_EXTRA_BITS 3u
#define MAX_TSEG1_TSEG2_BITS 22u

#define CAN_GSR_RBS_MASK 1u
#define CAN_CMR_RRB_MASK 4u

#define CAN_MAXIMUM_DATA_SIZE 8u
#define CAN10_MAXIMUM_ID 0x7FFu

/**
 * @brief The Time segments of a CAN bit.
 */
typedef enum {
  CAN_TSEG1,
  CAN_TSEG2,
  CAN_NUMBER_OF_TSEG,
} can_tseg_number;

#define CAN_BTR_TSEG1_SHIFT 16u
#define CAN_BTR_TSEG2_SHIFT 20u
#define CAN_BTR_SJW_SHIFT 14u
#define CAN_BTR_BRP_SHIFT 0u

#define CAN_BTR_TSEG1_MASK 0x000F0000U
#define CAN_BTR_TSEG2_MASK 0x00700000U
#define CAN_BTR_SJW_MASK 0x0000C000U
#define CAN_BTR_BRP_MASK 0x000003FFU

#define WRONG_BTR_VALUE 0xFFFFFFFF

/**
 * @brief The transmit buffers of the CAN device.
 */
typedef enum {
  CAN_TRANSMIT1,
  CAN_TRANSMIT2,
  CAN_TRANSMIT3,
  CAN_NUMBER_OF_TRANSMIT_BUFFERS
} can_transmit_number;

/**
 * @brief The CAN status and control masks to send a message
 * for each transmit buffer.
 */
typedef struct {
  uint32_t can_status_mask;
  uint32_t not_cc_cmr_value;
} can_transmit_info;

/** @brief Represents the CAN controller registers.*/
typedef struct {
  volatile uint32_t MOD;
  volatile uint32_t CMR;
  volatile uint32_t GSR;
  volatile uint32_t ICR;
  volatile uint32_t IER;
  volatile uint32_t BTR;
  volatile uint32_t EWL;
  volatile uint32_t SR;
  volatile registers_can_message receive;
  volatile registers_can_message transmit[ CAN_NUMBER_OF_TRANSMIT_BUFFERS ];
} can_device;

/**
 * @brief A TX or RX pin for each CAN device .
 */
typedef enum {
  CAN_TX_PIN,
  CAN_RX_PIN,
  NUMBER_OF_CAN_PINS
} can_pin_number;

/**
 * @brief A driver entry for each low level device.
 */
typedef struct {
  can_device *const device;
  const lpc176x_module module;
  const uint32_t pconp_pin;
  const uint32_t pins[ NUMBER_OF_CAN_PINS ];
  const lpc176x_pin_function pinfunction;
} can_driver_entry;

/** @brief Represents the CAN centralized registers. */
typedef struct {
  volatile uint32_t TX_SR;
  volatile uint32_t RX_SR;
  volatile uint32_t MSR;
} can_central;

/** @brief Represents the acceptance filter registers. */
typedef struct {
  volatile uint32_t AFMR;
  volatile uint32_t SFF_SA;
  volatile uint32_t SFF_GRP_SA;
  volatile uint32_t EFF_SA;
  volatile uint32_t EFF_GRP_SA;
  volatile uint32_t EOT;
  volatile uint32_t LUT_ERR_ADR;
  volatile uint32_t LUT_ERR;
  volatile uint32_t FCANIE;
  volatile uint32_t FCANIC0;
  volatile uint32_t FCANIC1;
} can_acceptance_filter;

/**
 * @brief The possible CAN formats for a message.
 */
typedef enum {
  CANStandard = 0,
  CANExtended = 1,
  CANAny = 2
} can_format;

/**
 * @brief The types of message.
 */
typedef enum {
  CANData = 0,
  CANRemote = 1
} can_type;

#define CAN_INTERRUPT_TYPE_MASK 0x1ffu

/**
 * @brief The vector with all the callbacks for the CAN isr.
 */
typedef lpc176x_can_isr lpc176x_can_isr_vector[ CAN_IRQ_NUMBER ];

#define CAN_MOD_RM 0x00000001U

#define CAN_ACCF_AFMR_ACCOF 0x00000001U
#define CAN_ACCF_AFMR_ACCBP 0x00000002U
#define CAN_ACCF_AFMR_EFCAN 0x00000004U

#define CAN_IER_RIE 0x000000001U
#define CAN_IER_TIE1 0x000000002U
#define CAN_IER_EIE 0x000000004U
#define CAN_IER_DOIE 0x000000008U
#define CAN_IER_WUIE 0x000000010U
#define CAN_IER_EPIEX 0x000000020U
#define CAN_IER_ALIEX 0x000000040U
#define CAN_IER_BEIEX 0x000000080U
#define CAN_IER_IDIEX 0x000000100U
#define CAN_IER_TIE2 0x000000200U
#define CAN_IER_TIE3 0x000000400U

#endif /*LPC176X_TIMER_DEFS_H*/
