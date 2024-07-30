gensh2
======

 Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 Adapted by: John Mills (jmills@tga.com)
 Corrections: Radzislaw Galler (rgaller@et.put.poznan.pl)


```
BSP NAME:           generic SH2 (gensh2)
BOARD:              EVB7045F (note 2)
BUS:                n/a
CPU FAMILY:         Hitachi SH
CPU:                SH 7045F
COPROCESSORS:	    none
MODE:               n/a

DEBUG MONITOR:      gdb
```

PERIPHERALS
-----------
```
TIMERS:             on-chip
  RESOLUTION:         cf. Hitachi SH 704X Hardware Manual (Phi/16)
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
TTY DRIVER:         /dev/console
```

STDIO
-----
```
PORT:               /dev/sci0
ELECTRICAL:         SCI0
BAUD:               9600
BITS PER CHARACTER: 8
PARITY:             NONE
STOP BITS:          1
```

NOTES
-----
(1) The present 'hw_init.c' file provides 'early_hw_init'(void) which
    is normally called from 'start.S' to provide such minimal HW setup
    as is conveniently written in 'C' and can make use of global
    symbols for 7045F processor elements. It also provides
    'void bsp_hw_init (void)' normally called from 'bspstart.c', shortly
    before RTEMS itself is started.

    These are both minimal functions intended to support the RTEMS test
    suites.

(2) See README.EVB7045F



Getting started with EVB7045F and gensh2
----------------------------------------
Author: Radzislaw Galler (rgaller@et.put.poznan.pl)

This is a capture of success path to put a RTEMS sample
'hello.exe' on the evaluation board EVB7045F.


What you need
-------------
   * Computer with two operating systems: Linux and Wndows 2000 (tm) -
     that was in my case (see section 'Variations')

   * Flash Development Toolkit (FDT) - available on HMSE homepage
     (http://www.hmse.com/products/fdt/support.htm)

   * 'gdbstubs' - available on SourceForge
     (http://sourceforge.net/projects/gdbstubs/) 

   * working GNU C compiler for Hitach SH processors; do-it-yourself
     (on Linux) or download ready stuff for Windows'9x/NT/2k from
     (http://www.hitachi-eu.com/hel/ecg/) or from Hitach Databook 2001
     CD-ROM (if no luck try to search on the net for gnu99r1p1.zip)
     
   * GDB compiled for target sh-rtems  - do-it-yourself or download
     from ftp://ftp.oarcorp.com:21/pub/rtems/snapshots/c_tools/

   * RTEMS (ofcourse) you probably already have if you are reading this
     document

Instalation of 'gdbstubs'
-------------------------
  Once you downladed and unzipped gdbstubs you have to compile
  it. First modify the Makefile to use the compiler installed on your
  machine.  Then issue the command:

  $ make 

  This should produce the default target sh2-7045edk.out. This is the
  S-record file which should be added to FDT project (renaming it to
  *.mot extension helps a bit). 
  If you are lucky you will be able to put the file into the FLASH
  following the instuctions in FDT and EVB manuals. 
  
  Well I wasn't lucky so I had to bypass the Universal Programming
  Board (see EVB7045F User Manual) and manually put the processor into
  BOOT mode. This can be done by shortening the capacitor C8 (or C108
  on schematics) which puts the UPB into permanent reset state, and by
  removing jumper JP4 (or JP104 on schematics) and connecting its
  middle pin to the ground. After pressing CRES button the processor
  is in BOOT mode. In FDT select "direct connection":

  Menu Project->Properties->Device->Select Interface

  After that there should be no problem in putting the program into the
  FLASH. 

Loading 'hello.exe' on board
----------------------------
  I assume you are able to compile RTEMS with 'gensh2' BSP and
  necessary tools. If not please refer to 'started.pdf' document which
  describes the procedure (http://www.oarcorp.com/).

  At the time of writing this document 'gdbstubs' default
  communication port was SCI1. So it was the default port for
  /dev/console in RTEMS. To avoid problems I had check these settings
  both in 'gdbstubs' and $RTEMS_ROOT/c/src/lib/libbsp/sh/gensh2/include/bsp.h

  After changing the line 

```c
  #define BSP_CONSOLE_DEVNAME "/dev/sci1"
```

  to

```c
  #define BSP_CONSOLE_DEVNAME "/dev/sci0"
```
  in 'bsp.h' and rebuilding RTEMS there should no problem in running
  'hello.exe' and other samples. 

  For downloading connect a serial cable to computer and EVB. You will
  also need a second cable and second serial port to see the effects
  of your work.

  Assuming you are working in Linux and Xwindows fire up two terminal
  windows. In the first one run sh-rtems-gdb, in the second run a
  serial port terminal (for example 'minicom'). Set up the serial
  terminal to a port connected to SCI0 and leave the window in a
  visible place on the desktop. The debugger should be invoked best
  from the directory where 'hello.exe' is placed. Assuming that here
  is a GDB session:

  --------start------

```shell
  $ sh-rtems-gdb hello.exe 
  GNU gdb 5.0
  Copyright 2000 Free Software Foundation, Inc.
  GDB is free software, covered by the GNU General Public License, and you are
  welcome to change it and/or distribute copies of it under certain conditions.
  Type "show copying" to see the conditions.
  There is absolutely no warranty for GDB.  Type "show warranty" for details.
  This GDB was configured as "--host=i686-pc-linux-gnu --target=sh-rtems"...
  (gdb) set remotebaud 115200
  (gdb) target remote /dev/ttyS0
  Remote debugging using /dev/ttyS0
  0x0 in ?? ()
  (gdb) load 
  Loading section .text, size 0x12d70 lma 0x444000
  Loading section .data, size 0xb80 lma 0x456df0
  Loading section .stack, size 0x10 lma 0xfffffec0
  Start address 0x444000 , load size 80128
  Transfer rate: 58274 bits/sec, 153 bytes/write.
  (gdb) continue 
  Continuing.

  Program received signal 0, Signal 0.
  0x44ec36 in exit (code=0) at exit.c:70
  70	exit.c: No such file or directory.
  (gdb) 
  --------end-------
```
  And here is a capture from the serial terminal window:

```shell
*** HELLO WORLD TEST ***
Hello World
*** END OF HELLO WORLD TEST ***

  Beautiful, isn't it? That's all!
```


Variations
----------
  I'm sure that not every one can afford having two operating systems
  on one computer. I believe there will be a day that nobody will need
  an MS stuff anymore... ;)

  It is possible to repeat the success on MS Windows only. To do the
  same on Linux only you need a tool to downlad 'gdbstubs' on the
  board. This should be no problem to find it on the net but right now
  I don't know about it.

  For your convenience there are several graphical interfaces for GDB
  available on the net. I just name two of them:

  DDD - stands for Data Display Debugger
        (http://www.gnu.org/software/ddd/)

  Insight - a Tcl/Tk interface available both for MS Windows and
            Xwindows (http://sources.redhat.com/insight/)
