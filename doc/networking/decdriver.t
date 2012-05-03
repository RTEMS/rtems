@c 
@c  RTEMS Remote Debugger Server Specifications
@c
@c  Written by: Emmanuel Raguet <raguet@crf.canon.fr>
@c

@chapter DEC 21140 Driver

@section DEC 21240 Driver Introduction

@c XXX add back in cross reference to list of boards.

One aim of our project is to port RTEMS on a standard PowerPC platform.
To achieve it, we have chosen a Motorola MCP750 board. This board includes
an Ethernet controller based on a DEC21140 chip. Because RTEMS has a
TCP/IP stack, we will
have to develop the DEC21140 related ethernet driver for the PowerPC port of
RTEMS. As this controller is able to support 100Mbps network and as there is
a lot of PCI card using this DEC chip, we have decided to first
implement this driver on an Intel PC386 target to provide a solution for using
RTEMS on PC with the 100Mbps network and then to port this code on PowerPC in
a second phase.


The aim of this document is to give some PCI board generalities and
to explain the software architecture of the RTEMS driver. Finally, we will see
what will be done for ChorusOs and Netboot environment .


@section Document Revision History

@b{Current release}: 

@itemize @bullet
@item Current applicable release is 1.0.
@end itemize
@b{Existing releases}:

@itemize @bullet
@item 1.0 : Released the 10/02/98. First version of this document.
@item 0.1 : First draft of this document
@end itemize
@b{Planned releases}:

@itemize @bullet
@item None planned today.
@end itemize

@section DEC21140 PCI Board Generalities

@c XXX add crossreference to PCI Register Figure
This chapter describes rapidely the PCI interface of this Ethernet controller.
The board we have chosen for our PC386 implementation is a D-Link DFE-500TX.
This is a dual-speed 10/100Mbps Ethernet PCI adapter with a DEC21140AF chip.
Like other PCI devices, this board has a PCI device's header containing some
required configuration registers, as shown in the PCI Register Figure.
By reading
or writing these registers, a driver can obtain information about the type of
the board, the interrupt it uses, the mapping of the chip specific registers, ...



On Intel target, the chip specific registers can be accessed via 2
methods : I/O port access or PCI address mapped access. We have chosen to implement
the PCI address access to obtain compatible source code to the port the driver
on a PowerPC target.

@c
@c PCI Device's Configuration Header Space Format
@c

