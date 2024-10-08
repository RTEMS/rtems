TLL6527M
========

```
BSP NAME:           TLL6527M
BOARD:              TLL6527M 
CPU FAMILY:         Blackfin
CPU:                Blackfin 527 
MODE:               32 bit mode

DEBUG MONITOR:      
SIMULATOR:          
```

PERIPHERALS
-----------

```
TIMERS:             internal
  RESOLUTION:         1 milisecond
SERIAL PORTS:       2 internal UART (polled/interrupt/dma)
REAL-TIME CLOCK:    internal
DMA:                internal
VIDEO:              none
SCSI:               none
NETWORKING:         none
```

DRIVER INFORMATION
------------------
```
CLOCK DRIVER:       internal
TIMER DRIVER:       internal
I2C:
SPI:
PPI:
SPORT:
```

STDIO
-----
```
PORT:               Console port 1
ELECTRICAL:         RS-232
BAUD:               9600
BITS PER CHARACTER: 8
PARITY:             None
STOP BITS:          1
```

NOTES
-----
The TLL56527M board contains analog devices blackfin 527 processor. In addition
to the peripherals provided by bf527 the board has a temprature sensor, 
accelerometer and power module connected via I2C. It also has LCD interface,
Card reader interface.

The analog device bf52X family of processors are different from the bf53x range 
of processors. This port supports the additional features that are not 
supported by the blackfin 53X family of processors.
  
The TLL6527M does not use the interrupt module used by the bfin 53x since it has
an additional system interrupt controller isr registers for additional lines.
On the 53X these line are multiplexed. 
The centralized interrupt handler is implemented to use lookup tables for 
jumping to the user ISR. For more details look at files implemented under 
libcpu/bfin/bf52x/interrupt/*

This port supports only the uart peripheral. The uart is supported via 
polling, DMA, interrupt. The uart file is generic and is common between the 
ports. Under bsp configure.ac files 
* change the CONSOLE_BAUDRATE or to choose among different baudrate.
* Set UART_USE_DMA for UART to use DMA based transfers. In DMA based transfer
  chunk of buffer is transmitted at once and then an interrupt is generated.
* Set CONSOLE_USE_INTERRUPTS to use interrupt based transfers. After every 
  character is transmitted an interrupt is generated. 
* If CONSOLE_USE_INTERRUPTS, UART_USE_DMA are both not set then the port uses
  polling to transmit data over uart. This call is blocking. 

TLL6527 specific file are mentioned below.
c/src/lib/libcpu/bfin/bf52x/*
c/src/lib/libbsp/bfin/TLL6527M/*


The port was compiled using
---------------------------
  1. bfin-rtems4.11-gcc (GCC) 4.5.2 20101216 
              (RTEMS gcc-4.5.2-3.el5/newlib-1.19.0-1.el5)
  2. automake (GNU automake) 1.11.1
  3. autoconf (GNU Autoconf) 2.68


The port was configured using the flags
---------------------------------------
--target=bfin-rtems4.11 --enable-rtemsbsp=TLL6527M --enable-tests=samples 
--disable-posix --disable-itron 
       

ISSUES
------
Could not place code in l1code (SRAM) because it was not being loaded by the 
gnu loaded.
