mvme5500
========
Please reference README.booting for the boot/load process.

For the priority setting of the Interrupt Requests (IRQs), please
reference README.irq 

The BSP is built and tested on the 4.7.1 and 4.7.99.2 CVS RTEMS release.

I believe in valuable real-time programming, where technical neatness,
performance and truth are.  I hope I still believe. Any suggestion,
bug reports, or even bug fixes (great!) would be highly appreciated
so that I still believe what I believe.


ACKNOWLEDGEMENTS
----------------
Acknowledgements:

Valuable information was obtained from the following:
1) Marvell NDA document for the discovery system controller.
Other related documents are listed at :
http://www.aps.anl.gov/epics/meetings/2006-06/RTEMS_Primer_SIG/RTEMS_BSP_MVME5500.pdf
2) netBSD: For the two NICS and some headers :
           Allegro Networks, Inc., Wasabi Systems, Inc.  
3) RTEMS:  This BSP also builds on top of the work of others who have
 contributed to similar RTEMS powerpc shared and motorola_powerpc BSPs, most
 notably Eric Valette, Till Straumann (SVGM1 BSP, too), Eric Norum and others.

LICENSE
-------
See ./LICENSE file.

```
BSP NAME:           mvme5500
BOARD:              MVME5500 by Motorola
BUS:                PCI
CPU FAMILY:         ppc
CPU:                MPC7455 @ 1GHZ
COPROCESSORS:       N/A
MODE:               32/64 bit mode (support 32 bit for now)
DEBUG MONITOR:      MOTLoad
SYSTEM CONTROLLER:  GT64260B
```      

PERIPHERALS
-----------
```      
TIMERS:             Eight, 32 bit programmable
SERIAL PORTS:       2 NS 16550 on GT64260B
REAL-TIME CLOCK:    MK48T37V
32K NVSRAM:         MK48T37V
WATCHDOG TIMER:     use the one in GT-64260B
DMA:                8 channel DMA controller (GT-64260B)
VIDEO:              none
NETWORKING:         Port 1: Intel 82544EI Gigabit Ethernet Controller
		    10/100/1000Mb/s routed to front panel RJ-45
	            Port 2: 10/100 Mb ethernet unit integrated on the
                    Marvell's GT64260 system controller 
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


Jumpers
=======

1) The BSP is tested with the 60x bus mode instead of the MPX bus mode.
   ( No jumper or a jumper across pins 1-2 on J19 selects the 60x bus mode)

2) On the mvme5500 board, Ethernet 1 is the Gigabit Ethernet port and is
   front panel only. Ethernet 2 is 10/100 BaseT Ethernet. For front-panel
   Ethernet2, install jumpers across pins 1-2 on all J6, J7, J100 and
   J101 headers.

3) Enable SROM initialization at startup. (No jumper or a jumper across
   pins 1-2 on J17)

In fact, (if I did not miss anything) the mvme5500 board should function
properly if one keeps all the jumpers at factory configuration.
One can leave out the jumper on J30 to disable EEPROM programming.


BSP BAT usage
-------------
```      
DBAT0 and IBAT0
0x00000000
0x0fffffff  1st 256M, for MEMORY access (caching enabled)

DBAT1 and IBAT1
0x00000000
0x0fffffff  2nd 256M, for MEMORY access (caching enabled)
```      

UPDATE: (2004/5).
The BSP now uses page tables for mapping the entire 512MB
of RAM. DBAT0 and DBAT1 is hence free for use by the 
application. A simple 1:1 (virt<->phys) mapping is employed.
The BSP write-protects the text and read-only data
areas of the application.  Special acknowledgement to Till
Straumann <strauman@slac.stanford.edu> for providing inputs in
porting the memory protection software he wrote (BSP_pgtbl_xxx())
to MVME5500.


The default VME configuration uses DBAT0 to map
more PCI memory space for use by the universe VME
bridge:

```      
DBAT0
0x90000000	PCI memory space <-> VME
0x9fffffff

