MCF5206eLITE
============

Copyright (C) 2000,2001 OKTET Ltd., St.-Petersburg, Russia
Author: Victor V. Vengerov <vvv@oktet.ru>

The license and distribution terms for this file may be
found in the file LICENSE in this distribution or at
http://www.rtems.org/license/LICENSE.


This board support package works with MCF5206eLITE evaluation board with
Motorola Coldfire MCF5206e CPU.

Many thanks to Balanced Audio Technology (http://www.balanced.com),
company which donates MCF5206eLITE evaluation board, P&E Coldfire BDM 
interface and provides support for development of this BSP and generic
MCF5206 CPU code.

Decisions made at compile time include:

Decisions to be made a link-edit time are:
    - The size of memory allocator heap. By default, all available
      memory allocated for the heap. To specify amount of memory
      available for heap:
      LDFLAGS += -Wl,--defsym -Wl,HeapSize=xxx

    - The frequency of system clock oscillator. By default, this frequency
      is 54MHz. To select other clock frequency for your application, put
      line like this in application Makefile: 
      LDFLAGS += -qclock=40000000

    - Select between RAM or ROM images. By default, RAM image generated
      which may be loaded starting from address 0x30000000 to the RAM.
      To prepare image intended to be stored in ROM, put the following
      line to the application Makefile:
      LDFLAGS += -qflash

      You may select other memory configuration providing your own
      linker script.

```
BSP NAME:	    mcf5206elite
BOARD:              MCF5206eLITE Evaluation Board
BUS:                none
CPU FAMILY:         Motorola ColdFire
COPROCESSORS:       none
MODE:               not applicable
DEBUG MONITOR:      none (Hardware provides BDM)
```

PERIPHERALS
-----------
```
TIMERS:             PIT, Watchdog(disabled)
  RESOLUTION:       one microsecond
SERIAL PORTS:       2 UART
REAL-TIME CLOCK:    DS1307
NVRAM:              DS1307
DMA:                2 general purpose
VIDEO:              none
SCSI:               none
NETWORKING:         none
I2C BUS:            MCF5206e MBUS module
```

DRIVER INFORMATION
------------------
```
CLOCK DRIVER:       Programmable Interval Timer
IOSUPP DRIVER:      UART 1
SHMSUPP:            none
TIMER DRIVER:       yes
I2C DRIVER:         yes
```

STDIO
-----
```
PORT:               UART 1
ELECTRICAL:         EIA-232
BAUD:               19200
BITS PER CHARACTER: 8
PARITY:             None
STOP BITS:          1
```

Board description
-----------------
clock rate:	54 MHz default (other oscillator can be installed)
bus width:	16-bit PROM, 32-bit external SRAM
ROM:		Flash memory device AM29LV800BB, 1 MByte, 3 wait states, 
                chip select 0
RAM:            Static RAM 2xMCM69F737TQ, 1 MByte, 1 wait state, chip select 2


Host System
-----------
RedHat 6.2 (Linux 2.2.14), RedHat 7.0 (Linux 2.2.17)


Verification
------------
Single processor tests:  passed
Multi-processort tests:  not applicable
Timing tests: passed

