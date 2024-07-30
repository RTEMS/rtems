psim
====
```
BSP NAME:           psim
BOARD:              PowerPC Simulator in GDB (psim)
BUS:                N/A
CPU FAMILY:         ppc
CPU:                PowerPC 603, 603e, 604
COPROCESSORS:       N/A
MODE:               32 bit mode

DEBUG MONITOR:      BUG mode (emulates Motorola debug monitor)
```

PERIPHERALS
-----------
```
TIMERS:             PPC internal Timebase register
  RESOLUTION:         ???
SERIAL PORTS:       simulated via bug
REAL-TIME CLOCK:    PPC internal Decrementer register
DMA:                none
VIDEO:              none
SCSI:               none
NETWORKING:         none
```

DRIVER INFORMATION
------------------
```
CLOCK DRIVER:       PPC internal
IOSUPP DRIVER:      N/A
SHMSUPP:            N/A
TIMER DRIVER:       PPC internal
TTY DRIVER:         PPC internal
```

STDIO
-----
```
PORT:               Console port 0
ELECTRICAL:         na
BAUD:               na
BITS PER CHARACTER: na
PARITY:             na
STOP BITS:          na
```

Notes
-----
The device tree for this BSP is included with rtems-tools and automated
testing of psim is supported by the RTEMS Tester. See
rtems-tools/tester/rtems/testing/bsps/psim-device-tree for specifics. This
file must be kept in sync with startup/linkcmds and the MMU initialization
in the BSP.

For the multiprocessing tests to run, psim supports an area of UNIX
shared memory and UNIX semaphore mapped into the PowerPC address space.



Vectors
=======
The location of the vectors file object is critical.

From the comments at the head of vectors.s:

  The issue with this file is getting it loaded at the right place.
  The first vector MUST be at address 0x????0100.
  How this is achieved is dependant on the tool chain.

  However the basic mechanism for ELF assemblers is to create a
  section called ".vectors", which will be loaded to an address
  between 0x????0000 and 0x????0100 (inclusive) via a link script.

  The basic mechanism for XCOFF assemblers is to place it in the
  normal text section, and arrange for this file to be located
  at an appropriate position on the linker command line.

  The variable 'PPC_VECTOR_FILE_BASE' must be defined to be the
  offset from 0x????0000 to the first location in the file.  This
  will usually be 0x0000 or 0x0100.

Andrew Bray 18/8/1995
