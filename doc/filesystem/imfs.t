
@chapter In-Memory File System 

This chapter describes the In-Memory File System.

@section OPS Table Functions

OPS table functions are defined in a rtems_filesystem_operations_table
structure.  It defines functions that are specific to a given file system.
One table exists for each file system that is supported in the RTEMS
configuration. The structure definition appears below and is followed by
general developmental information on each of the functions contained in this
function management structure.

@example
typedef struct @{
  rtems_filesystem_evalpath_t        evalpath;
  rtems_filesystem_evalmake_t        evalformake;
  rtems_filesystem_link_t            link;
  rtems_filesystem_unlink_t          unlink;
  rtems_filesystem_node_type_t       node_type;
  rtems_filesystem_mknod_t           mknod;
  rtems_filesystem_rmnod_t           rmnod;
  rtems_filesystem_chown_t           chown;
  rtems_filesystem_freenode_t        freenod;
  rtems_filesystem_mount_t           mount;
  rtems_filesystem_fsmount_me_t      fsmount_me;
  rtems_filesystem_unmount_t         unmount;
  rtems_filesystem_fsunmount_me_t    fsunmount_me;
  rtems_filesystem_utime_t           utime;
  rtems_filesystem_evaluate_link_t   eval_link;
  rtems_filesystem_symlink_t         symlink;
@} rtems_filesystem_operations_table;
@end example



@c
@c
@c

@page

@subsection evalpath()

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX


@c
@c
@c

@page

@subsection evalformake()

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX


@c
@c
@c

@page

@subsection link()

@subheading Slot Function:

link

@subheading Arguments:


@example
rtems_filesystem_location_info_t    *to_loc,      /* IN */
rtems_filesystem_location_info_t    *parent_loc,  /* IN */
const char                          *token        /* IN */
@end example

@subheading File:

imfs_link.c

@subheading Development Comments:


This routine is used in the IMFS file system to create a hard-link.

It will first examine the st_nlink count of the node that we are trying to.
If the link count exceeds LINK_MAX an error will be returned.

The name of the link will be normalized to remove extraneous separators from
the end of the name.

IMFS_create_node will be used to create a file system node that will have the
following characteristics:

@itemize @bullet

@item parent that was determined in the link() function in file link.c

@item Type will be set to IMFS_HARD_LINK

@item name will be set to the normalized name

@item mode of the hard-link will be set to the mode of the target node

@end itemize

If there was trouble allocating memory for the new node an error will be
returned.

The st_nlink count of the target node will be incremented to reflect the new
link.

The time fields of the link will be set to reflect the creation time of the
hard-link.


@c @c @c

@page

@subsection unlink()

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX


@c
@c
@c

@page

@subsection node_type()

@subheading Slot Function:

IMFS_node_type()

@subheading Arguments:

@example
rtems_filesystem_location_info_t    *pathloc        /* IN */
@end example

@subheading File:

imfs_ntype.c

@subheading Development Comments:

This routine will locate the IMFS_jnode_t structure that holds ownership
information for the selected node in the file system.

This structure is pointed to by pathloc->node_access.

The IMFS_jnode_t type element indicates one of the node types listed below:

@itemize @bullet

@item RTEMS_FILESYSTEM_DIRECTORY

@item RTEMS_FILESYSTEM_DEVICE

@item RTEMS_FILESYSTEM_HARD_LINK

@item RTEMS_FILESYSTEM_MEMORY_FILE

@end itemize

@c
@c
@c

@page

@subsection mknod()

@subheading Slot Function:

IMFS_mknod()

@subheading Arguments:

@example
const char                          *token,        /* IN */
mode_t                               mode,         /* IN */
dev_t                                dev,          /* IN */
rtems_filesystem_location_info_t    *pathloc       /* IN/OUT */
@end example

@subheading File:

imfs_mknod.c

@subheading Development Comments:

This routine will examine the mode argument to determine is we are trying to
create a directory, regular file and a device node. The creation of other
node types is not permitted and will cause an assert.

