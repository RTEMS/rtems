@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c


@chapter Base File System

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

@section Base File System Mounting

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


@subsection Base File System Node Structure and Function

Each regular file, device, hard link, and directory is represented by a data
structure called a @code{jnode}. The @code{jnode} is formally represented by the
structure:

@example
struct IMFS_jnode_tt @{
  Chain_Node          Node;             /* for chaining them together */
  IMFS_jnode_t       *Parent;           /* Parent node */
  char                name[NAME_MAX+1]; /* "basename" */
  mode_t              st_mode;          /* File mode */
  nlink_t             st_nlink;         /* Link count */
  ino_t               st_ino;           /* inode */

  uid_t               st_uid;           /* User ID of owner */
  gid_t               st_gid;           /* Group ID of owner */
  time_t              st_atime;         /* Time of last access */
  time_t              st_mtime;         /* Time of last modification */
  time_t              st_ctime;         /* Time of last status change */
  IMFS_jnode_types_t  type;             /* Type of this entry */
  IMFS_typs_union     info;
@};
@end example

The key elements of this structure are listed below together with a brief
explanation of their role in the file system.

@table @b

@item Node
This element exists simply to allow the entire @code{jnode} structure to be
included in a chain.

@item Parent
A pointer to another @code{jnode} structure that is the logical parent of the
node in which it appears. There are circumstances that will produce a null
parent pointer within a @code{jnode}. This can occur when a hard link is
created to a file and the file is then removed without removing the hard
link.

@item name
The name of this node within the file system hierarchical tree. Example:  If
the fully qualified pathname to the @code{jnode} was /a/b/c, the @code{jnode} name
field would contain the null terminated string "c"

@item st_mode
The standard Unix access permissions for the file or directory.

@item st_nlink
The number of hard links to this file. When a @code{jnode} is first created
its link count is set to 1. A @code{jnode} and its associated resources
cannot be deleted unless its link count is less than 1.

@item st_ino
A unique node identification number

@item st_uid
The user ID of the file's owner

@item st_gid
The group ID of the file's owner

@item st_atime
The time of the last access to this file

@item st_mtime
The time of the last modification of this file

@item st_ctime
The time of the last status change to the file

@item type
The indication of node type must be one of the following states:

@itemize @bullet
@item IMFS_DIRECTORY 
@item IMFS_MEMORY_FILE 
@item IMFS_HARD_LINK
@item IMFS_SYM_LINK
@item IMFS_DEVICE 
@end itemize


@item info
This contains a structure that is unique to file type (See IMFS_typs_union
in imfs.h).

@itemize @bullet

@item IMFS_DIRECTORY

An in memory file system directory contains a dynamic chain structure that
records all files and directories that are subordinate to the directory node.

@item IMFS_MEMORY_FILE

Under the in memory file system regular files hold data. Data is dynamically
allocated to the file in 128 byte chunks of memory.  The individual chunks of
memory are tracked by arrays of pointers that record the address of the
allocated chunk of memory. Single, double, and triple indirection pointers
are used to record the locations of all segments of the file.  These
memory-tracking techniques are graphically depicted in figures XXX and XXX of
appendix A.

@item IMFS_HARD_LINK

The IMFS file system supports the concept of hard links to other nodes in the
IMFS file system. These hard links are actual pointers to the memory
associated with other nodes in the file system. This type of link cannot
cross-file system boundaries.

@item IMFS_SYM_LINK

The IMFS file system supports the concept of symbolic links to other nodes in
any file system. A symbolic link consists of a pointer to a character string
that represents the pathname to the target node. This type of link can
cross-file system boundaries.

@item IMFS_DEVICE

All RTEMS devices now appear as files under the in memory file system. On
system initialization, all devices are registered as nodes under the file
system.

@end itemize

@end table


@subsection Node removal constraints for the base files system

@itemize @bullet 

@item If a node is a directory with children it cannot be removed.

@item The root node of the base file system or the mounted file system 
cannot be removed.

@item A node that is a directory that is acting as the mount point of a file 
system cannot be removed.

@item Prior to node removal, decrement the node's link count by one. The 
link count must be less than one to allow for removal of the node.

@end itemize

@subsection Housekeeping

@itemize @bullet 

@item If the global variable rtems_filesystem_current refers to the node that
we are trying to remove, the node_access element of this structure must be
set to NULL to invalidate it.

@item If the node was of IMFS_MEMORY_FILE type, free the memory associated
with the memory file before freeing the node. Use the IMFS_memfile_remove()
function.

@end itemize


@section IMFS

@subsection OPS Table Functions for the In Memory File System (IMFS)

@example

OPS Table Functions             File                    Routine Name

Evalpath Imfs_eval.c IMFS_eval_path()
Evalformake Imfs_eval.c IMFS_evaluate_for_make()
Link Imfs_link.c IMFS_link()
Unlink Imfs_unlink.c IMFS_unlink()
Node_type Imfs_ntype.c IMFS_node_type()
Mknod Imfs_mknod.c IMFS_mknod()
Rmnod Imfs_rmnod.c IMFS_rmnod()
Chown Imfs_chown.c IMFS_chown()
Freenod Imfs_free.c IMFS_freenodinfo()
Mount Imfs_mount.c IMFS_mount()
Fsmount_me Imfs_init.c IMFS_initialize()
Unmount Imfs_unmount.c IMFS_unmount()
Fsunmount_me Imfs_init.c IMFS_fsunmount()
Utime Imfs_utime.c IMFS_utime()
Eval_link Imfs_eval.c IMFS_evaluate_link()
Symlink Imfs_symlink.c IMFS_symlink()
Readlink Imfs_readlink.c IMFS_readlink()
@end example



@subsection Handler Functions for Regular Files of In Memory File System

@example
Handler Function            File            Routine Name

Open Memfile.c Memfile_open()
Close Memfile.c Memfile_close()
Read Memfile.c Memfile_read()
Write Memfile.c Memfile_write()
Ioctl Memfile.c Memfile_ioctl()
Lseek Memfile.c Memfile_lseek()
Fstat Imfs_stat.c IMFS_stat()
Fchmod Imfs_fchmod.c IMFS_fchmod()
Ftruncate Memfile.c Memfile_ftruncate()
Fpathconf NA NULL
Fsync NA NULL
Fdatasync NA NULL
@end example


@subsection Handler Functions for Directories of In Memory File System

@example
Handler Function             File                Routine Name

Open imfs_directory.c Imfs_dir_open()
Close imfs_directory.c Imfs_dir_close()
Read imfs_directory.c Imfs_dir_read()
Write imfs_directory.c NULL
Ioctl imfs_directory.c NULL
Lseek imfs_directory.c Imfs_dir_lseek()
Fstat imfs_directory.c Imfs_dir_fstat()
Fchmod imfs_fchmod.c IMFS_fchmod()
Ftruncate NA NULL
Fpathconf NA NULL
Fsync NA NULL
Fdatasync NA NULL
@end example



@subsection Handler Functions for Devices of In Memory File System

@example
Handler Function          File                  Routine Name

Open deviceio.c Device_open()
Close deviceio.c Device_close()
Read deviceio.c Device_read()
Write deviceio.c Device_write()
Ioctl deviceio.c Device_ioctl()
Lseek deviceio.c Device_lseek()
Fstat imfs_stat.c IMFS_stat()
Fchmod imfs_fchmod.c IMFS_fchmod()
Ftruncate NA NULL
Fpathconf NA NULL
Fsync NA NULL
Fdatasync NA NULL
@end example

