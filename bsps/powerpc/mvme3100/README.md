mvme3100
========
 Valuable information was obtained from the following drivers

    linux: (BCM54xx) Maciej W. Rozycki, Amy Fong.

 This BSP also builds on top of the work of others who have contributed
 to similar RTEMS (powerpc) BSPs, most notably Eric Valette, Eric Norum
 and others.

 This BSP was produced by the Stanford Linear Accelerator Center, 
 Stanford University under contract with the US Department of Energy.

LICENSE
-------
See ./LICENSE file.

Note that not all files that are part of this BSP were written by
myself. Consult individual file headers for copyright
and authorship information.


HARDWARE SUPPORT
---------------
(some of the headers mentioned below contain more
detailed information)

NOTE:  The BSP supports the mvme3100 board.

WARNING: It is extremely important that a MOTLoad "waitProbe", "netShut"
         sequence be executed before booting RTEMS. Otherwise, network
         interface interrupt handlers installed by MOTLoad may cause memory
         corruption

CONSOLE: 2 serial devices, UART driver from 'shared' - no surprises
       ("/dev/ttyS0", [="/dev/console"], "/dev/ttyS1"). (Only
       /dev/ttyS0 is accessible from the front panel.)

CLOCK: Decrementer, same as other PPC BSPs. (FIXME: a openpic timer
       could be used.) The bookE decrementer is slightly different
	   from the classic PPC decrementer but the differences are
	   hidden from the user.

PIC (interrupt controller) (bsp/irq.h): OpenPIC integrated with
       the MPC8540. (see also: bsp/openpic.h).

PCI (bsp/pci.h): 
       In addition to rtems' PCI API, a call is available to scan
       all devices executing a user callback on each device.
       BSP_pciConfigDump() is a convenience wrapper dumping essential
       information (IDs, BAs, IRQ pin/line) to the console or a file.

MEMORY MAP: MotLoad; all addresses (MEM + I/O) read from PCI config. space
       are CPU addresses. For sake of portability, drivers should still
       use the _IO_BASE, PCI_MEM_BASE, PCI_DRAM_OFFSET constants.

NVRAM: No NVRAM.

FLASH (bsp/flashPgm.h): Routines to write flash. Highest level
      wrapper writes a file to flash.
	  NOTE: Writing to flash is disabled by default;
	        call BSP_flashWriteEnable().

I2C (bsp.h, rtems/libi2c.h, libchip/i2c-xxx.h):  temp. sensor, eeprom
      and real-time clock (RTC) are available as device files (bsp.h);
	  lower-level interface is provided by libi2c.h.

	  Available i2c devices are:

				/dev/i2c0.vpd-eeprom
				/dev/i2c0.usr-eeprom
				/dev/i2c0.usr1-eeprom
				/dev/i2c0.ds1621
				/dev/i2c0.ds1621-raw
				/dev/i2c0.ds1375-raw

      You can e.g., read the board temperature:
			    fd = open("/dev/i2c0.ds1621",O_RDONLY)
				read(fd,&temp,1)
				close(fd);
				printf("Board Temp. is %idegC\n",(int)temp);

VME: (bsp/VME.h, bsp/vme_am_defs.h, bsp/VMEDMA.h).
      *always* use VME.h API, if possible; do *not* use chip driver
	  (vmeTsi148.h) directly unless you know what you are
	  doing (i.e., if you need specific features provided by the particular
	  chip)

      VMEConfig.h should not be used by applications as it makes them
      dependent on BSP internals. VMEConfig.h is intended to be used
	  by BSP designers only.

      VME interrupt priorities: the VME bridge(s) do not implement
      priorities in hardware.
      However, on the 3100 multiple physical interrupt
      lines/wires connect the VME bridge to the PIC. Hence, it is possible
      to assign the different wires different priorities at the PIC
      (see bsp/openpic.h) and to route VME interrupts to different
	  wires according to their priority.
	  You need to call driver specific routines
      for this (vmeXXXIntRoute()), however (for driver-specific API
	  consult bsp/vmeTsi148.h).

	  For VME DMA *always* use the bsp/VMEDMA.h API. DO NOT use
	  chip-specific features. Applications written using the bsp/VMEDMA.h
	  API are portable between the UniverseII and the Tsi148.

HARDWARE TIMERS: (bsp/openpic.h). Programmable general-purpose 
      timers. Routines are provided to setup, start and stop
	  GPTs. The setup routine allows for specifying single-shot or periodic
	  mode and dispatches a user ISR when the GPT expires.

NETWORK: (bsp/if_tsec_pub.h). In addition to the standard bsdnet
      'attach' function the driver offers a low-level API that
	  can be used to implement alternate communication links
	  which are totally decoupled from BSDNET.

	  Consult 'KNOWN_PROBLEMS'.

VPD: (bsp/vpd.h). The board's VPD (vital-product-data such as S/N,
      MAC addresses and so forth) can be retrieved.

BOOTING: BSP has a relocator-header. Clear MSR and jump to the first
      instruction in the binary. R3 and R4, if non-null, point to the
      start/end of an optional command line string that is copied into
      BSP_commandline_string. The BSP is compatible with 'netboot'.

Have fun.

-- Till Straumann <strauman@slac.stanford.edu>, 2007.


Known Problems
==============
I have observed what seem to be strange
initialization problems with the ethernet
driver:

I usually configure RTEMS networking by
BOOTP (the problem has nothing to do with
BOOTP but I just want to describe my
environment). Sometimes (it can actually
happen quite frequently, like 1 out of 4
attempts but since yesterday when I decided
to hunt this down more systematically
the problem seems to have gone - typical!)
networking fails to initialize properly:

BOOTP requests are sent (to the MAC),
TX interrupts occur and the TX MIB
counters increment - i.e., everything
seems normal but no data can be seen on
the wire. Also, even though we are on
a quite busy network, the receiver
doesn't see anything, i.e., 0 RX
interrupts, RX MIB counters for broadcast
packets remain steady at zero etc.
In brief, everyting seems normal at the
MAC and higher layers but no connection
to the wire seems to be established.

Some further tests reveal (system under
test is in the 'bad' state):
 1 communication with the BCM5461 PHY
   is normal. Registers can be read/written
   and everything seems normal. In particular,
   the link status is reported OK: disconnect
   the cable and MII - BMSR bit 1<<2 is clear,
   reconnect the cable and BMSR[2] is set.
   Restart autoneg, the link goes and comes
   back after a short while.
 2 setting the loopback bit in the TSEC's
   MACCFG1 register correctly feeds packets
   back into the RX, RX MIB counters now
   increment and indicate data flow.
   There are RX interrupts and all indicates
   (I haven't actually looked at RX packet
   data) that the RX would work normally.
   After switching MACCFG1[LOOP_BACK] off
   no RX traffic can be seen anymore.
 3 resetting the PHY (BMCR = 0x8000) and/or
   restarting autoneg (BMCR = 0x1200) seems
   to perform the desired action (registers
   take on expected values) but still no luck
   with communication all the way through
   to the wire.

Especially point 2 seems to indicate that
the problem is likely to be between the
wire and the MAC somewhere but re-setting
the PHY doesn't change things. Analysis is
much complicated by the fact that there
is no documentation on the BCM5461 chip
available.

Noteworthy is also that if the system
initializes OK then it continues to work
normally; if initialization fails then
only resetting the board and restarting
helps.

I wanted to test if it makes a difference
if MotLoad used the chip prior to RTEMS
being booted (in case MotLoad did some
magic step during initialization) but 
before I could really test this the
problem went away.

Big Mystery...

12/12/2007, T.S.