Memory space will be allocated for a @code{jnode} and the node will be set up
according to the nodal type that was specified. The IMFS_create_node()
function performs the allocation and setup of the node.

The only problem that is currently reported is the lack of memory when we
attempt to allocate space for the @code{jnode} (ENOMEN).


@c
@c
@c

@page

@subsection rmnod()

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX


@c
@c
@c

@page

@subsection chown()

@subheading Slot Function:

IMFS_chown()

@subheading Arguments:

@example
rtems_filesystem_location_info_t    *pathloc        /* IN */
uid_t                                owner          /* IN */
gid_t                                group          /* IN */
@end example

@subheading File:

imfs_chown.c

@subheading Development Comments:

This routine will locate the IMFS_jnode_t structure that holds ownership
information for the selected node in the file system.

This structure is pointed to by pathloc->node_access.

The st_uid and st_gid fields of the node are then modified. Since this is a
memory based file system, no further action is required to alter the
ownership of the IMFS_jnode_t structure.



@c
@c
@c

@page

@subsection freenod()

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX 


@c
@c
@c

@page

@subsection mount()

@subheading Slot Function:

IMFS_mount()

@subheading Arguments:

@example
rtems_filesystem_mount_table_entry_t   *mt_entry
@end example

@subheading File:

imfs_mount.c

@subheading Development Comments:

This routine provides the file system specific processing required to mount a
file system for the system that contains the mount point. It will determine
if the point that we are trying to mount onto is a node of IMFS_DIRECTORY
type.

If it is the node's info element is altered so that the info.directory.mt_fs
element points to the mount table chain entry that is associated with the
mounted file system at this point. The info.directory.mt_fs element can be
examined to determine if a file system is mounted at a directory. If it is
NULL, the directory does not serve as a mount point. A non-NULL entry
indicates that the directory does serve as a mount point and the value of
info.directory.mt_fs can be used to locate the mount table chain entry that
describes the file system mounted at this point.


@c
@c
@c

@page

@subsection fsmount_me()

@subheading Slot Function:

IMFS_initialize()

@subheading Arguments:

@example
rtems_filesystem_mount_table_entry_t   *mt_entry    
@end example

@subheading File:

imfs_init.c

@subheading Development Comments:

This function is provided with a file system to take care of the internal
file system management details associated with mounting that file system
under the RTEMS environment.

It is not responsible for the mounting details associated the file system
containing the mount point.

The rtems_filesystem_mount_table_entry_t structure contains the key elements
below:

rtems_filesystem_location_info_t         *mt_point_node,

This structure contains information about the mount point. This 
allows us to find the ops-table and the handling functions 
associated with the file system containing the mount point.

rtems_filesystem_location_info_t         *fs_root_node,

This structure contains information about the root node in the file 
system to be mounted. It allows us to find the ops-table and the 
handling functions associated with the file system to be mounted.

rtems_filesystem_options_t                 options,

Read only or read/write access

void                                         *fs_info,

This points to an allocated block of memory the will be used to 
hold any file system specific information of a global nature. This 
allocated region if important because it allows us to mount the 
same file system type more than once under the RTEMS system. 
Each instance of the mounted file system has its own set of global 
management information that is separate from the global 
management information associated with the other instances of the 
mounted file system type.

rtems_filesystem_limits_and_options_t    pathconf_info,

The table contains the following set of values associated with the 
mounted file system:

@itemize @bullet

@item link_max

@item max_canon

@item max_input

@item name_max

@item path_max

@item pipe_buf

@item posix_async_io

@item posix_chown_restrictions

@item posix_no_trunc

@item posix_prio_io

@item posix_sync_io

@item posix_vdisable

@end itemize

These values are accessed with the pathconf() and the fpathconf ()  
functions.

const char                                   *dev

The is intended to contain a string that identifies the device that contains
the file system information. The file systems that are currently implemented
are memory based and don't require a device specification.

