@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter System Initialization

After the RTEMS initialization is performed, the application's
initialization will be performed. Part of initialization is a call to
rtems_filesystem_initialize(). This routine will mount the `In Memory File
System' as the base file system.  Mounting the base file system consists
of the following:

@itemize @bullet

@item Initialization of mount table chain control structure

@item Allocation of a -jnode- structure that will server as the root node
of the `In Memory File System'

@item Initialization of the allocated -jnode- with the appropriate OPS,
directory handlers and pathconf limits and options.

@item Allocation of a memory region for file system specific global
management variables

@item Creation of first mount table entry for the base file system

@item Initialization of the first mount table chain entry to indicate that
the mount point is NULL and the mounted file system is the base file
system

@end itemize


After the base file system has been mounted, the following operations are
performed under its directory structure:

@itemize @bullet

@item Creation of the /dev directory

@item Registration of devices under /dev directory

@end itemize

