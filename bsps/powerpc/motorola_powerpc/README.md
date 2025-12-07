MCP750
======
```
BSP NAME:           MCP750
BOARD:              MCP750 from motorola
BUS:                PCI
CPU FAMILY:         ppc
CPU:                PowerPC 750
COPROCESSORS:       N/A
MODE:               32 bit mode

DEBUG MONITOR:      PPCBUG mode 
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
NETWORKING:         DEC21140
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
Based on papyrus bsp which only really supports
the PowerOpen ABI with an ELF assembler.



Booting
=======

This file documents the on board monitor (PPCBUG) configuration used
to be able to boot the archives located in powerpc-rtems/c/mcp750/bin.
This information was provided by Eric Valette <eric.valette@free.fr>

NOTE (by Till Straumann <strauman@slac.stanford.edu>, 2003):
Apparently, PPCBug fails to shut down the network interface after
loading an image. This means that the ethernet chip is still able
to write into its descriptors and network buffer memory which
can result in the loaded system to be corrupted if that system
relocates itself!. The proper place to shut down the interface
would be PPCBug itself or a 'PPCBug startup script' - unfortunately,
PPCBug doesn't offer such a feature. Therefore, the bootloader
is by default compiled with the
#ifdef USE_PPCBUG
compile-time option ENABLED. It will then use a PPCBug system
call to shut down the ethernet chip during an early stage of
the boot process.
NOTE: THIS (i.e. the system call) WILL FAIL IF YOU USE SOFTWARE
OTHER THAN PPCBUG TO BOOT THE BSP. In such a case, you must
recompile with #undef USE_PPCBUG and make sure the ethernet
interface is quiet by other means.

```shell
----------------------- ENV command--------------
PPC1-Bug>env
Bug or System environment [B/S] = B? 
Field Service Menu Enable [Y/N] = N? 
Probe System for Supported I/O Controllers [Y/N] = Y? 
Auto-Initialize of NVRAM Header Enable [Y/N] = Y? 
Network PReP-Boot Mode Enable [Y/N] = Y?                <====================
SCSI Bus Reset on Debugger Startup [Y/N]   = N? 
Primary SCSI Bus Negotiations Type [A/S/N] = A? 
Primary SCSI Data Bus Width [W/N]          = N? 
Secondary SCSI Identifier                  = "07"? 
NVRAM Boot List (GEV.fw-boot-path) Boot Enable [Y/N]           = Y? 
NVRAM Boot List (GEV.fw-boot-path) Boot at power-up only [Y/N] = Y? 
NVRAM Boot List (GEV.fw-boot-path) Boot Abort Delay            = 5? 
Auto Boot Enable [Y/N]           = Y? 
Auto Boot at power-up only [Y/N] = Y? 
Auto Boot Scan Enable [Y/N]      = Y? 
Auto Boot Scan Device Type List  = FDISK/CDROM/TAPE/HDISK/? 
Auto Boot Controller LUN   = 14? 
Auto Boot Device LUN       = 40? 
Auto Boot Partition Number = 03? 
Auto Boot Abort Delay      = 7? 
Auto Boot Default String [NULL for an empty string] = ? 
ROM Boot Enable [Y/N]            = N? 
ROM Boot at power-up only [Y/N]  = Y? 
ROM Boot Abort Delay             = 5? 
ROM Boot Direct Starting Address = FFF00000? 
ROM Boot Direct Ending Address   = FFFFFFFC? 
Network Auto Boot Enable [Y/N]           = N? 
Network Auto Boot at power-up only [Y/N] = N? 
Network Auto Boot Controller LUN = 00? 
Network Auto Boot Device LUN     = 00? 
Network Auto Boot Abort Delay    = 5? 
Network Auto Boot Configuration Parameters Offset (NVRAM) = 00001000? 
Memory Size Enable [Y/N]         = Y? 
Memory Size Starting Address     = 00000000? 
Memory Size Ending Address       = 02000000? 
DRAM Speed in NANO Seconds       = 60? 
ROM First Access Length (0 - 31) = 10? 
ROM Next Access Length  (0 - 15) = 0? 
DRAM Parity Enable [On-Detection/Always/Never - O/A/N]    = O? 
L2Cache Parity Enable [On-Detection/Always/Never - O/A/N] = O? 
PCI Interrupts Route Control Registers (PIRQ0/1/2/3) = 0A050000? 
Serial Startup Code Master Enable [Y/N] = N? 
Serial Startup Code LF Enable [Y/N] =     N? 
---------------------NIOT ---------------------------
PPC1-Bug>niot
Controller LUN =00? 
Device LUN     =00? 
Node Control Memory Address =03F9E000? 
Client IP Address      =194.2.81.157? 
Server IP Address      =194.2.81.241? 
Subnet IP Address Mask =255.255.255.0? 
Broadcast IP Address   =255.255.255.255? 
Gateway IP Address     =194.2.81.254? 
Boot File Name ("NULL" for None)     =debug-ppc? 
Argument File Name ("NULL" for None) =? 
Boot File Load Address         =001F0000? 
Boot File Execution Address    =001F0000? 
Boot File Execution Delay      =00000000? 
Boot File Length               =00000000? 
Boot File Byte Offset          =00000000? 
BOOTP/RARP Request Retry       =05? 
TFTP/ARP Request Retry         =05? 
Trace Character Buffer Address =00000000? 
BOOTP/RARP Request Control: Always/When-Needed (A/W)=W? 
BOOTP/RARP Reply Update Control: Yes/No (Y/N)       =Y? 
--------------------------------------------------------
```


QEMU
====
The 'qemuprep'/'qemuprep-altivec' BSPs are variants of
'motorola_powerpc' that can run under QEMU. They are *not*
binary compatible with other variants of 'motorola_powerpc'
(nor with each other).

Most significant differences to real hardware:
 - no OpenPIC, just a 8259 PIC (even though qemu implements an openpic
   at least to some extent it is not configured into the prep platform
   as of qemu-0.14.1).
 - no VME (absense of the VME controller is detected by the BSP)
 - the only network chip supported by both, qemu and vanilla RTEMS
   is the ISA NE2000 controller. Note that the default interrupt line
   settings used by RTEMS and QEMU differ: RTEMS uses 5 and QEMU 9.
   This can be addressed by passing a RTEMS commandline option
   --ne2k-irq=9.
   Other controllers (i8559, e1000, pcnet) implemented by qemu can
   also be used but require unbundled RTEMS drivers (libbsdport).
   Note that the bundled 'if_fxp' has not been ported to PPC and works
   on x86 only.
 - unlike a real motorola board you can run qemu emulating a 7400 CPU
   which features altivec. I.e., you can use this BSP (altivec-enabled
   variant) to test altivec-enabled code.

Compatibility: qemu had quite a few bugs related to the PREP platform.
Version 0.12.4, for example, required patches. 0.14.1 seems to have
fixed the show-stoppers. Hence, you *need* at least qemu-0.14.1 for
this BSP; it should work without the need for patching QEMU.

BIOS: qemu requires you to use a BIOS. The one that came with qemu
0.12.4 didn't work for me so I created a minimal dummy that provides
enough functionality for the RTEMS bootloader to work.

BSP Variants:
You can compile the BSP for either a 604 CPU or a 7400 (altivec-enabled).
Note that you cannot run the altivec-enabled BSP variant on a CPU w/o
altivec/SIMD hardware. The non-altivec variant is called 'qemuprep'
and the altivec-enabled one 'qemuprep-altivec'. Hence, you can
configure RTEMS:

604/non-altivec variant only:

```shell
  configure --target=powerpc-rtems --enable-rtemsbsp=qemuprep