If the mt_point_node.node_access is NULL then we are mounting the base file 
system.

The routine will create a directory node for the root of the IMFS file
system.

The node will have read, write and execute permissions for owner, group and
others.

The node's name will be a null string.

A file system information structure(fs_info) will be allocated and
initialized for the IMFS file system. The fs_info pointer in the mount table
entry will be set to point the file system information structure.

The pathconf_info element of the mount table will be set to the appropriate
table of path configuration constants ( IMFS_LIMITS_AND_OPTIONS ).

The fs_root_node structure will be filled in with the following:

@itemize @bullet

@item pointer to the allocated root node of the file system

@item directory handlers for a directory node under the IMFS file system

@item OPS table functions for the IMFS

@end itemize

A 0 will be returned to the calling routine if the process succeeded,
otherwise a 1 will be returned.


@c
@c
@c

@page

@subsection unmount()

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX 


@c
@c
@c

@page

@subsection fsunmount_me()

@subheading Slot Function:

imfs_fsunmount_me()

@subheading Arguments:

@example
rtems_filesystem_mount_table_entry_t   *mt_entry    
@end example

@subheading File:

imfs_fsunmount_me.c

@subheading Development Comments:

XXX 


@c
@c
@c

@page

@subsection utime()

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX 


@c
@c
@c

@page

@subsection eval_link()

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX

@c
@c
@c
@page
@section Handler Table Functions for Regular Files

Handler table functions are defined in a rtems_filesystem_file_handlers_r
structure. It defines functions that are specific to a node type in a given
file system. One table exists for each of the file system's node types. The
structure definition appears below. It is followed by general developmental
information on each of the functions associated with regular files contained
in this function management structure.

@example
typedef struct @{
    rtems_filesystem_open_t           open;
    rtems_filesystem_close_t          close;
    rtems_filesystem_read_t           read;
    rtems_filesystem_write_t          write;
    rtems_filesystem_ioctl_t          ioctl;
    rtems_filesystem_lseek_t          lseek;
    rtems_filesystem_fstat_t          fstat;
    rtems_filesystem_fchmod_t         fchmod;
    rtems_filesystem_ftruncate_t      ftruncate;
    rtems_filesystem_fpathconf_t      fpathconf;
    rtems_filesystem_fsync_t          fsync;
    rtems_filesystem_fdatasync_t      fdatasync;
@} rtems_filesystem_file_handlers_r;
@end example


@c
@c
@c

@page

@subsection open() for Regular File

@subheading Slot Function:

memfile_open()

@subheading Arguments:

@example
rtems_libio_t   *iop,
const char      *pathname,
unsigned32       flag,
unsigned32       mode
@end example

@subheading File:

memfile.c

@subheading Development Comments:

Currently this function is a shell. No meaningful processing is performed and
a success code is always returned.

@c
@c
@c

@page

@subsection close() for Regular File

@subheading Slot Function:

memfile_close()

@subheading Arguments:

@example
rtems_libio_t     *iop
@end example

@subheading File:

memfile.c

@subheading Development Comments:

This routine is a dummy for regular files under the base file system. It
performs a capture of the IMFS_jnode_t pointer from the file control block
and then immediately returns a success status.


@c
@c
@c

@page

@subsection read() for Regular File

@subheading Slot Function:

memfile_read()

@subheading Arguments:

@example
rtems_libio_t     *iop,
void              *buffer,
unsigned32         count
@end example

@subheading File:

memfile.c

@subheading Development Comments:

This routine will determine the @code{jnode} that is associated with this file.

It will then call IMFS_memfile_read() with the @code{jnode}, file position index,
buffer and transfer count as arguments.

IMFS_memfile_read() will do the following:

@itemize @bullet

@item Verify that the @code{jnode} is associated with a memory file

@item Verify that the destination of the read is valid

@item Adjust the length of the read if it is too long

@item Acquire data from the memory blocks associated with the file

