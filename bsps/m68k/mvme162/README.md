MVME162
=======

EISCAT Scientific Association. M.Savitski

This material is a part of the MVME162 Board Support Package
for the RTEMS executive. Its licensing policies are those of the
RTEMS distribution.

Updated by Joel Sherrill (jsherril@redstone.army.mil) after
inclusion in the standard release.


Models
------

There are three different models of the MVME162 board.  There are many
variations within each model.

```
  Model        Variants
  ---------    --------------------------------------------------
  MVME162      MVME162-0xx
  MVME162FX    MVME162-4xx, MVME162-5xx
  MVME162LX    MVME162-2xx, MVME162-3xx, MVME162-7xx, MVME162-8xx
```

All models use either an MC68040 or MC68LC040 (no FPU) processors.  The
processor used varies by variant as does the speed, the amount and type
of memory and the I/O devices (serial, ethernet, SCSI and VME).  See the
README.models file for details.


Configuring the BSP
-------------------
The BSP needs to be configured for your specific board.  The following
files need to be modified.

include/bsp.h
Change the MOT_162BUG_VEC_ADDRESS define to start of memory for your
board

make/custom/mvme162.cfg
If your board has an MC68040 processor
- change the value of RTEMS_CPU_MODEL
- remove the -msoft-float flag from CPU_CFLAGS


MVME162FX and DMA on the IP bus
-------------------------------

From Eric Vaitl <eric@viasat.com>:

If you have any customers that will be using the 162FX, tell them to 
be careful. The main difference between the 162 and the 162FX is DMA 
on the IP bus. I spent over a month trying to write a DMA HDLC driver 
for GreenSprings IP-MP and couldn't get it to work. I talked to some 
people at GreenSprings, and they agreed that there really is no way to 
get DMA to work unless you know the size of the packets in advance. 
Once the IP2 chip DMA controller is given the character count and 
enabled, it doesn't accept further commands until all of the 
characters have arrived. The only way to terminate a DMA transfer 
prematurely is by raising DMAEND* during the last read. None of the IP 
modules that I know of are currently able to do that. GreenSprings is 
working on the problem, but nothing is going to available for a few 
months. 

Installation
------------
Nothing unique to the MVME162.  It has been incorporated into the
standard release.

Port Description
----------------
This section describes the initial port effort.  There have been 
additions and modifications to the bsp since this was done.
Interestingly, this was the first bsp submitted to the RTEMS project
and the submission offer came out of the blue with no prior
communication with the author. :)

The port was done using already existing ports to the M68020 boards,
DMV152 and MVME136. 

The initial host development system was SUN/Solaris 2.3, and
the cross-development environment consisted of Free Software
Foundation (FSF)'s GNU C compiler (version 2.6), GNU Assembler
(version 2.3) and GNU binary utilities binutils version 2.5.2,
built with m68k as a target. The recent/latest versions of other
GNU programs (flex, make, etc) were also used at the build stage.

In all subdirectories of the RTEMS distribution tree, the directories
mvme136 were duplicated as mvme162.

Essential modifications are detailed below:

- the MVME162-specific hardware registers were described in bsp.h

- timer and clock routines were made to use the MVME162's Tick Timers 1
and 2, respectively

- shared memory support was replaced by stubs for the time being

- console IO was lifted entirely from the DMV152 support code, thanks
to the fact that Z8530 SCC used in DMV152 is upwards compatible with
the Z85230 SCC of the MVME162. (Only the memory mapping of the SCC
registers had to be changed.)

- symbols in several *.s files were prepended with underscores to
comply with the xgcc configuration used (it prepends underscores to all
symbols defined in c code)

- linkcmds file was modified to place the linked code into the memory
configured for the board in use

- bspstart.c was modified as follows:

         monitors_vector_table = (rtems_isr *)0xFFE00000;

was made to point to the power-up location of MVME162 interrupt vector
table.  
     