@ifclear use-html
@image{PCIreg,,,PCI Device's Configuration Header Space Format}
@end ifclear

@ifset use-html
@c <IMG SRC="PCIreg.jpg" WIDTH=500 HEIGHT=600 ALT="PCI Device's Configuration Header Space Format">
@html
 <IMG SRC="PCIreg.jpg" ALT="PCI Device's Configuration Header Space Format">
@end html
@end ifset


@c XXX add crossreference to PCI Register Figure

On RTEMS, a PCI API exists. We have used it to configure the board. After initializing
this PCI module via the @code{pci_initialize()} function, we try to detect
the DEC21140 based ethernet board. This board is characterized by its Vendor
ID (0x1011) and its Device ID (0x0009). We give these arguments to the
@code{pcib_find_by_deviceid}
function which returns , if the device is present, a pointer to the configuration
header space (see PCI Registers Fgure). Once this operation performed,
the driver
is able to extract the information it needs to configure the board internal
registers, like the interrupt line, the base address,... The board internal
registers will not be detailled here. You can find them in @b{DIGITAL
Semiconductor 21140A PCI Fast Ethernet LAN Controller
- Hardware Reference Manual}.

@c fix citation


@section RTEMS Driver Software Architecture

In this chapter will see the initialization phase, how the controller uses the
host memory and the 2 threads launched at the initialization time. 


@subsection Initialization phase

The DEC21140 Ethernet driver keeps the same software architecture than the other
RTEMS ethernet drivers. The only API the programmer can use is the @code{rtems_dec21140_driver_attach}
@code{(struct rtems_bsdnet_ifconfig *config)} function which
detects the board and initializes the associated data structure (with registers
base address, entry points to low-level initialization function,...), if the
board is found. 

Once the attach function executed, the driver initializes the DEC
chip. Then the driver connects an interrupt handler to the interrupt line driven
by the Ethernet controller (the only interrupt which will be treated is the
receive interrupt) and launches 2 threads : a receiver thread and a transmitter
thread. Then the driver waits for incoming frame to give to the protocol stack
or outcoming frame to send on the physical link.


@subsection Memory Buffer

@c XXX add cross reference to Problem
This DEC chip uses the host memory to store the incoming Ethernet frames and
the descriptor of these frames. We have chosen to use 7 receive buffers and
1 transmit buffer to optimize memory allocation due to cache and paging problem
that will be explained in the section @b{Encountered Problems}.


To reference these buffers to the DEC chip we use a buffer descriptors
ring. The descriptor structure is defined in the Buffer Descriptor Figure.
Each descriptor
can reference one or two memory buffers. We choose to use only one buffer of
1520 bytes per descriptor.


The difference between a receive and a transmit buffer descriptor
is located in the status and control bits fields. We do not give details here,
please refer to the [DEC21140 Hardware Manual].

@c 
@c Buffer Descriptor
@c

@ifclear use-html
@image{recvbd,,,"Buffer Descriptor"}
@end ifclear

@ifset use-html
@c <IMG SRC="recvbd.jpg" WIDTH=500 HEIGHT=600 ALT="Buffer Descriptor">
@html
<IMG SRC="recvbd.jpg" ALT="Buffer Descriptor">
@end html
@end ifset



@subsection Receiver Thread

This thread is event driven. Each time a DEC PCI board interrupt occurs, the
handler checks if this is a receive interrupt and send an event ``reception''
to the receiver thread which looks into the entire buffer descriptors ring the
ones that contain a valid incoming frame (bit OWN=0 means descriptor belongs
to host processor). Each valid incoming ethernet frame is sent to the protocol
stack and the buffer descriptor is given back to the DEC board (the host processor
reset bit OWN, which means descriptor belongs to 21140).


@subsection Transmitter Thread

This thread is also event driven. Each time an Ethernet frame is put in the
transmit queue, an event is sent to the transmit thread, which empty the queue
by sending each outcoming frame. Because we use only one transmit buffer, we
are sure that the frame is well-sent before sending the next.


@section Encountered Problems

On Intel PC386 target, we were faced with a problem of memory cache management.
Because the DEC chip uses the host memory to store the incoming frame and because
the DEC21140 configuration registers are mapped into the PCI address space,
we must ensure that the data read (or written) by the host processor are the
ones written (or read) by the DEC21140 device in the host memory and not old
data stored in the cache memory. Therefore, we had to provide a way to manage
the cache. This module is described in the document @b{RTEMS
Cache Management For Intel}. On Intel, the
memory region cache management is available only if the paging unit is enabled.
We have used this paging mechanism, with 4Kb page. All the buffers allocated
to store the incoming or outcoming frames, buffer descriptor and also the PCI
address space of the DEC board are located in a memory space with cache disable.


Concerning the buffers and their descriptors, we have tried to optimize
the memory space in term of allocated page. One buffer has 1520 bytes, one descriptor
has 16 bytes. We have 7 receive buffers and 1 transmit buffer, and for each,
1 descriptor : (7+1)*(1520+16) = 12288 bytes = 12Kb = 3 entire pages. This
allows not to lose too much memory or not to disable cache memory for a page
which contains other data than buffer, which could decrease performance.


@section ChorusOs DEC Driver

Because ChorusOs is used in several Canon CRF projects, we must provide such
a driver on this OS to ensure compatibility between the RTEMS and ChorusOs developments.
On ChorusOs, a DEC driver source code already exists but only for a PowerPC
target. We plan to port this code (which uses ChorusOs API) on Intel target.
This will allow us to have homogeneous developments. Moreover, the port of the
development performed with ChorusOs environment to RTEMS environment will be
easier for the developers.


@section Netboot DEC driver

We use Netboot tool to load our development from a server to the target via
an ethernet network. Currently, this tool does not support the DEC board. We
plan to port the DEC driver for the Netboot tool.


But concerning the port of the DEC driver into Netboot, we are faced
with a problem : in RTEMS environment, the DEC driver is interrupt or event
driven, in Netboot environment, it must be used in polling mode. It means that
we will have to re-write some mechanisms of this driver.


@section List of Ethernet cards using the DEC chip

Many Ethernet adapter cards use the Tulip chip. Here is a non exhaustive list
of adapters which support this driver :

@itemize @bullet
@item Accton EtherDuo PCI.
@item Accton EN1207 All three media types supported.
@item Adaptec ANA6911/TX 21140-AC.
@item Cogent EM110 21140-A with DP83840 N-Way MII transceiver.
@item Cogent EM400 EM100 with 4 21140 100mbps-only ports + PCI Bridge.
@item Danpex EN-9400P3.
@item D-Link DFE500-Tx 21140-A with DP83840 transceiver.
@item Kingston EtherX KNE100TX 21140AE.
@item Netgear FX310 TX 10/100 21140AE. 
@item SMC EtherPower10/100 With DEC21140 and 68836 SYM transceiver. 
@item SMC EtherPower10/100 With DEC21140-AC and DP83840 MII transceiver. 
Note: The EtherPower II uses the EPIC chip, which requires a different driver. 
@item Surecom EP-320X DEC 21140. 
@item Thomas Conrad TC5048. 
@item Znyx ZX345 21140-A, usually with the DP83840 N-Way MII transciever. Some ZX345
cards made in 1996 have an ICS 1890 transciver instead. 
@item ZNYX ZX348 Two 21140-A chips using ICS 1890 transcievers and either a 21052
or 21152 bridge. Early versions used National 83840 transcievers, but later
versions are depopulated ZX346 boards. 
@item ZNYX ZX351 21140 chip with a Broadcom 100BaseT4 transciever. 
@end itemize

Our DEC driver has not been tested with all these cards, only with the D-Link
DFE500-TX.

@itemize @code{ }
@item @cite{[DEC21140 Hardware Manual] DIGITAL, @b{DIGITAL
Semiconductor 21140A PCI Fast Ethernet LAN Controller - Hardware
Reference Manual}}.

@item @cite{[99.TA.0021.M.ER]Emmanuel Raguet,
@b{RTEMS Cache Management For Intel}}.
@end itemize