@item Update the access time for the data in the file

@end itemize

@c
@c
@c

@page

@subsection write() for Regular File

@subheading Slot Function:

XXX

@subheading Arguments:

XXX
@subheading File:

XXX

@subheading Development Comments:

XXX

@c
@c
@c

@page

@subsection ioctl() for Regular File

@subheading Slot Function:

XXX

@subheading Arguments:

@example
rtems_libio_t     *iop,
unsigned32       command,
void              *buffer
@end example

@subheading File:

memfile.c

@subheading Development Comments:

The current code is a placeholder for future development. The routine returns
a successful completion status.

@c
@c
@c

@page

@subsection lseek() for Regular File

@subheading Slot Function:

Memfile_lseek()

@subheading Arguments:

@example
rtems_libio_t     *iop,
off_t              offset,
int                whence
@end example

@subheading File:

memfile.c

@subheading Development Comments:

This routine make sure that the memory based file is sufficiently large to
allow for the new file position index.

The IMFS_memfile_extend() function is used to evaluate the current size of
the memory file and allocate additional memory blocks if required by the new
file position index. A success code is always returned from this routine.

@c
@c
@c

@page

@subsection fstat() for Regular File

@subheading Slot Function:

IMFS_stat()

@subheading Arguments:

@example
rtems_filesystem_location_info_t   *loc,
struct stat                        *buf
@end example

@subheading File:

imfs_stat.c

@subheading Development Comments:

This routine actually performs status processing for both devices and regular
files.

The IMFS_jnode_t structure is referenced to determine the type of node under
the file system.

If the node is associated with a device, node information is extracted and
transformed to set the st_dev element of the stat structure.

If the node is a regular file, the size of the regular file is extracted from
the node.

This routine rejects other node types.

The following information is extracted from the node and placed in the stat
structure:

@itemize @bullet

@item st_mode

@item st_nlink

@item st_ino

@item st_uid

@item st_gid

@item st_atime

@item st_mtime

@item st_ctime

@end itemize

@c
@c
@c

@page

@subsection fchmod() for Regular File

@subheading Slot Function:

IMFS_fchmod()

@subheading Arguments:

@example
rtems_libio_t     *iop
mode_t              mode
@end example

@subheading File:

imfs_fchmod.c

@subheading Development Comments:

This routine will obtain the pointer to the IMFS_jnode_t structure from the
information currently in the file control block.

Based on configuration the routine will acquire the user ID from a call to
getuid()  or from the IMFS_jnode_t structure.

It then checks to see if we have the ownership rights to alter the mode of
the file.  If the caller does not, an error code is returned.

An additional test is performed to verify that the caller is not trying to
alter the nature of the node. If the caller is attempting to alter more than
the permissions associated with user group and other, an error is returned.

If all the preconditions are met, the user, group and other fields are set
based on the mode calling parameter.

@c
@c
@c

@page

@subsection ftruncate() for Regular File

@subheading Slot Function:

XXX

@subheading Arguments:

XXX
@subheading File:

XXX

@subheading Development Comments:

XXX

@c
@c
@c

@page

@subsection fpathconf() for Regular File

@subheading Slot Function:

NULL

@subheading Arguments:

Not Implemented

@subheading File:

Not Implemented

@subheading Development Comments:

Not Implemented


@c
@c
@c

@page

@subsection fsync() for Regular File

@subheading Slot Function:

XXX

@subheading Arguments:

XXX
@subheading File:

XXX

@subheading Development Comments:

XXX


@c
@c
@c

@page

@subsection fdatasync() for Regular File

@subheading Slot Function:

XXX

@subheading Arguments:

XXX
@subheading File:

XXX

@subheading Development Comments:

XXX

@c
@c
@c
@page
@section Handler Table Functions for Directories

Handler table functions are defined in a rtems_filesystem_file_handlers_r
structure. It defines functions that are specific to a node type in a given
file system. One table exists for each of the file system's node types. The
structure definition appears below. It is followed by general developmental
information on each of the functions associated with directories contained in
this function management structure.

