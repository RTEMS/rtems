@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Mounting and Unmounting Filesystems

@section Mount Points

The following is the list of the characteristics of a mount point:

@itemize @bullet

@item The mount point must be a directory. It may have files and other
directories under it. These files and directories will be hidden when the
filesystem is mounted.

@item The task must have read/write/execute permissions to the mount point
or the mount attempt will be rejected.

@item Only one filesystem can be mounted to a single mount point.

@item The Root of the mountable filesystem will be referenced by the name
of the mount point after the mount is complete.

@end itemize

@section Mount Table Chain

The mount table chain is a dynamic list of structures that describe
mounted filesystems a specific points in the filesystem hierarchy. It is
initialized to an empty state during the base filesystem initialization.
The mount operation will add entries to the mount table chain. The
un-mount operation will remove entries from the mount table chain.

Each entry in the mount table chain is of the following type:

@example
struct rtems_filesystem_mount_table_entry_tt
@{
   Chain_Node                             Node;
   rtems_filesystem_location_info_t       mt_point_node;
   rtems_filesystem_location_info_t       mt_fs_root;
   int                                    options;
   void                                  *fs_info;

   rtems_filesystem_limits_and_options_t  pathconf_limits_and_options;

  /*
   *  When someone adds a mounted filesystem on a real device,
   *  this will need to be used.
   *
   *  The best option long term for this is probably an
   *  open file descriptor.
   */
   char                                  *dev;
@};
@end example

@section Adding entries to the chain during mount

When a filesystem is mounted, its presence and location in the file
system hierarchy is recorded in a dynamic list structure known as a chain.
A unique rtems_filesystem_mount_table_entry_tt structure is logged for
each filesystem that is mounted. This includes the base filesystem.

@section Removing entries from the chain during unmount

When a filesystem is dismounted its entry in the mount table chain is
extracted and the memory for this entry is freed.

