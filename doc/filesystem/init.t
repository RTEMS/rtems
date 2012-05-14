@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter System Initialization

After the RTEMS initialization is performed, the application's
initialization will be performed. Part of initialization is a call to
rtems_filesystem_initialize(). This routine will mount the `In Memory File
System' as the base filesystem.  Mounting the base filesystem consists
of the following:

@itemize @bullet

@item Initialization of mount table chain control structure

@item Allocation of a @code{jnode} structure that will server as the root node
of the `In Memory Filesystem'

@item Initialization of the allocated @code{jnode} with the appropriate OPS,
directory handlers and pathconf limits and options.

@item Allocation of a memory region for filesystem specific global
management variables

@item Creation of first mount table entry for the base filesystem

@item Initialization of the first mount table chain entry to indicate that
the mount point is NULL and the mounted filesystem is the base file
system

@end itemize


After the base filesystem has been mounted, the following operations are
performed under its directory structure:

@itemize @bullet

@item Creation of the /dev directory

@item Registration of devices under /dev directory

@end itemize

@section Base Filesystem

RTEMS initially mounts a RAM based file system known as the base file system.  
The root directory of this file system tree serves as the logical root of the
directory hierarchy (Figure 3). Under the root directory a `/dev' directory
is created under which all I/O device directories and files are registered as
part of the file system hierarchy.

@example
Figure of the tree structure goes here.
@end example

A RAM based file system draws its management resources from memory. File and
directory nodes are simply allocated blocks of memory. Data associated with
regular files is stored in collections of memory blocks. When the system is
turned off or restarted all memory-based components of the file system are
lost.

The base file system serves as a starting point for the mounting of file
systems that are resident on semi-permanent storage media. Examples of such
media include non- volatile memory, flash memory and IDE hard disk drives
(Figure 3). File systems of other types will be mounted onto mount points
within the base file system or other file systems that are subordinate to the
base file system. The framework set up under the base file system will allow
for these new file system types and the unique data and functionality that is
required to manage the future file systems.

@subsection Base Filesystem Mounting

At present, the first file system to be mounted is the `In Memory File
System'. It is mounted using a standard MOUNT() command in which the mount
point is NULL.  This flags the mount as the first file system to be
registered under the operating system and appropriate initialization of file
system management information is performed (See figures 4 and 5). If a
different file system type is desired as the base file system, alterations
must be made to base_fs.c. This routine handles the mount of the base file
system.


@example
Figure of the mount table chain goes here.
@end example


Once the root of the base file system has been established and it has been
recorded as the mount point of the base file system, devices are integrated
into the base file system. For every device that is configured into the
system (See ioman.c) a device registration process is performed. Device
registration produces a unique dev_t handle that consists of a major and
minor device number. In addition, the configuration information for each
device contains a text string that represents the fully qualified pathname to
that device's place in the base file system's hierarchy. A file system node
is created for the device along the specified registration path.


@example
Figure  of the Mount Table Processing goes here.
@end example


Note: Other file systems can be mounted but they are mounted onto points
(directory mount points) in the base file system.