@example
typedef struct @{
    rtems_filesystem_open_t           open;
    rtems_filesystem_close_t          close;
    rtems_filesystem_read_t           read;
    rtems_filesystem_write_t          write;
    rtems_filesystem_ioctl_t          ioctl;
    rtems_filesystem_lseek_t          lseek;
    rtems_filesystem_fstat_t          fstat;
    rtems_filesystem_fchmod_t         fchmod;
    rtems_filesystem_ftruncate_t      ftruncate;
    rtems_filesystem_fpathconf_t      fpathconf;
    rtems_filesystem_fsync_t          fsync;
    rtems_filesystem_fdatasync_t      fdatasync;
@} rtems_filesystem_file_handlers_r;
@end example


@c
@c
@c

@page

@section open() for Directories

@subheading Slot Function:

imfs_dir_open()

@subheading Arguments:

@example
rtems_libio_t  *iop,
const char     *pathname,
unsigned32      flag,
unsigned32      mode
@end example

@subheading File:

imfs_directory.c

@subheading Development Comments:

This routine will look into the file control block to find the @code{jnode} that
is associated with the directory.

The routine will verify that the node is a directory. If its not a directory
an error code will be returned.

If it is a directory, the offset in the file control block will be set to 0.
This allows us to start reading at the beginning of the directory.

@c
@c
@c

@page

@section close() for Directories

@subheading Slot Function:

imfs_dir_close()

@subheading Arguments:

@example
rtems_libio_t     *iop
@end example

@subheading File:

imfs_directory.c

@subheading Development Comments:

This routine is a dummy for directories under the base file system. It
immediately returns a success status.

@c
@c
@c

@page

@section read() for Directories

@subheading Slot Function:

imfs_dir_read

@subheading Arguments:

@example
rtems_libio_t  *iop,
void           *buffer,
unsigned32      count
@end example

@subheading File:

imfs_directory.c

@subheading Development Comments:

This routine will read a fixed number of directory entries from the current
directory offset. The number of directory bytes read will be returned from
this routine.


@c
@c
@c

@page

@section write() for Directories

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX

@c
@c
@c

@page

@section ioctl() for Directories

@subheading Slot Function:

ioctl

@subheading Arguments:


@subheading File:

Not supported

@subheading Development Comments:

XXX

@c
@c
@c

@page

@section lseek() for Directories

@subheading Slot Function:

imfs_dir_lseek()

@subheading Arguments:

@example
rtems_libio_t      *iop,
off_t               offset,
int                 whence
@end example

@subheading File:

imfs_directory.c

@subheading Development Comments:

This routine alters the offset in the file control block.

No test is performed on the number of children under the current open
directory.  The imfs_dir_read() function protects against reads beyond the
current size to the directory by returning a 0 bytes transfered to the
calling programs whenever the file position index exceeds the last entry in
the open directory.

@c
@c
@c

@page

@section fstat() for Directories

@subheading Slot Function:

imfs_dir_fstat()

@subheading Arguments:


@example
rtems_filesystem_location_info_t   *loc,
struct stat                        *buf
@end example

@subheading File:

imfs_directory.c

@subheading Development Comments:

The node access information in the rtems_filesystem_location_info_t structure
is used to locate the appropriate IMFS_jnode_t structure. The following
information is taken from the IMFS_jnode_t structure and placed in the stat
structure:

@itemize @bullet
@item st_ino
@item st_mode
@item st_nlink
@item st_uid
@item st_gid
@item st_atime
@item st_mtime
@item st_ctime
@end itemize

The st_size field is obtained by running through the chain of directory
entries and summing the sizes of the dirent structures associated with each
of the children of the directory.

@c
@c
@c

@page

@section fchmod() for Directories

@subheading Slot Function:

IMFS_fchmod()

@subheading Arguments:

@example
rtems_libio_t     *iop
mode_t             mode
@end example