```

7400/altivec variant only:
```shell
  configure --target=powerpc-rtems --enable-rtemsbsp=qemuprep-altivec
```

both variants:
```shell
  configure --target=powerpc-rtems --enable-rtemsbsp='qemuprep qemuprep-altivec'
``
Building QEMU:
In case you have no pre-built qemu-0.14.1 you can
compile it yourself:

cd qemu-0.14.1
configure --target-list=ppc-softmmu
make

Running QEMU:
A number of command-line options are important (BTW: make sure
you run the PPC/PREP emulator and not a natively installed i386/PC
emulating 'qemu')

```
-M prep         --- select machine type: prep
-cpu 604        --- select 604 CPU for non-altivec variant
-cpu 7400       --- select 7400 CPU for altivec variant

              NOTE: the 7455 and 7457 emulations are buggy as of
              qemu-0.14.1 and they won't work.

-bios <rtems-install-prefix>/powerpc-rtems/qemuprep/qemu_fakerom.bin
-bios <rtems-install-prefix>/powerpc-rtems/qemuprep-altivec/qemu_fakerom.bin
                --- select proprietary dummy 'BIOS'

-nographic      --- redirect serial/IO to console where qemu is run

-kernel <path>  --- path to your RTEMS executable (.ralf file, e.g., 'hello.ralf')
-no-reboot      --- terminate after one run
-append <arg>   --- RTEMS kernel comand line (use e.g., to modify
                    ne2000 driver interrupt line)
```

