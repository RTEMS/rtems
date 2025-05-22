gen83xx
=======
```
BSP NAME:           gen83xx
BOARD:              Freescale MPC8349 board MPC8349EAMDS
BUS:                PCI (unused)
CPU FAMILY:         ppc
CPU:                PowerPC e300 (SW compatible to 603e)
COPROCESSORS:       N/A
MODE:               32 bit mode

DEBUG MONITOR:      U-Boot 
```

PERIPHERALS
-----------
```
TIMERS:             PPC internal Timebase register
  RESOLUTION:         ???
SERIAL PORTS:       2 internal PSCs
REAL-TIME CLOCK:    (not yet supported)
DMA:                none
VIDEO:              none
SCSI:               none
NETWORKING:         2xTSEC triple speed ethernet channels
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
PORT:               Console port 1
ELECTRICAL:         na
BAUD:               9600
BITS PER CHARACTER: 8
PARITY:             N
STOP BITS:          1
```

mpc8313erdb
-----------
SPI
---
In master mode SCS (SPI_D) cannot be used as GPIO[31].  Unfortunately this pin
is connected to the SD Card slot. See also [1] SPI 5.

TSEC
----
The interrupt vector values are switched at the IPIC.  See also [1] IPIC 1.

REFERENCES:

[1]	MPC8313ECE Rev. 3, 3/2008: "MPC8313E PowerQUICC(tm) II Pro Integrated Host
	Processor Device Errata"

Example U-Boot Sequence
-----------------------
```
setenv ethact TSEC1
setenv ipaddr 192.168.96.106
setenv serverip 192.168.96.31
tftp 1000000 ticker.img
bootm
```
Making a U-Boot Image
---------------------
```shell
powerpc-rtems4.9-objcopy -O binary ticker.exe ticker.bin
gzip -9 ticker.bin
mkimage -A ppc -O rtems -T kernel -C gzip -a 100 -e 10000 -n "RTEMS
Test" -d ticker.bin.gz ticker.img
```
