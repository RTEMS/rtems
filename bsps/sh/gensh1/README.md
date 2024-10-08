gensh1
======
Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)

```
BSP NAME:           generic SH1 (gensh1)
BOARD:              n/a
BUS:                n/a
CPU FAMILY:         Hitachi SH
CPU:                SH 7032
COPROCESSORS:	    none
MODE:               n/a

DEBUG MONITOR:      gdb
```

PERIPHERALS
-----------
```
TIMERS:             on-chip
  RESOLUTION:         cf. Hitachi SH 703X Hardware Manual (Phi/4)
SERIAL PORTS:       on-chip (with 2 ports)
REAL-TIME CLOCK:    none
DMA:                not used
VIDEO:              none
SCSI:               none
NETWORKING:         none
```

DRIVER INFORMATION
------------------
```
CLOCK DRIVER:       on-chip timer
IOSUPP DRIVER:      default
SHMSUPP:            default
TIMER DRIVER:       on-chip timer
TTY DRIVER:         /dev/null (stub)
```

STDIO
-----
```
PORT:               /dev/null (stub)
ELECTRICAL:         n/a
BAUD:               n/a
BITS PER CHARACTER: n/a
PARITY:             n/a
STOP BITS:          n/a
```

NOTES
-----
(1) The stub console driver (null) is enabled by default.

(2) The driver for the on-chip serial devices (sci) is still in its infancy
    and not fully tested. It may even be non-functional and therefore is
    disabled by default. Please let us know any problems you encounter with
    it.
    To activate it edit libbsp/sh/gensh1/include/bsp.h