@subheading File:

imfs_fchmod.c

@subheading Development Comments:

This routine will obtain the pointer to the IMFS_jnode_t structure from the
information currently in the file control block.

Based on configuration the routine will acquire the user ID from a call to
getuid()  or from the IMFS_jnode_t structure.

It then checks to see if we have the ownership rights to alter the mode of
the file.  If the caller does not, an error code is returned.

An additional test is performed to verify that the caller is not trying to
alter the nature of the node. If the caller is attempting to alter more than
the permissions associated with user group and other, an error is returned.

If all the preconditions are met, the user, group and other fields are set
based on the mode calling parameter.

@c
@c
@c

@page

@section ftruncate() for Directories

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX

@c
@c
@c

@page

@section fpathconf() for Directories

@subheading Slot Function:

fpathconf

@subheading Arguments:

Not Implemented

@subheading File:

Not Implemented

@subheading Development Comments:

Not Implemented


@c
@c
@c

@page

@section fsync() for Directories

@subheading Slot Function:

XXX

@subheading Arguments:

XXX
@subheading File:

XXX

@subheading Development Comments:

XXX


@c
@c
@c

@page

@section fdatasync() for Directories

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX


@section Handler Table Functions for Devices

Handler table functions are defined in a rtems_filesystem_file_handlers_r
structure. It defines functions that are specific to a node type in a given
file system. One table exists for each of the file system's node types. The
structure definition appears below. It is followed by general developmental
information on each of the functions associated with devices contained in
this function management structure.

@example
typedef struct @{
  rtems_filesystem_open_t           open;
  rtems_filesystem_close_t          close;
  rtems_filesystem_read_t           read;
  rtems_filesystem_write_t          write;
  rtems_filesystem_ioctl_t          ioctl;
  rtems_filesystem_lseek_t          lseek;
  rtems_filesystem_fstat_t          fstat;
  rtems_filesystem_fchmod_t         fchmod;
  rtems_filesystem_ftruncate_t      ftruncate;
  rtems_filesystem_fpathconf_t      fpathconf;
  rtems_filesystem_fsync_t          fsync;
  rtems_filesystem_fdatasync_t      fdatasync;
@} rtems_filesystem_file_handlers_r;
@end example

@c
@c
@c

@page

@section open() for Devices

@subheading Slot Function:

device_open()

@subheading Arguments:

@example
rtems_libio_t     *iop,
const char        *pathname,
unsigned32         flag,
unsigned32         mode
@end example

@subheading File:

deviceio.c

@subheading Development Comments:

This routine will use the file control block to locate the node structure for
the device.

It will extract the major and minor device numbers from the @code{jnode}.

The major and minor device numbers will be used to make a rtems_io_open()  
function call to open the device driver. An argument list is sent to the
driver that contains the file control block, flags and mode information.

@c
@c
@c

@page

@section close() for Devices

@subheading Slot Function:

device_close()

@subheading Arguments:

@example
rtems_libio_t     *iop
@end example

@subheading File:

deviceio.c

@subheading Development Comments:

This routine extracts the major and minor device driver numbers from the
IMFS_jnode_t that is referenced in the file control block.

It also forms an argument list that contains the file control block.

A rtems_io_close() function call is made to close the device specified by the
major and minor device numbers.


@c
@c
@c

@page

@section read() for Devices

@subheading Slot Function:

device_read()

@subheading Arguments:

@example
rtems_libio_t     *iop,
void              *buffer,
unsigned32         count
@end example

@subheading File:

deviceio.c

@subheading Development Comments:

This routine will extract the major and minor numbers for the device from the -
jnode- associated with the file descriptor.

A rtems_io_read() call will be made to the device driver associated with the file 
descriptor. The major and minor device number will be sent as arguments as well 
as an argument list consisting of:

@itemize @bullet
@item file control block

@item file position index

@item buffer pointer where the data read is to be placed

