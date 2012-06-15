/**
 * @file
 *
 * @brief  The generic libchip serial driver interface
 */


/*
 *  This file contains the TTY driver table definition
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __LIBCHIP_SERIAL_h
#define __LIBCHIP_SERIAL_h

#include <termios.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Types for get and set register routines
 */

/**
 *  @typedef getRegister_f
 *
 *  This type function provides a hook for the bsp specific method
 *  that gets register data from the given port and register. 
 */
typedef uint8_t   (*getRegister_f)(uintptr_t port, uint8_t reg);

/**
 *  @typedef setData_f
 *
 *  This type function provides a hook for the bsp specific method
 *  that sets register data from the given port and register to the
 *  given value. 
 */
typedef void      (*setRegister_f)(uintptr_t port, uint8_t reg, uint8_t  value);

/**
 *  @typedef getData_f
 *
 *  This type function provides a hook for the bsp specific method
 *  that gets data from the specified port. 
 */
typedef uint8_t   (*getData_f)(uintptr_t port);

/**
 *  @typedef setData_f
 *
 *  This type function provides a hook for the bsp specific method
 *  that writes value to the specified port.
 */
typedef void      (*setData_f)(uintptr_t port, uint8_t value);

/**
 *  @typedef _console_fns
 *
 *  This type definition provides a structure of functions each
 *  methood provides an interfce to the serial por to do a specific
 *  function.
 */
typedef struct _console_fns {
  bool    (*deviceProbe)(int minor);
  int     (*deviceFirstOpen)(int major, int minor, void *arg);
  int     (*deviceLastClose)(int major, int minor, void *arg);
  int     (*deviceRead)(int minor);
  ssize_t (*deviceWrite)(int minor, const char *buf, size_t len);
  void    (*deviceInitialize)(int minor);
  void    (*deviceWritePolled)(int minor, char cChar);
  int     (*deviceSetAttributes)(int minor, const struct termios *t);
  bool      deviceOutputUsesInterrupts;
} console_fns;

/**
 *  @typedef _console_flow
 *
 *  This type definition provides a structure of functions
 *  that provide flow control for the transmit buffer.
 */
typedef struct _console_flow {
  int (*deviceStopRemoteTx)(int minor);
  int (*deviceStartRemoteTx)(int minor);
} console_flow;


/**
 * This type defination provides an enumerated type of all
 * supported libchip console drivers.
 */
typedef enum {
  SERIAL_MC68681,              /* Motorola MC68681 or Exar 88681 */
  SERIAL_NS16550,              /* National Semiconductor NS16550 */
  SERIAL_NS16550_WITH_FDR,     /* National Semiconductor NS16550
                                  with Fractional Divider Register (FDR) */
  SERIAL_Z85C30,               /* Zilog Z85C30 */
  SERIAL_CUSTOM                /* BSP specific driver */
} console_devs;

/**
 * This type defination provides an structure that is used to 
 * uniquely identify a specific serial port.
 */
typedef struct _console_tbl {
  /**  This is the name of the device. */
  char          *sDeviceName;
  /** This indicates the chip type.  It is especially important when
   *   multiple devices share the same interrupt vector and must be
   *   distinguished.
   */
  console_devs   deviceType;
  /** pDeviceFns   This is a pointer to the set of driver routines to use. */
  console_fns   *pDeviceFns;
  /** This value is passed to the serial device driver for use.  In termios
   *  itself the number is ignored.
   */
  bool         (*deviceProbe)(int minor);
  /** This is a pointer to the set of flow control routines to
   *  use. Serial device drivers will typically supply RTSCTS
   *  and DTRCTS handshake routines for DCE to DCE communication,
   *  however for DCE to DTE communication, no such routines
   *  should be necessary as RTS will be driven automatically
   *  when the transmitter is active.
   */
  console_flow  *pDeviceFlow;
  /** The high water mark in the input buffer is set to the buffer
   *  size less ulMargin. Once this level is reached, the driver's
   *  flow control routine used to stop the remote transmitter will
   *  be called. This figure should be greater than or equal to
   *  the number of stages of FIFO between the transmitter and
   *  receiver.
   *
   *  @note At the current time, this parameter is hard coded
   *        in termios and this number is ignored.
   */
  uint32_t       ulMargin;
  /** After the high water mark specified by ulMargin has been
   *  reached, the driver's routine to re-start the remote
   *  transmitter will be called once the level in the input
   *  buffer has fallen by ulHysteresis bytes.
   *
   *  @note At the current time, this parameter is hard coded in termios.
   */
  uint32_t       ulHysteresis;
  /** This contains either device specific data or a pointer to a
   *  device specific structure containing additional information
   *  not provided in this table.
   */
  void          *pDeviceParams;
  /** This is the primary control port number for the device. This
   *  may be used to specify different instances of the same device type.
   */
  uint32_t       ulCtrlPort1;
  /** This is the secondary control port number, of use when a given
   *  device has more than one available channel.
   */
  uint32_t       ulCtrlPort2;
  /** This is the port number for the data port of the device */
  uint32_t       ulDataPort;
  /** This is the routine used to read register values. */
  getRegister_f  getRegister;
  /** This is the routine used to write register values. */
  setRegister_f  setRegister;
  /** This is the routine used to read the data register (RX). */
  getData_f      getData;
  /* This is the routine used to write the data register (TX). */
  setData_f      setData;
  /** This is the baud rate clock speed.*/
  uint32_t       ulClock;
  /** This encodes the interrupt vector of the device. */
  unsigned int   ulIntVector;
} console_tbl;

/**
 * This type defination provides data for the console port.
 */
typedef struct _console_data {
  void                   *termios_data;
  volatile bool           bActive;
  /** This field may be used for any purpose required by the driver  */
  void                   *pDeviceContext;
} console_data;

/**
 *  This is a dynamically sized set of tables containing the serial 
 *  port information.
 */
extern console_tbl   **Console_Port_Tbl;
/**
 * This is the number of serial ports defined in the Console_Port_Tbl.
 */
extern unsigned long   Console_Port_Count;

/**
 *  The statically configured serial port information tables which
 *  are used to initially populate the dynamic tables.
 */
extern console_tbl    Console_Configuration_Ports[];
/**
 * The number of serial ports defined in Console_Configuration_Ports 
 * */
extern unsigned long  Console_Configuration_Count;

/**
 *  This is an array of per port information.
 */
extern console_data  *Console_Port_Data;

extern rtems_device_minor_number Console_Port_Minor;

#ifdef __cplusplus
}
#endif

#endif