- The shutdown is a temporary solution. To exit cleanly, it has to disable
all enabled interrupts and restore the board to its power-up status.
Presently this is not done satisfactorily, as a result, the board needs
a hardware reset from the external VMEbus master or from the front
panel to ensure correct operation for subsequent downloads.

Host System
-----------
The VMEbus master used to externally control and download the MVME162
is a FORCE CPU-2CE board running Solaris 2.3. A simple program to load
s-records and start/reset the MVME162 was written. The code is in the
file tools/sload.c

This code depends on the external VMEbus master's vme driver and is
provided as an example, without the Makefile. The bulk of the program
which parses the s-records is courtesy of Kym Newbery,
(8918927y@lux.levels.unisa.edu.au).

In general, apart from x-gcc, the tools most often used while building
RTEMS for MVME162 were: find, grep, diff, and, of course

MVME162 Embedded Controller Programmer's Reference Guide,
Motorola, MVME162PG/D1.

Thanks
------
- to On-Line Applications Research Corporation (OAR) for developing
RTEMS and making it available on a Technology Transfer basis;
- to Joel Sherril, the leader of the RTEMS development group for
stimulating and helpful discussions;
- to Kym Newbery (8918927y@lux.levels.unisa.edu.au) for his s-record
parser;
- to Gerd Truschinski (gt@first.gmd.de) for creating and running the
crossgcc mailing list
- to FSF and Cygnus Support for great free software;

What's new
----------
  - 28.07.95 BSP adjusted to rtems-3.2.0. 
  - Now console driver uses interrupts on receive (ring buffer
    code lifted with thanks from the IDP BSP next door (../idp))
  - both front-panel serial interfaces are supported
  - serious bug in timer interrupts fixed
  - interrupt test tm27 now supported

```  
+----------------------------------+-------------------------------+
|  Dr. Mikhail (Misha) Savitski    |  Voice : +46-980-79162        |
|  Software Systems Engineer       |  Fax   : +46-980-79161        |
|  EISCAT Svalbard Radar Project   |  E-mail: mms@eiscathq.irf.se  |
|  EISCAT Scientific Association   |-----------  /\_/\  -----------|
|  Box 812 S-98128 Kiruna, Sweden  |  EIS       { o o }       CAT  |
+----------------------------------+-------oQQQ--(>I<)--QQQo-------+
```



MVME162 Models
==============

There are three different models of the MVME162 board with many variations
within each model.

```
  Model        Variants
  ---------    --------------------------------------------------
  MVME162      MVME162-0xx
  MVME162FX    MVME162-4xx, MVME162-5xx
  MVME162LX    MVME162-2xx, MVME162-3xx, MVME162-7xx, MVME162-8xx
```

All models use either an MC68040 or MC68LC040 (no FPU) processors.  The
processor used varies by variant as does the speed, the amount and type
of memory and the I/O devices (serial, ethernet, SCSI and VME).  See the
following tables for details.



MVME162 Variants
----------------

Source
------
o  MVME162 Embedded Controller User's Manual (MVME162/D2)


Common Configuration
--------------------
o  One EPROM socket
o  8Kx8 NVRAM/TOD clock
o  Two serial ports
o  1MB Flash memory
o  Four MVIP Industry Pack interfaces
o  One or two DRAM/SRAM mezzanine memory boards