@item count indicating the number of bytes that the program wishes to read 
from the device

@item flags from the file control block

@end itemize

On return from the rtems_io_read() the number of bytes that were actually
read will be returned to the calling program.


@c
@c
@c

@page

@section write() for Devices

@subheading Slot Function:

XXX

@subheading Arguments:

XXX
@subheading File:

XXX

@subheading Development Comments:

XXX

@c
@c
@c

@page

@section ioctl() for Devices

@subheading Slot Function:

ioctl

@subheading Arguments:

@example
rtems_libio_t     *iop,
unsigned32         command,
void              *buffer
@end example

@subheading File:

deviceio.c

@subheading Development Comments:

This handler will obtain status information about a device.

The form of status is device dependent.

The rtems_io_control() function uses the major and minor number of the device
to obtain the status information.

rtems_io_control() requires an rtems_libio_ioctl_args_t argument list which
contains the file control block, device specific command and a buffer pointer
to return the device status information.

The device specific command should indicate the nature of the information
that is desired from the device.

After the rtems_io_control() is processed, the buffer should contain the
requested device information.

If the device information is not obtained properly a -1 will be returned to
the calling program, otherwise the ioctl_return value is returned.

@c
@c
@c

@page

@section lseek() for Devices

@subheading Slot Function:

device_lseek()

@subheading Arguments:

@example
rtems_libio_t     *iop,
off_t              offset,
int                whence
@end example

@subheading File:

deviceio.c

@subheading Development Comments:

At the present time this is a placeholder function. It always returns a
successful status.

@c
@c
@c

@page

@section fstat() for Devices

@subheading Slot Function:

IMFS_stat()

@subheading Arguments:

@example
rtems_filesystem_location_info_t   *loc,
struct stat                        *buf
@end example

@subheading File:

imfs_stat.c

@subheading Development Comments:

This routine actually performs status processing for both devices and regular files. 

The IMFS_jnode_t structure is referenced to determine the type of node under the 
file system. 

If the node is associated with a device, node information is extracted and 
transformed to set the st_dev element of the stat structure.

If the node is a regular file, the size of the regular file is extracted from the node.

This routine rejects other node types.

The following information is extracted from the node and placed in the stat 
structure:

@itemize @bullet

@item st_mode

@item st_nlink

@item st_ino

@item st_uid

@item st_gid

@item st_atime

@item st_mtime

@item st_ctime

@end itemize



@c
@c
@c

@page

@section fchmod() for Devices

@subheading Slot Function:

IMFS_fchmod()

@subheading Arguments:

@example
rtems_libio_t     *iop
mode_t             mode
@end example

@subheading File:

imfs_fchmod.c

@subheading Development Comments:

This routine will obtain the pointer to the IMFS_jnode_t structure from the
information currently in the file control block.

Based on configuration the routine will acquire the user ID from a call to
getuid()  or from the IMFS_jnode_t structure.

It then checks to see if we have the ownership rights to alter the mode of
the file.  If the caller does not, an error code is returned.

An additional test is performed to verify that the caller is not trying to
alter the nature of the node. If the caller is attempting to alter more than
the permissions associated with user group and other, an error is returned.

If all the preconditions are met, the user, group and other fields are set
based on the mode calling parameter.


@c
@c
@c

@page

@section ftruncate() for Devices

@subheading Slot Function:

XXX

@subheading Arguments:

XXX
@subheading File:

XXX

@subheading Development Comments:

XXX

@c
@c
@c

@page

@section fpathconf() for Devices

@subheading Slot Function:

fpathconf

@subheading Arguments:

Not Implemented

@subheading File:

Not Implemented

@subheading Development Comments:

Not Implemented


@c
@c
@c

@page

@section fsync() for Devices

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX


@c
@c
@c

@page

@subsection fdatasync() for Devices

Not Implemented

@subheading Slot Function:

XXX

@subheading Arguments:

XXX

@subheading File:

XXX

@subheading Development Comments:

XXX

