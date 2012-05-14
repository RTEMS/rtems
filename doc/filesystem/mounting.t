@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

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

@table @b
@item Node
The Node is used to produce a linked list of mount table entry nodes.

@item mt_point_node
The mt_point_node contains all information necessary to access the
directory where a filesystem is mounted onto.  This element may contain
memory that is allocated during a path evaluation of the filesystem
containing the mountpoint directory.  The generic code allows this
memory to be returned by unmount when the filesystem identified by
mt_fs_root is unmounted.  

@item mt_fs_root
The mt_fs_root contains all information necessary to identify the root
of the mounted filesystem. The user is never allowed access to this
node by the generic code, but it is used to identify to the mounted 
filesystem where to start evaluation of pathnames at.

@item options
XXX

@item fs_info
The fs_info element is a location available for use by the mounted file
system to identify unique things applicable to this instance of the file
system.  For example the IMFS uses this space to provide node 
identification that is unique for each instance (mounting) of the filesystem.

@item pathconf_limits_and_options
XXX

@item dev
This character string represents the device where the filesystem will reside.

@end table

@section Adding entries to the chain during mount

When a filesystem is mounted, its presence and location in the file
system hierarchy is recorded in a dynamic list structure known as a chain.
A unique rtems_filesystem_mount_table_entry_tt structure is logged for
each filesystem that is mounted. This includes the base filesystem.

@section Removing entries from the chain during unmount

When a filesystem is dismounted its entry in the mount table chain is
extracted and the memory for this entry is freed.































































