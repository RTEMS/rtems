@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Requirements

A fairly large amount of disk space is required to perform the build of the
GNU C/C++ Cross Compiler Tools for RTEMS. The following table may help in
assessing the amount of disk space required for your installation: 

@example
+----------------------------------------+------------------------------+
|                Component               |     Disk Space Required      |
+----------------------------------------+------------------------------+
|          archive directory             |          30 Mbytes           |
|          tools src unzipped            |          77 Mbytes           |
|    each individual build directory     |    120 Mbytes worst case     |
|       each installation directory      |         20-60 Mbytes         |
+----------------------------------------+------------------------------+
@end example

The instructions in this manual should work on any computer running
a UNIX variant.  Some native GNU tools are used by this procedure
including:

@itemize @bullet
@item GCC
@item GNU make
@end itemize

In addition, some native utilities may be deficient for building 
the GNU tools.