Networking
----------
(We assume your RTEMS application is correctly configured and
built for networking using the ne2k adapter [other adapters 
can be used with unbundled/libbsdport drivers])

I use networking with a 'tap' interface on the host machine
and can then communicate with the emulated target in any
desired way.  The Ethernet address specified in the RTEMS network interface
configuration and the Qemu command line must match, otherwise uni-cast frames
are not received.  It is best to use a NULL pointer in the RTEMS network
interface configuration for the Ethernet address, so that the default from Qemu
is used.  Make sure that your firewall settings allow communication between
different Qemu instances and your host.

Linux
-----

# create a 'permanent' tap device that can be used by myself
# (as non-root user).
sudo tunctl -u `id -u`
# configure tap0 interface
sudo ifconfig tap0 10.1.1.1 netmask 255.255.255.0 up
# provide a suitable dhcpd config file (for the emulated
# platform to boot: IP address etc.
#
# execute dhcp on host
sudo dhcpd -d tap0

Start emulated prep platform:

```shell
ppc-softmmu/qemu-system-ppc                               \
    -M prep                                               \
    -cpu 7400                                             \
    -bios  <rtems-prefix>/powerpc-rtems/qemuprep-altivec/lib/qemu_fakerom.bin \
    -kernel <my_path>/my_app.ralf                         \
	-append --ne2k-irq=9                                  \
    -nographic                                            \
    -no-reboot                                            \
    -net nic,model=ne2k_isa                               \
    -net tap,vlan=0,ifname=tap0,script=no,downscript=no 
```
Again: if you use the non-altivec BSP variant, use -cpu 604
and if you use the altivec-enabled variant then you MUST use
-cpu 7400.

Have fun.

Till Straumann, 2011/07/18


dec21140
========
The dec21140 network driver is found in libchip/networking.


