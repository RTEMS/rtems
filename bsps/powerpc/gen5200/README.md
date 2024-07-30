gen5200
=======
```
BSP NAME:           gen5200
BOARD:              various boards based on MPC5200 Controller:
                    MicroSys PM520 with Carrier board CR825
BUS:                N/A
CPU FAMILY:         ppc
CPU:                PowerPC MPC5200
COPROCESSORS:       Hardware FPU 
MODE:               32 bit mode, I and D cache enabled
DEBUG MONITOR:      None
```

PERIPHERALS
-----------
```
TIMERS:             GPT
SERIAL PORTS:       3 PSCs                     
                    2 CAN IFs
                    1 I2C IF
REAL-TIME CLOCK:    PCF8563
DMA:                for Ethernet and CompactFlash
VIDEO:              none
SCSI:               none
IDE:                1 CompactFlash Slot supported
NETWORKING:         1 FEC Fast Ethernet
```

DRIVER INFORMATION
------------------
```
CLOCK DRIVER:       using one GPT
IOSUPP DRIVER:      none
SHMSUPP:            none
TIMER DRIVER:       Timebase register (lower 32 bits only)
```

STDIO
-----
```
PORT:               PSC1
ELECTRICAL:         RS-232
BAUD:               9600
BITS PER CHARACTER: 8
PARITY:             None
STOP BITS:          1
```

NOTES
-----
```
On-chip resources:
	PSC1		/dev/console /dev/tty00
	PSC2		/dev/tty01
	PSC3		/dev/tty02
```

Board description
-----------------
```
Clock rate:	external clock: 33MHz 
Bus width:	32 bit Flash, 32 bit SDRAM
FLASH:		8MByte
RAM:		64MByte SDRAM 
```

Debugging/ Code loading:
------------------------
Tested using the Lauterbach TRACE32 ICD debugger.


IceCube
-------
IceCube is the nickname for the FreeScale MPC5200LITE evaluation board
which seems to be the basis for boards from a number of other vendors.
The most complete and up to date information will be found on the
RTEMS Wiki.  We know of the following boards which are the IceCube:

 + FreeScale MPC5200LITE
 + Embedded Planets EP52000 (does not ship with U-Boot)

U-Boot supports this board very well. When using U-Boot the following
command sequence is used to transform an ELF file into a U-Boot image.

powerpc-rtems4.8-objcopy -R -S -O binary hello.exe hello.bin
cat hello.bin | gzip -9 >hello.gz
/opt/embedded/tools/usr/bin/mkimage \
  -A ppc -O rtems -T kernel -a 0x10000 -e 0x10000 -n "RTEMS" \
  -d hello.gz hello.img 

These ttcp results were between an EP5200 and Dell Insprion 9400
running Fedora 7.  A private network was used.

```shell
>>> ttcp -t -s 192.168.1.210
ttcp-t: buflen=8192, nbuf=2048, align=16384/0, port=5001  tcp  -> 192.168.1.210
ttcp-t: socket
ttcp-t: connect
ttcp-t: 16777216 bytes in 1.58 real seconds = 10385.86 KB/sec +++
ttcp-t: 2048 I/O calls, msec/call = 0.79, calls/sec = 1298.23
ttcp-t: 0.0user 1.5sys 0:01real 100% 0i+0d 0maxrss 0+0pf 0+0csw
>>> ttcp -r -s
ttcp-r: buflen=8192, nbuf=2048, align=16384/0, port=5001  tcp
ttcp-r: socket
ttcp-r: accept from 192.168.1.210
ttcp-r: 16777216 bytes in 1.78 real seconds = 9194.86 KB/sec +++
ttcp-r: 3499 I/O calls, msec/call = 0.52, calls/sec = 1963.67
```
