@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Building the GDB Debugger

GDB is not currently RTEMS aware. The following configurations have been
successfully used with RTEMS applications. 

@section GDB with Sparc Instruction Simulation (SIS)

@subsection Unarchive the gdb distribution

Use the following commands to unarchive the gdb distribution:

@example
cd tools
tar xzf ../arc/gdb-980122.tar.gz
@end example

The directory gdb-980122 is created under the tools directory.

@subsection Make the build directory

Create a build directory for the SIS Debugger

@example
cd tools
mkdir build-sis
@end example

@subsection Configure for the build

Configure the general gdb distribution for Sparc Instruction Simulation

@example
cd tools/build-sis
../gdb-980122/configure --target-sparc-erc32-aout \
--program-prefix=sparc-rtems- \
--disable-gdbtk \
--with-targets=all \
--prefix=<INSTALL_POINT_FOR_SIS>
@end example

Where:

<INSTALL_POINT_FOR_SIS> is a unique location where the gdb with SIS will be
created. 

@subsection Make the debugger

From tools/build-sis run:

@example
gmake
@end example


@section GDB with PowerPC instruction Simulation(PSIM)

@subsection Unarchive the gdb distribution

Use the following commands to unarchive the gdb distribution:

@example
cd tools
tar xzf ../arc/gdb-980122.tar.gz
@end example

The directory gdb-980122 is created under the tools directory.


@subsection Make the build directory

Create a build directory for the SIS Debugger

@example
cd tools
mkdir build-ppc
@end example

@subsection Configure for the build

Configure the general gdb distribution for PowerPC Instruction Simulation

@example
cd tools/build-ppc
../gdb-980122/configure --host=i486-linux \
      --target=powerpc-unknown-eabi \
      --program-prefix=powerpc-rtems- \
      --enable-sim-powerpc \
      --enable-sim-timebase \
      --enable-sim-inline \
      --enable-sim-hardware \
--prefix=<INSTALL_POINT_FOR_PPC>
@end example

Where:

<INSTALL_POINT_FOR_PPC> is a unique location where the gdb with PSIM will
be created. 


@subsection Make the debugger

From tools/build-ppc run:

@example
gmake
@end example


@section GDB with Dink32

@subsection Unarchive the gdb distribution

Use the following commands to unarchive the gdb distribution:

@example
cd tools
tar xzf ../arc/gdb-980122.tar.gz
@end example

The directory gdb-980122 is created under the tools directory.


@subsection Make the build directory

Create a build directory for the SIS Debugger

@example
cd tools
mkdir build-dink32
@end example

@subsection Replace dink32-rom.c

Obtain a valid copy of dink32-rom.c from RTEMS site.

Replace the copy of dink32-rom.c that came with the gdb-980122
distribution. It is located in:

@example
tools/gdb-980122/gdb/dink32-rom.c
@end example


@subsection Configure for the build

Configure the general gdb distribution for Sparc Instruction Simulation

@example
cd tools/build-dink32
../gdb-980122/configure --target-powerpc-elf \
--program-prefix=powerpc-rtems- \
--prefix=<INSTALL_POINT_FOR_DINK32>
--with-targets=all \
@end example

Where:

<INSTALL_POINT_FOR_DINK32> is a unique location where the gdb Dink32 will
be created. 

@subsection Make the debugger

From tools/build-dink32 run:

@example
gmake
@end example