mtx603e
=======
```
BSP NAME:           mtx603e
BOARD:              MTX-60X boards from motorola
BUS:                PCI, W83C554
CPU FAMILY:         ppc
CPU:                PowerPC 603e
COPROCESSORS:       N/A
MODE:               32 bit mode

DEBUG MONITOR:      PPCBUG mode 
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
NETWORKING:         DEC21140
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

This bsp is an instantiation of the generic motorola_powerpc BSP.  It is
"virtual" in the sense it does not supply any per-bsp files.  Instead,
it is defined by the aclocal and make/custom config files which supply
#defines that adapt the shared powerpc code.  This is seen in the
bootloader and irq setup files.

Although created for a MTX-603e board, this bsp should be readily
portable to any of the Motorola MTX boards, and has in fact run on a
MCP750 board.

Some MTX boards have multiple processors, at this time RTEMS does not
support SMP and there is no internal awareness of the architecture.



MVME2100
========
The MVME2100 is a Motorola VMEbus board which is similar to the other
Motorola PowerPC boards supported by this BSP.  But it does not support
the Motorola CPU Configuration Register.  This makes it impossible to
dynamically probe and determine that you are executing on this board
variant.  So this BSP variant must be explicitly built to only support
the MVME2100.  The complete list of differences found so far is:

  * No CPU Configuration Register
  * one COM port
  * COM port is on PCI IRQ not ISA IRQ
  * limited on RAM (32 or 64 MB)
  * uses the EPIC interrupt controller on the MPC8240
  * does not have an ISA bus but has an ISA I/O address space
  * cannot set DBAT2 in bspstart like other variants because
    there are PCI/ISA Interrupt Acknowledge registers at this space
    This BSP may have left some PCI memory uncovered
  * PPCBug starts programs with vectors still in ROM

Supported Features:
  - Interrupt driven console using termios
  - Network device driver
  - Real-Time Clock driver
  - Clock Tick Device Driver

Things to address:
  - Does not return to monitor
  - Level 1 cache is disabled for now
  - Check on trying to read CPU Configuration Register for CHRP/Prep for PCI
    and report a failure if in the wrong mode.  May be able to set the model
    but it may be hard to test if we break PPCBug.
  - Use NVRAM for network configuration information

BSP Features Not Implemented:
  - VMEbus mapped in but untested
  - OpenPIC features not required for BSP are not supported

Memory Map
----------
```
                                                     BAT Mapping

    ffff ffff   |------------------------------------| ----- ffff ffff
                | ROM/FLASH Bank 0                   |   |
    fff0 0000   |------------------------------------|   |
                | System I/O                         |   |
    ffe0 0000   |------------------------------------|   |
                | Replicated ROM/FLASH Bank 0        |   |
                | Replicated System I/O              |   |
    ff80 0000   |------------------------------------|   |
                | ROM/FLASH Bank 1                   | DBAT3
    ff00 0000   |------------------------------------|  - Supervisor R/W
                | PCI Interrupt Acknowledge          |  - Cache Inhibited
    fef0 0000   |------------------------------------|  - Guarded
                | PCI Configuration Data Register    |   |
    fee0 0000   |------------------------------------|   | 
                | PCI Configuration Address Register |   | 
    fec0 0000   |------------------------------------|   | 
                | PCI I/O Space                      |   | 
    fe80 0000   |------------------------------------|   | 
                | PCI/ISA I/O Space                  |   | 
    fe00 0000   |------------------------------------|   | 
                | PCI/ISA Memory Space               |   | 
    fd00 0000   |------------------------------------|   | 
                |                                    |   | 
                |                      xxxxxxxxxxxxxx| ----- f000 0000
                |                      x not mapped  |   |
                |                      xxxxxxxxxxxxxx| ----- a000 0000 
                |                                    |   | 
                |                                    |   |
                |                                    | DBAT0
                |                                    |  - Supervisor R/W
                |                                    |  - Cache Inhibited
                |                                    |  - Guarded
                |                                    |   |
                |                                    |   | 
                |                                    | ----- 9000 0000
                |                                    |   |
                |                                    |   |
                | PCI Memory Space                   | DBAT2
                |                                    |  - Supervisor R/W
                |                                    |  - Cache Inhibited 
                |                                    |  - Guarded
                |                                    |   |
                |                                    |   |
                |                                    |   |
    8000 0000   |------------------------------------| ----- 8000 0000
                |                      x             |   
                |                      x not mapped  |
                | Reserved             xxxxxxxxxxxxxx| ----- 1000 0000
                |                                    |   |
                |                                    |   |
    0200 0000   |------------------------------------|   |
                |                                    |   |
                |                                    |   |
                |                                    |   |
                |                                    |   |
                | DRAM (32MB)                        | DBAT1/IBAT1
                |                                    |  - Supervisor R/W
                |                                    |   |
                |                                    |   |
                |                                    |   |
                |                                    |   |
    0000 0000   |------------------------------------| ----- 0000 0000
```


TTCP Performance on First Day Run
---------------------------------
Fedora Core 1 on (according to /proc/cpuinfo) a 300 Mhz P3 using Netgear
10/100 CardBus NIC on a dedicated 10BaseT LAN.  

```
ON MVME2100:            ttcp -t -s 192.168.2.107
REPORTED ON MVME2100:
ttcp-t: buflen=8192, nbuf=2048, align=16384/0, port=5001  tcp  -> 192.168.2.107
ttcp-t: socket
ttcp-t: connect
ttcp-t: 16777216 bytes in 20.80 real seconds = 787.69 KB/sec +++
ttcp-t: 2048 I/O calls, msec/call = 10.40, calls/sec = 98.46
ttcp-t: 0.0user 20.8sys 0:20real 100% 0i+0d 0maxrss 0+0pf 0+0csw

