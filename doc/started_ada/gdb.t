@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the GNU Debugger

GDB is not currently RTEMS aware. The following configurations have been
successfully used with RTEMS applications:

@itemize @bullet
@item Sparc Instruction Simulator (SIS)
@item PowerPC Instruction Simulator (PSIM)
@item DINK32
@end itemize

Other configurations of gdb have successfully been used by RTEMS users
but are not documented here.

@section Unarchive the gdb Distribution

Use the following commands to unarchive the gdb distribution:

@example
cd tools
tar xzf ../archive/@value{GDB-TAR}
@end example

The directory @value{GDB-UNTAR} is created under the tools directory.

@c 
@c  GDB GNAT Patch
@c

@section Apply GNAT Patch to GDB

@ifclear GDB-GNATPATCH 
No GNAT specific patches are required for @value{GDB-VERSION} to
support @value{RTEMS-VERSION} and @value{GNAT-VERSION}.
@end ifclear

@ifset GDB-GNATPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{GDB-UNTAR}
zcat archive/@value{GDB-GNATPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/@value{GDB-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file.

To see the files that have been modified use the sequence:

@example
cd tools/@value{GDB-UNTAR}
find . -name "*.orig" -print
@end example

The files that are found, have been modified by the patch file.

@end ifset

@c
@c  GDB RTEMS Patch
@c

@section Apply RTEMS Patch to GDB

@ifclear GDB-RTEMSPATCH
No RTEMS specific patches are required for @value{GDB-VERSION} to
support @value{RTEMS-VERSION}.
@end ifclear

@ifset GDB-RTEMSPATCH

Apply the patch using the following command sequence:

@example
cd tools/@value{GDB-UNTAR}
zcat archive/@value{GDB-RTEMSPATCH} | patch -p1
@end example

Check to see if any of these patches have been rejected using the following
sequence:

@example
cd tools/@value{GDB-UNTAR}
find . -name "*.rej" -print
@end example

If any files are found with the .rej extension, a patch has been rejected.
This should not happen with a good patch file.

To see the files that have been modified use the sequence:

@example
cd tools/@value{GDB-UNTAR}
find . -name "*.orig" -print
@end example

The files that are found, have been modified by the patch file.

@end ifset


@section GDB with Sparc Instruction Simulation (SIS)

@subheading Make the Build Directory

Create a build directory for the SIS Debugger

@example
cd tools
mkdir build-sis
@end example

@subheading Configure for the Build

Configure the GNU Debugger for the
Sparc Instruction Simulator (SIS):

@example
cd tools/build-sis
../@value{GDB-UNTAR}/configure --target-sparc-erc32-aout \
    --program-prefix=sparc-rtems- \
    --disable-gdbtk \
    --enable-targets=all \
    --prefix=<INSTALL_POINT_FOR_SIS>
@end example

Where <INSTALL_POINT_FOR_SIS> is a unique location where the gdb
with SIS will be created. 

@subheading Make the Debugger

From tools/build-sis execute the following command sequence:

@example
make all install
@end example

NOTE: The @code{make} utility used should be GNU make.

@section GDB with PowerPC Instruction Simulator

@subheading Make the Build Directory

Create a build directory for the SIS Debugger

@example
cd tools
mkdir build-ppc
@end example

@subheading Configure for the Build

Configure the GNU Debugger for the PowerPC
Instruction Simulator (PSIM):

@example
cd tools/build-ppc
../@value{GDB-UNTAR}/configure \
      --target=powerpc-unknown-eabi \
      --program-prefix=powerpc-rtems- \
      --enable-sim-powerpc \
      --enable-sim-timebase \
      --enable-sim-inline \
      --enable-sim-hardware \
      --enable-targets=all \
      --prefix=<INSTALL_POINT_FOR_PPC>
@end example

Where <INSTALL_POINT_FOR_PPC> is a unique location where the gdb
with PSIM will be created. 


@subheading Make the Debugger

From tools/build-ppc execute the following command sequence:

@example
make all install
@end example

NOTE: The @code{make} utility used should be GNU make.

@section GDB for DINK32

@subheading Make the Build Directory

Create a build directory for the DINK32 Debugger

@example
cd tools
mkdir build-dink32
@end example

@subheading Configure for the Build

Configure the GNU Debugger to communicate with
the DINK32 ROM monitor:

@example
cd tools/build-dink32
../@value{GDB-UNTAR}/configure --target-powerpc-elf \
    --program-prefix=powerpc-rtems- \
    --enable-targets=all \
    --prefix=<INSTALL_POINT_FOR_DINK32>
@end example

Where <INSTALL_POINT_FOR_DINK32> is a unique location where the
gdb Dink32 will be created. 

@subheading Make the Debugger

From tools/build-dink32 execute the following command sequence:

@example
make all install
@end example

NOTE: The @code{make} utility used should be GNU make.
