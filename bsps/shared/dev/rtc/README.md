Real Time Clock
===============

+ It would be nice to utilize the interrupt capabilities of some
  RTC parts.  This could be used to trigger checking the software
  clock against the hardware clock.

+ The periodic capability of most RTCs is not suitable for use
  as a general purpose flexible clock tick source.  For example,
  many RTCs generate only a handful of periods with 100 Hz being the
  most frequent. 

+ The tick field is not set on get.  Anything smaller than a second
  is ignored on set and get operations.

+ Day of week is ignored since RTEMS does not set it internally.

+ There is no attempt in RTEMS to know about time zones.


Harris ICM7170
--------------

+ Tested on a DMV177.

+ Interrupt capabilities are ignored.


Mostek 48T08
------------

+ Untested.

+ NVRAM is ignored.


ds1643
------
The Mostek M48T08 is compatible with the Dallas Semiconductor DS1643.  Please
use that driver.


icm7170 Configuration Table Use
--------------------------------
sDeviceName

   The name of this device.

deviceType

   This field must be RTC_ICM7170.

pDeviceFns

   The device interface control table.  This must be icm7170_fns.

deviceProbe

   This is the address of the routine which probes to see if the device
   is present.

pDeviceParams

    This field specifies the clock frequency.  It may be one of the
    following:
        ICM7170_AT_32_KHZ
        ICM7170_AT_1_MHZ
        ICM7170_AT_2_MHZ
        ICM7170_AT_4_MHZ

ulCtrlPort1

   This field is the base address of the RTC area of the chip.

ulCtrlPort2

   This field is ignored.

ulDataPort

   This field is ignored.


getRegister
setRegister

   These follow standard conventions.


m48t08 Configuration Table Use
------------------------------

sDeviceName

   The name of this device.

deviceType

   This field must be RTC_M48T08.

pDeviceFns

   The device interface control table.  This must be m48t08_fns.

deviceProbe

   This is the address of the routine which probes to see if the device
   is present.

pDeviceParams

    This is ignored and should be NULL.

ulCtrlPort1

   This field is the base address of the RTC area of the chip.  The
   NVRAM portion of the chip is ignored.

ulCtrlPort2

   This field is ignored.

ulDataPort

   This field is ignored.


getRegister
setRegister

   These follow standard conventions.


m48t18
------
This is supported by the m48t08 driver.


mc146818a
---------
This is supported by the mc146818a driver.
