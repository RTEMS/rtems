rtl22xx
=======

RTEMS BSP for Philips's ARM processor

This BSP is designed for the following Philips' ARM microprocessors:

+ LPC2210
+ LPC2212
+ LPC2214
+ LPC2290
+ LPC2294

Some LPC21xx ARM should also be able to use this BSP.

Philphs's LPC22xx ARM processor has an ARM7TDMI-S core, and can
run at 60MHz. It has an external memory bus, and peripherals like
UART, I2C, SPI, ADC and etc. Some of them have on chip flash (256k)
and CAN.  The board used to develop the BSP is compatible with
LPC-E2214/LPC-E2294 boards from http://www.olimex.com. The board has
a 512K SRAM (256K used to store the .text for debugging purposes)
and two serial ports.

The license and distribution terms for this file may be found in
the file LICENSE in this distribution or at
http://www.rtems.org/license/LICENSE
