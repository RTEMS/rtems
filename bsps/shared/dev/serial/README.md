serial
======

This is the serial controller portion of the libchip library.  This 
directory contains the source code for reusable console driver
support code.  Each individual driver is configured using the
console_tbl data structure.  This structure is defined and explained
in the console.h file.

The reusable chip drivers do not directly access the serial controller.
They access the registers on the controller via a set of up to four 
functions which are provided by the BSP.  These functins set and get
general registers and data buffers.  Some chips can access the data
buffers as general registers and thus the driver may not require
those interface routines.



Status
======

+ Hardware flow control is not currently supported.  Some of the chip
  drivers (in particular the z8530) have support for hardware flow control
  but this has not been tested in the libchip context.  There will need
  to be a way to totally disabled hardware flow control which is not
  currently in this.

+ "ulClockSpeed" configuration item field to become a pointer to a table
  of chip specific information.  For example, the z8530 should specify
  clock speed and clock divisor setting.

+ A termios structure should be included to specify  the initial settings.
  Right now all drivers default to 9600, 8N1.
 
+ Need to switch to passing pointers rather than a minor number to 
  functions which are strictly internal to each chip driver.  This
  should be a performance win.

+ Need a test which prompts you for termios settings and tests them.  Until
  this happens, testing for the variety of settings possible will be limited.
  This test should be able to test any serial port while prompts come to the
  console.

MC68681
-------

+ Works interrupt and polled.

+ Hardware flow control not included.

NS16650
-------

+ ns16550_set-attributes function is untested.

+ Hardware flow control included but is currently disabled in ISR.

Z85C30
------

+ Works polled and interrupt.

+ Hardware flow control included but is currently disabled in ISR.

+ Needs to support mode where more specific vectors are generated.


Configuration Table Use
=======================

sDeviceName

   The name of this device.

deviceType

   This field must be SERIAL_MC68681.

pDeviceFns

   The device interface control table.  This may be:
      + mc68681_fns for interrupt driven IO
      + mc68681_fns_polled for polled IO

deviceProbe

   This is the address of the routine which probes to see if the device
   is present.

pDeviceFlow

   This field is ignored as hardware flow control is not currently supported.

ulMargin

    This is currently unused.

ulHysteresis

    This is currently unused.

pDeviceParams

    This is set to the default settings.

ulCtrlPort1

   This field is the base address of the entire DUART.

ulCtrlPort2

   This field is the base address of the port specific registers.

ulDataPort

   This field is bit mapped as follows:
     bit 0:  baud rate set a or b
     bit 1-2: BRG selection ("Select Extend bit")

   Note: If both ports on single DUART are not configured for the same
         baud rate set, then unexpected results will occur.

   Note: On the Exar 88c681, if a standard clock of 3.6864 Mhz is used
         and the "Select Extend bit" is 0 (disabled), then the default
         MC68681 baud rate table is selected.

getRegister
setRegister

   These follow standard conventions.

getData
setData

   These are unused since the TX and RX data registers can be accessed
   as regular registers.

ulClock

   This is a pointer to a baud rate mapping table.  If set to
   mc68681_baud_rate_table, then the CSR/ACR/X bit mappings shown
   in the 68681 and 88681 manuals are used.  Otherwise, the board
   specific baud rate mapping is used.

   NULL is not a valid value.

ulIntVector

   This is the interrupt vector number associated with this chip.



Status
======

There are no known problems with this driver.

Configuration Table Use
-----------------------

sDeviceName

   The name of this device.

deviceType

   This field must be SERIAL_NS16550.

pDeviceFns

   The device interface control table.  This may be:
      + ns16550_fns for interrupt driven IO
      + ns16550_fns_polled for polled IO

deviceProbe

   This is the address of the routine which probes to see if the device
   is present.

pDeviceFlow

   This field is ignored as hardware flow control is not currently supported.

ulMargin

    This is currently unused.

ulHysteresis

    This is currently unused.

pDeviceParams

    This is set to the default settings.  At this point, it is the default
    baud rate cast as a (void *).

ulCtrlPort1

   This field is the base address of this port on the UART.

ulCtrlPort2

   This field is unused for the NS16550.

ulDataPort

   This field is the base address of this port on the UART. 

getRegister
setRegister

   These follow standard conventions.

getData
setData

   These are unused since the TX and RX data registers can be accessed
   as regular registers.

ulClock

   This is the clock constant which is divided by the desired baud
   to get the value programmed into the part.  The formula for this
   for 9600 baud is:

      chip_divisor_value = ulClock / 9600.

   NOTE: When ulClock is 0, the correct value for a PC (115,200) is
   used.

ulIntVector

   This is the interrupt vector number associated with this chip.



xr88681
=======
The Exar XR88681 is an enhanced version of the Motorola MC68681 and is
supported by the mc68681 driver.


z85c30
======

Configuration Table Use
-----------------------

sDeviceName

   The name of this device.

deviceType

   This field must be SERIAL_Z85C30.

pDeviceFns

   The device interface control table.  This may be:
      + z85c30_fns for interrupt driven IO
      + z85c30_fns_polled for polled IO

deviceProbe

   This is the address of the routine which probes to see if the device
   is present.

pDeviceFlow

   This field is set to one of the following values:
      + NULL for no hardware flow control
      + z85c30_flow_RTSCTS for RTS/CTS based flow control
      + z85c30_flow_DTRCTS for DTR/CTS based flow control

ulMargin

    This is currently unused.

ulHysteresis

    This is currently unused.

pDeviceParams

    This is set to the default settings.

ulCtrlPort1

   This field is the address of the control register for this port.

ulCtrlPort2

   This field is the address of the control register for chip.

ulDataPort

   This field is the address of the data register for this port.

getRegister
setRegister

   These follow standard conventions.

getData
setData

   These follow standard conventions.

ulClock

   This is the clock speed of the baud rate clock.
   NULL, then the CSR/ACR/X bit mappings shown in the 68681 and 88681
   manuals are used.  Otherwise, the board specific baud rate mapping
   is used.

ulIntVector

   This is the interrupt vector number associated with this chip.