```
Model  Processor  Speed  DRAM  SRAM   Other
-----  ---------  -----  ----  -----  ------------------
 001   MC68LC040  25MHz   1MB  512KB
 002   MC68040    25MHz   1MB  512KB
 003   MC68LC040  25MHz   1MB  512KB  No VMEbus
 010   MC68LC040  25MHz   4MB  512KB
 011   MC68LC040  25MHz   4MB  512KB  SCSI
 012   MC68LC040  25MHz   4MB  512KB  Ethernet
 013   MC68LC040  25MHz   4MB  512KB  Ethernet, SCSI
 014   MC68LC040  25MHz   4MB    -    Ethernet, No VMEbus
 020   MC68040    25MHz   4MB  512KB
 021   MC68040    25MHz   4MB  512KB  SCSI
 022   MC68040    25MHz   4MB  512KB  Ethernet
 023   MC68040    25MHz   4MB  512KB  Ethernet, SCSI
 026   MC68040    25MHz   4MB    -    Ethernet, No VMEbus
 030   MC68LC040  25MHz   8MB  512KB
 031   MC68LC040  25MHz   8MB  512KB  SCSI
 032   MC68LC040  25MHz   8MB  512KB  Ethernet
 033   MC68LC040  25MHz   8MB  512KB  Ethernet, SCSI
 040   MC68040    25MHz   8MB  512KB
 041   MC68040    25MHz   8MB  512KB  SCSI
 042   MC68040    25MHz   8MB  512KB  Ethernet
 043   MC68040    25MHz   8MB  512KB  Ethernet, SCSI

Serial Interface Modules
------------------------
SIM05  01-W3846B  EIA-232-D  DTE
SIM06  01-W3865B  EIA-232-D  DCE
SIM07  01-W3868B  EIA-530    DTE
SIM08  01-W3867B  EIA-530    DCE

DRAM/SRAM Expansion Memory Boards
---------------------------------
?
```

MVME162FX Variants
------------------

Source
------
o  MVME162FX Data Sheet
o  MVME162FX Embedded Controller Installation and Use (V162FXA/IH3)
o  MVME162FX Embedded Controller Programmer's Reference Guide (V162FXA/PG1)
o  MVME162FX 400/500-Series VME Embedded Controller Installation and Use
     (V162FXA/IH4) Edition of March 2000\Uffffffff
o  V162FXA/LT2, November 1995


Common Configuration
--------------------
o  One EPROM socket
o  8Kx8 NVRAM/TOD clock
o  Two serial ports
o  1MB Flash memory with 162Bug installed
o  512KB SRAM with battery backup
o  Four IndustryPack interfaces
o  One or two DRAM/SRAM mezzanine memory boards


Uses MC2 Chip, IP2 Chip, 4MB or 12MB mezzanine DRAM board

```
Model  Processor  Speed  DRAM  Other
-----  ---------  -----  ----  ------------------
 403
 410   MC68LC040  25Mhz   4MB
 411   MC68LC040  25Mhz   4MB  SCSI
 412   MC68LC040  25Mhz   4MB  Ethernet
 413   MC68LC040  25Mhz   4MB  Ethernet, SCSI
 420   ?
 421   ?
 422   ?
 423   ?
 430   MC68LC040  25Mhz   8MB
 431   MC68LC040  25Mhz   8MB  SCSI
 432   MC68LC040  25Mhz   8MB  Ethernet
 433   MC68LC040  25Mhz   8MB  Ethernet, SCSI
 440   ?
 441   ?
 442   ?
 443   ?
 450   ?
 451   ?
 452   ?
 453   MC68LC040  25Mhz  16MB  Ethernet, SCSI
 460   ?
 461   ?
 462   ?
 463   ?
 510   MC68040    32MHz   4MB
 511   MC68040    32MHz   4MB  SCSI
 512   MC68040    32MHz   4MB  Ethernet
 513   MC68040    32MHz   4MB  Ethernet, SCSI
 520   MC68040    32MHz   8MB
 521   MC68040    32MHz   8MB  SCSI
 522   MC68040    32MHz   8MB  Ethernet
 523   MC68040    32MHz   8MB  Ethernet, SCSI
 530   MC68040    32MHz  16MB
 531   MC68040    32MHz  16MB  SCSI
 532   MC68040    32MHz  16MB  Ethernet
 533   MC68040    32MHz  16MB  Ethernet, SCSI


Serial Interface Modules
------------------------
SIM05  01-W3846B  EIA-232-D    DTE
SIM06  01-W3865B  EIA-232-D    DCE
SIM07  01-W3868B  EIA-530      DTE
SIM08  01-W3867B  EIA-530      DCE
SIM09  01-W3002F  EIA-485/422  DTE/DCE


DRAM/SRAM Expansion Memory Boards
---------------------------------
MVME162-502   4MB DRAM
MVME162-503  12MB DRAM
?             2MB SRAM
```