Port  VME-Addr   Size       PCI-Adrs   Mode:
0:    0x20000000 0x0F000000 0x90000000 A32, Dat, Sup
1:    0x00000000 0x00FF0000 0x9F000000 A24, Dat, Sup
2:    0x00000000 0x00010000 0x9FFF0000 A16, Dat, Sup
```


Booting
=======

Written by S. Kate Feng <feng1@bnl.gov>, Aug. 28, 2007

The bootloader is adapted from Till Straumann's Generic Mini-loader,
which he wrote originally for the SVGM powerpc board. 
The BSP is built and tested on the 4.7 CVS RTEMS release.

Booting requirement
-------------------

1) One needs to setup BOOTP/DHCP and TFTP servers and /etc/bootptab(BOOTP)
   or /etc/dhcpd.conf (DHCP) properly to boot the system.
   (Note : EPICS needs a NTP server).
 
2) Please copy the prebuilt RTEMS binary (e.g. misc/rtems5500-cexp.bin)
   and perhaps others (e.g. misc/st.sys) to the /tftpboot/epics/hostname/bin/
   directory or the TFTPBOOT one you specified in the 'tftpGet'
   command of the boot script (as shown in the following example).

3) Example of the boot script setup carried out on the MOTLoad 
   command line : 

```shell
MVME5500> gevEdit mot-script-boot
(Blank line terminates input.)
waitProbe
tftpGet -a4000000 -cxx.xx.xx.xx -sxx.xx.xx.xx -m255.255.254.0 -d/dev/enet0 -fepics/hostname/bin/rtems5500-cexp.bin
netShut
go -a4000000

Update Global Environment Area of NVRAM (Y/N) ? Y
MVME5500>

Note : (cxx.xx.xx.xx is the client IP address and 
        sxx.xx.xx.xx is the server IP address)

WARNING : It is extremely important that the MOTLoad "waitProbe", "netShut"
          sequence be executed before booting RTEMS. Otherwise, network
          interface interrupt handlers installed by MOTLoad may cause memory
          corruption
```

4) Other reference web sites for mvme5500 BSP:
http://lansce.lanl.gov/EPICS/presentations/KateFeng%20RTEMS-mvme55001.ppt
http://www.nsls.bnl.gov/facility/expsys/software/EPICS/
http://www.nsls.bnl.gov/facility/expsys/software/EPICS/FAQ.txt

5) When generating code (especially C++) for this system, one should
   use at least gcc-3.2 (preferrably a copy downloaded from the RTEMS
   site [snapshot area] )

6) To reboot the RTEMS-MVME5500 (board reset), one can invoke the
   bsp_reset() command at Cexp> prompt.
 
7) Please reference http://www.slac.stanford.edu/~strauman/rtems
for the source code and installation guidance of cexp, GeSys and
other useful utilities such as telnet, nfs, and so on. 

8) To get started with RTEMS/EPICS and to build development
tools and BSP, I would recommend one to reference
http://www.aps.anl.gov/epics/base/RTEMS/tutorial/
in additional to the RTEMS document.


IRQ
===
Shuchen Kate Feng  <feng1@bnl.gov>, Sept. 2, 2007

As per implementation in shared PPC code,
the BSPirqPrioTable[96] listed in irq_init.c is where the
software developers can change the levels of priority
for all the interrupts based on the need of their
applications.  The IRQs can be eanbled dynamically via the
BSP_enable_pic_irq(), or disbaled dynamically via the
BSP_disable_pic_irq(). 


Support for run-time priority setup could be
added easily, but there is no action taken yet due to concerns
over computer security at VME CPU level. 


The software developers are forbidden to setup picIsrTable[],
as it is a powerful engine for the BSP to find the pending
highest priority IRQ at run time. It ensures the fastest/faster
interrupt service to the highest/higher priority IRQ, if pending.


VME
===
Written by S. Kate Feng <feng1@bnl.gov> , 7/22/04

Some VME modules(e.g. Oms58 motor controller) might require a PCI sync
command following the out_xx() function (e.g. out_be16()) if mvme5500 is
used as the SBC.  The mechanism is a hardware hook to help software
synchronize between the CPU and PCI activities. The PCI sync is
implemented in pci/pci_interface.c. For more example of the usage,one
can reference the drvOMS58.cc file that is posted in synAppRTEMS of 
http://www.nsls.bnl.gov/organization/UserScience/Detectors/Software/Default.htm.

In spite of the PCI sync overhead for the Oms58 motor controller, I do
not see the runtime performance of RTEMS-mvme5500 being compromised as
compared with that of RTEMS-mvme2307.  For example, it takes the same
time to run motor_init() of synAppRTEMS for 48 motor initializations
running either RTEMS-mvme2307 or RTEMS-mvme5500.
