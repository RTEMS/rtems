@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Hardware to Ease Debugging

The items in this category provide information on various hardware 
debugging assistants that are available.

@section MC683xx BDM Support for GDB

Eric Norum (eric@@skatter.usask.ca) has a driver for a parallel
port interface to a BDM module.  This driver has a long history 
and is based on a driver by Gunter Magin (magin@@skil.camelot.de)
which in turn was based on BD32 for DOS by Scott Howard.  Eric Norum
and Chris Johns (ccj@@acm.org) have put together a package containing
everything you need to use this BDM driver including software, PCB layouts,
and machining drawings.  From the README:

"This package contains everything you need to be able to run GDB on
Linux and control a Motorola CPU32+ (68360) or Coldfire (5206, 5206e, or
5207) target through a standard PC parallel port."

Information on this BDM driver is available at the following URL:

@example
http://www.calm.hw.ac.uk/davidf/coldfire/gdb-bdm-linux.htm
@end example

The package is officially hosted at Eric Norum's ftp site:

@example
ftp://skatter.usask.ca/pub/eric/BDM-Linux-gdb/
@end example

Peter Shoebridge (peter@@zeecube.com) has ported the Linux
parallel port BDM driver from Eric Norum to Windows NT.  It is
available at http://www.zeecube.com/bdm.

The efi332 project has a home-built BDM module and gdb backend for 
Linux.  See http://efi332.eng.ohio-state.edu/efi332/hardware.html)
for details.  The device driver and gdb backend are based on those
by Gunter Magin (magin@@skil.camelot.de) available from
ftp.lpr.e-technik.tu-muenchen.de.

Pavel Pisa (pisa@@cmp.felk.cvut.cz) has one available at
http://cmp.felk.cvut.cz/~pisa/m683xx/bdm_driver.html.

Huntsville Microsystems (HMI) has GDB support for their BDM module
available upon request.  It is also available from their ftp site:
ftp://ftp.hmi.com/pub/gdb

The Macraigor OCD BDM module has a driver for Linux
written by Gunter Magin (magin@@skil.camelot.de).  
No URLs yet.

Finally, there is a overview of BDM at the following URL: 
http://cmp.felk.cvut.cz/~pisa/m683xx/bdm_driver.html.

Information in this section from:

@itemize @bullet
@item Brendan Simon <brendan@@dgs.monash.edu.au>
@item W Gerald Hicks <wghicks@@bellsouth.net>
@item Chris Johns <ccj@@acm.org>
@item Eric Norum <eric@@skatter.usask.ca>
@item Gunter Magin <magin@@skil.camelot.de>

@end itemize


@section MPC8xx BDM Support for GDB

@c "Adrian Bocaniciu" <a.bocaniciu@computer.org> has a driver
@c for NT and is willing to share it but he needs to be emailed
@c privately since he needs to explain somethings about it.
@c It has been used for over a year as of 08/18/99.

Christian Haan <chn@@intego.de> has written a driver for FreeBSD 
based for"a slightly changed ICD BDM module (because of changes
in the BDM interface on the PowerPC)" that "probably will work with
the PD module too."  His work is based on the M68K BDM work by
Gunter Magin (Gunter.Magin@@skil.camelot.de) and 
the PPC BDM for Linux work by Sergey Drazhnikov (swd@@agua.comptek.ru).
This is not yet publicly available.

Sergey Drazhnikov (swd@@agua.comptek.ru) has written a PPC BDM driver for
Linux.  Information is available at http://cyclone.parad.ru/ppcbdm.

Huntsville Microsystems (HMI) has GDB support for their BDM module
available upon request.  It is also available from their ftp site:
ftp://ftp.hmi.com/pub/gdb

GDB includes support for a set of primitives to support the Macraigor
Wiggler (OCD BDM).  Unfortunately, this requires the use of a
proprietary interface and is supported only on Windows.  This forces
one to use CYGWIN.  Reports are that this results in a slow
interface.  Scott Howard (http://www.objsw.com) has announced 
that support for the gdb+wiggler combination under DJGPP which should
run significantly faster.

@itemize @bullet
@item Leon Pollak <leonp@@plris.com>
@item Christian Haan <chn@@intego.de> 
@end itemize

