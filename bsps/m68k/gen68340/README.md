gen68340
========

This package requires a version of GCC that supports the `-mcpu32` option.

Please send any comments, improvements, or bug reports to:
Geoffroy Montel
g_montel@yahoo.com

This board support package works both MC68340 and MC68349 systems.

Special console features:
- support of polled and interrupts mode (both MC68340 and MC68349)
- support of FIFO FULL mode (only for MC68340, the MC68349 doesn't have any timer, so
  you may write your own timer driver if you have an external one)
 
The type of the board is automatically recognised in the initialization sequence.

WARNING: there's still no network driver!
 I hope it will come in the next RTEMS version!

```
BSP NAME:	    gen68340
BOARD:              Generic 68360 as described in Motorola MC68340 User's Manual
BOARD:              Home made MC68340 board
BOARD:		    Home made MC68349 board
BUS:                none
CPU FAMILY:         Motorola CPU32
COPROCESSORS:       none
MODE:               not applicable

DEBUG MONITOR:      none (Hardware provides BDM)
DEBUG SETUP:	    EST Vision Ice


```
PERIPHERALS
-----------
```
TIMERS:             two timers
  RESOLUTION:       one microsecond
SERIAL PORTS:       2 channel on the UART
REAL-TIME CLOCK:    yes
DMA:                yes
VIDEO:              none
SCSI:               none
NETWORKING:         Ethernet on SCC1.
```


DRIVER INFORMATION
------------------
```
CLOCK DRIVER:       
IOSUPP DRIVER:      
SHMSUPP:            none
TIMER DRIVER:       Timer 1 for timing test suites
		    Timer 2 for console's FIFO FULL mode
```


STDIO
-----
```
PORT:               1
ELECTRICAL:         
BAUD:               9600
BITS PER CHARACTER: 8
PARITY:             None
STOP BITS:          1
```


Board description
-----------------
```
clock rate:	25 MHz
bus width:	16-bit PROM, 32-bit DRAM
ROM:		To 1 MByte, 60 nsec (0 wait states), chip select 0
RAM:		1 to 16 MByte DRAM SIMM, 60 nsec (0 wait states), parity or nonparity
```

Host System
-----------
Cygwin 32

Verification (Standalone 68360)
-------------------------------
Single processor tests:  Passed
Multi-processor tests:  not applicable