MVME162LX Variants
------------------

Source
------
o  Supplement to MVME162LX Embedded Controller Installation Guide
    (MVME162LXIG/D1A1) February 1995
o  MVME162LX Embedded Controller Data Sheet
o  MVME162LX 200/300 Series Embedded Controller Programmer's Reference
     Guide (V162LX2-3A/PG2)
o  MVME162LX 200/300 Series Embedded Controller Installation and Use
     (V162LX2-3A/IH3)
o  MVME162LX 700/800 Series Embedded Controller Installation and Use
     (V162-7A/IH1)
o  MVME162LX 700/800 Series Embedded Controller Installation and Use
     (V162-7A/IH2)


Common Configuration
--------------------
o  One EPROM socket
o  8Kx8 NVRAM/TOD clock
o  4 serial ports EIA-232-D DTE (unless otherwise noted)
o  1MB Flash
o  2 IP sites (unless otherwise noted)


```
Model  Processor  Speed  DRAM      Other
-----  ---------  -----  --------  ------------------
 200   MC68LC040  25MHz   1MB      No serial(?)
 201   MC68LC040  25MHz   1MB
 202   MC68LC040  25MHz   1MB
 210   MC68LC040  25MHz   4MB
 211   MC68LC040  25MHz   4MB      SCSI
 212   MC68LC040  25MHz   4MB      Ethernet
 213   MC68LC040  25MHz   4MB      Ethernet, SCSI
 216   MC68LC040  25MHz   4MB      Ethernet, No VMEbus, No serial(?)
 220   MC68040    25MHz   4MB
 222   MC68040    25MHz   4MB      Ethernet
 223   MC68040    25MHz   4MB      Ethernet, SCSI
 233   MC68LC040  25MHz   4MB ECC
 233   MC68LC040  25MHz   4MB ECC  Ethernet, SCSI
 243   MC68040    25MHz   4MB ECC  Ethernet, SCSI
 253   MC68LC040  25MHz  16MB ECC  Ethernet, SCSI
 253   MC68LC040  25MHz  16MB ECC  Ethernet, SCSI
 262   MC68040    25MHz  16MB ECC  Ethernet
 263   MC68040    25MHz  16MB ECC  Ethernet, SCSI
 322   MC68LC040  25MHz   8MB ECC  Ethernet
 323   MC68LC040  25MHz   8MB ECC  Ethernet, SCSI
 333   MC68040    25MHz   8MB ECC  Ethernet, SCSI, No IP sites(?)
 353   MC68040    25MHz  32MB ECC  Ethernet, SCSI, 4 IP sites
 723   MC68040    32MHz   4MB      Ethernet, SCSI
 743   MC68040    32MHz   4MB ECC  Ethernet, SCSI
 763   MC68040    32MHz  16MB ECC  Ethernet, SCSI
 813   MC68040    32MHz   8MB      Ethernet, SCSI
 833   MC68040    32MHz   8MB ECC  Ethernet, SCSI
 853   MC68040    32MHz  32MB ECC  Ethernet, SCSI
 863   MC68040    32MHz  16MB ECC  Ethernet, SCSI
```


DRAM Expansion Memory Boards
----------------------------
MVME162-202   4MB     (non-stacking)
MVME162-203  16MB ECC (non-stacking)
MVME162-204  16MB ECC (stacking)
MVME162-207   4MB ECC (non-stakcing)
MVME162-208   4MB ECC (stacking)
MVME162-209   8MB ECC (non-stacking)
MVME162-210   8MB ECC (stacking)
MVME162-211  32MB ECC (non-stacking)
MVME162-212  32MB ECC (stacking)