ON MVME2100:            ttcp -t -s 192.168.2.107
REPORTED ON MVME2100:
ttcp -r -s
ttcp-r: buflen=8192, nbuf=2048, align=16384/0, port=5001  tcp
ttcp-r: socket
ttcp-r: accept from 192.168.2.107
ttcp-r: 16777216 bytes in 15.41 real seconds = 1063.21 KB/sec +++
ttcp-r: 11588 I/O calls, msec/call = 1.36, calls/sec = 751.98
ttcp-r: 0.0user 15.4sys 0:15real 100% 0i+0d 0maxrss 0+0pf 0+0csw
```


MVME2300
========
This BSP was adapted from Eric Valette MCP750 Generic motorola
port to MVME2300 by Jay Kulpinski <jskulpin@eng01.gdds.com>.
In other to work correctly, the Tundra Universe chip must
be turned off using PPCBug as explained below.

The Tundra Universe chip is a bridge between the PCI and VME buses.
It has four programmable mapping windows in each direction, much like
the Raven.  PPCBUG lets you specify the mappings if you don't want
to do it in your application.  The mappings on our board, which may
or not be the default Motorola mappings, had one window appearing
at 0x01000000 in PCI space.  This is the same place the bootloader
code remapped the Raven registers.  The windows' mappings are 
very likely to be application specific, so I wouldn't worry too
much about setting them in the BSP, but it would be nice to have
a standard interface to do so.  Whoever needs that first can 
incorporate the ppcn_60x BSP code for the Universe chip. :-)

These options in PPCBUG's ENV command did the job:

```
VME3PCI Master Master Enable [Y/N] = Y?
PCI Slave Image 0 Control                = 00000000?   <-----
PCI Slave Image 0 Base Address Register  = 00000000?
PCI Slave Image 0 Bound Address Register = 00000000?
PCI Slave Image 0 Translation Offset     = 00000000?
PCI Slave Image 1 Control                = 00000000?   <-----
PCI Slave Image 1 Base Address Register  = 01000000?
PCI Slave Image 1 Bound Address Register = 20000000?
PCI Slave Image 1 Translation Offset     = 00000000?
PCI Slave Image 2 Control                = 00000000?   <-----
PCI Slave Image 2 Base Address Register  = 20000000?
PCI Slave Image 2 Bound Address Register = 22000000?
PCI Slave Image 2 Translation Offset     = D0000000?
PCI Slave Image 3 Control                = 00000000?   <-----
PCI Slave Image 3 Base Address Register  = 2FFF0000?
PCI Slave Image 3 Bound Address Register = 30000000?
PCI Slave Image 3 Translation Offset     = D0000000?        
```


MVME2400
========
The generic motorla_powerpc BSP was adapted to work on a MVME2432 by
Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>.

The main steps needed were adaptions to the "Hawk" controller, which
replaces the MVME2300 Raven and Falcon chips.

This board now runs with the same BSP configuration as the MCP750, so
select the mcp750 BSP.

The following settings in the PPCBUG's ENV were also important (taken
from the "README.MVME2300" file:)

```
VME3PCI Master Master Enable [Y/N] = Y?
PCI Slave Image 0 Control                = 00000000?   <-----
PCI Slave Image 0 Base Address Register  = 00000000?
PCI Slave Image 0 Bound Address Register = 00000000?
PCI Slave Image 0 Translation Offset     = 00000000?
PCI Slave Image 1 Control                = 00000000?   <-----
PCI Slave Image 1 Base Address Register  = 01000000?
PCI Slave Image 1 Bound Address Register = 20000000?
PCI Slave Image 1 Translation Offset     = 00000000?
PCI Slave Image 2 Control                = 00000000?   <-----
PCI Slave Image 2 Base Address Register  = 20000000?
PCI Slave Image 2 Bound Address Register = 22000000?
PCI Slave Image 2 Translation Offset     = D0000000?
PCI Slave Image 3 Control                = 00000000?   <-----
PCI Slave Image 3 Base Address Register  = 2FFF0000?
PCI Slave Image 3 Bound Address Register = 30000000?
PCI Slave Image 3 Translation Offset     = D0000000?        
```


Other Boards
============
This BSP is designed to support multiple Motorola PowerPC boards.  The
following extract from some email from Eric Valette should provide
the basic information required to use this BSP on other models.

```
Joel>      + I am sure there are other Motorola boards which this BSP should
Joel>        support.  If you know of other models that should work, list
Joel>        them off to me.  I will make them aliases and note them as
Joel>        untested in the status.
```

Extract of motorola.c :

```c
static const mot_info_t mot_boards[] = {
  {0x300, 0x00, "MVME 2400"},
  {0x010, 0x00, "Genesis"},
  {0x020, 0x00, "Powerstack (Series E)"},
  {0x040, 0x00, "Blackhawk (Powerstack)"},
  {0x050, 0x00, "Omaha (PowerStack II Pro3000)"},
  {0x060, 0x00, "Utah (Powerstack II Pro4000)"},
  {0x0A0, 0x00, "Powerstack (Series EX)"},
  {0x1E0, 0xE0, "Mesquite cPCI (MCP750)"},
  {0x1E0, 0xE1, "Sitka cPCI (MCPN750)"},
  {0x1E0, 0xE2, "Mesquite cPCI (MCP750) w/ HAC"},
  {0x1E0, 0xF6, "MTX Plus"},
  {0x1E0, 0xF7, "MTX wo/ Parallel Port"},
  {0x1E0, 0xF8, "MTX w/ Parallel Port"},
  {0x1E0, 0xF9, "MVME 2300"},
  {0x1E0, 0xFA, "MVME 2300SC/2600"},
  {0x1E0, 0xFB, "MVME 2600 with MVME712M"},
  {0x1E0, 0xFC, "MVME 2600/2700 with MVME761"},
  {0x1E0, 0xFD, "MVME 3600 with MVME712M"},
  {0x1E0, 0xFE, "MVME 3600 with MVME761"},
  {0x1E0, 0xFF, "MVME 1600-001 or 1600-011"},
  {0x000, 0x00, ""}
};
```

In theory, each board starting with 0x1E0 should be really straighforward
to port (604 proc or above and raven host bridge...).

Joel> Then we just have to add README.BOARD_MODEL and TIMES.BOARD_MODEL

I should also make a README to explain that some file containing
switch statement should be completed (e.g bsps/powerpc/shared/irq_init.c
[NOTE: This is that README. :) ]

```c
  if ( (currentBoard == MESQUITE) ) {
    VIA_isa_bridge_interrupts_setup();
    known_cpi_isa_bridge = 1;
  }
  if (!known_cpi_isa_bridge) {
    printk("Please add code for PCI/ISA bridge init to bsps/shared/irq/irq_init.c\n");
    printk("If your card works correctly please add a test and set known_cpi_isa_bridge to true\n");
  }
```

and bsps/powerpc/exceptions/

```c
int mpc604_vector_is_valid(rtems_vector vector)
{
  /*
   * Please fill this for MVME2307
   */
  printk("Please complete libcpu/powerpc/XXX/raw_exception.c\n");
  return 0;
}

int mpc60x_set_exception  (const rtems_raw_except_connect_data* except)
{
    unsigned int level;

    if (current_ppc_cpu == PPC_750) {
      if (!mpc750_vector_is_valid(except->exceptIndex)){
        return 0;
      }
      goto exception_ok;
    }
    if (current_ppc_cpu == PPC_604) {
      if (!mpc604_vector_is_valid(except->exceptIndex)){
        return 0;
      }
      goto exception_ok;
    }
    printk("Please complete libcpu/powerpc/XXX/raw_exception.c\n");
    return 0;
```

NB : re readding the code I should have done a switch... TSSSS.A future patche
     I think.
