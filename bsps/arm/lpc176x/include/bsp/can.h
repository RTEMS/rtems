/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief CAN controller for the mbed lpc1768 board.
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

#ifndef LPC176X_CAN_H
#define LPC176X_CAN_H

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/lpc176x.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief The CAN devices of the board.
 */
typedef enum {
  CAN_0,
  CAN_1,
  CAN_DEVICES_NUMBER
} lpc176x_can_number;

/**
 * @brief  A CAN message represented for the registers of the device.
 */
typedef struct {
  uint32_t info;
  uint32_t id;
  uint32_t data_a;
  uint32_t data_b;
} registers_can_message;

/**
 * @brief A CAN message represented with each logical parts
 */
typedef struct {
  unsigned int reserved1 : 16;
  unsigned int dlc       :  4;   /* Bits 16..19: DLC - Data Length Counter*/
  unsigned int reserved0 : 10;
  unsigned int rtr       :  1;   /* Bit 30: Set if this is a RTR message*/
  unsigned int type      :  1;   /* Bit 31: Set if this is a 29-bit ID message*/
  unsigned int id;               /* CAN Message ID (11-bit or 29-bit)*/
  unsigned char data[ 8 ];       /* CAN Message Data Bytes 0-7*/
} low_level_can_message;

/**
 * @brief A CAN message represented of both forms.
 */
typedef union {
  low_level_can_message low_level;
  registers_can_message registers;
} can_message;

/**
 * @brief The possible interrupt sources for CAN.
 */
typedef enum {
  IRQ_RX = 0,
  IRQ_TX,
  IRQ_ERROR,
  IRQ_OVERRUN,
  IRQ_WAKEUP,
  IRQ_PASSIVE,
  IRQ_ARB,
  IRQ_BUS,
  IRQ_READY,
  CAN_IRQ_NUMBER
} can_irq_type;

/**
 * @brief An isr for a CAN interrupt
 *
 * @param number The CAN which rised the interrupt.
 */
typedef void (*lpc176x_can_isr) ( lpc176x_can_number number );

/**
 * @brief A CAN frequency value
 */
typedef unsigned int can_freq;

/**
 * @brief Opens CAN device.
 * @details It enables the module and gives it a clock, sets the pins,
 * disables the interrupts, sets the frequency and bypasses
 * the acceptance filter.
 *
 * @param  minor The device to open.
 * @param freq The desired frequency.
 * @return RTEMS_SUCCESFUL on success.
 */
rtems_status_code can_open( lpc176x_can_number minor, can_freq freq );

/**
 * @brief Closes the passed CAN device and shut it down.
 *
 * @param minor The device to close.
 * @return RTEMS_SUCCESSFUL  if ok, RTEMS_INVALID_NUMBER for a bad parameter.
 */
rtems_status_code can_close( lpc176x_can_number minor );

/**
 * @brief Reads the CAN device.
 *
 * @param minor The CAN device to read.
 * @param message The read message.
 * @return RTEMS_SUCCESSFUL if read ok, RTEMS_IO_ERROR otherwise.
 */
rtems_status_code can_read(
  const lpc176x_can_number minor,
  can_message             *message
);

/**
 * @brief Writes the passed CAN message into the selected CAN device.
 *
 * @param minor The device to write.
 * @param message The message to write.
 * @return RTEMS_SUCCESFUL if write ok. RTEMS_IO_ERROR otherwise.
 */
rtems_status_code can_write(
  const lpc176x_can_number minor,
  const can_message *const message
);

/**
 * @brief Registers an isr in the driver vector, and enables the interrupt
*  in the device.
 *
 * @param number The CAN device to set
 * @param type The interrupt type.
 * @param isr The isr to register.
 * @return RTEMS_SUCCESSFUL if ok RTEMS_INVALID_NUMBER otherwise.
 */
rtems_status_code can_register_isr(
  const lpc176x_can_number number,
  const can_irq_type       type,
  const lpc176x_can_isr    isr
);

/**
 * @brief Creates a CAN message.
 * @details [long description]
 *
 * @param msg The created message.
 * @param _id The can id for the message.
 * @param _data The data of the message.
 * @param _len The length of the message.
 * @return RTEMS_SUCCESFUL if created, RTEMS_INVALID_NUMBER otherwise.
 */
rtems_status_code create_can_message(
  can_message *const msg,
  const int          _id,
  const char *const  _data,
  const char         _len
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ifndef LPC176X_CAN_H */
