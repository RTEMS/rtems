@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.


@chapter Filesystem Implementation Requirements

This chapter details the behavioral requirements that all filesystem
implementations must adhere to.

@section General

The RTEMS filesystem framework was intended to be compliant with the
POSIX Files and Directories interface standard. The following filesystem
characteristics resulted in a functional switching layer.

@example
Figure of the Filesystem Functional Layering goes here.
This figure includes networking and disk caching layering.
@end example

@ifset use-ascii
@example
@group
@end group
@end example
@end ifset

@ifset use-tex
@c @image{FunctionalLayerCake,6in,4in}
@end ifset

@ifset use-html
@end ifset

@enumerate

@item Application programs are presented with a standard set of POSIX
compliant functions that allow them to interface with the files, devices
and directories in the filesystem. The interfaces to these routines do
not reflect the type of subordinate filesystem implementation in which
the file will be found.

@item The filesystem framework developed under RTEMS allows for mounting
filesystem of different types under the base filesystem.

@item The mechanics of locating file information may be quite different
between filesystem types.

@item The process of locating a file may require crossing filesystem
boundaries.

@item The transitions between filesystem and the processing required to
access information in different filesystem is not visible at the level
of the POSIX function call.

@item The POSIX interface standard provides file access by character
pathname to the file in some functions and through an integer file
descriptor in other functions.

@item The nature of the integer file descriptor and its associated
processing is operating system and filesystem specific.

@item Directory and device information must be processed with some of the
same routines that apply to files.

@item The form and content of directory and device information differs
greatly from that of a regular file.

@item Files, directories and devices represent elements (nodes) of a tree
hierarchy.

@item The rules for processing each of the node types that exist under the
filesystem are node specific but are still not reflected in the POSIX
interface routines.

@end enumerate


@example
Figure of the Filesystem Functional Layering goes here.
This figure focuses on the Base Filesystem and IMFS.
@end example

@example
Figure of the IMFS Memfile control blocks 
@end example

@section File and Directory Removal Constraints

The following POSIX constraints must be honored by all filesystems.

@itemize @bullet

@item If a node is a directory with children it cannot be removed.

@item The root node of any filesystem, whether the base filesystem or a 
mounted filesystem, cannot be removed.

@item A node that is a directory that is acting as the mount point of a file
system cannot be removed.

@item On filesystems supporting hard links, a link count is maintained.
Prior to node removal, the node's link count is decremented by one.  The
link count must be less than one to allow for removal of the node.

@end itemize

@c
@c
@c
@section API Layering

@subsection Mapping of Generic System Calls to Filesystem Specific Functions

The list of generic system calls includes the routines open(), read(),
write(), close(), etc..

The Files and Directories section of the POSIX Application Programs
Interface specifies a set of functions with calling arguments that are
used to gain access to the information in a filesystem. To the
application program, these functions allow access to information in any
mounted filesystem without explicit knowledge of the filesystem type or
the filesystem mount configuration. The following are functions that are
provided to the application:

@enumerate
@item access()
@item chdir()
@item chmod()
@item chown()
@item close()
@item closedir()
@item fchmod()
@item fcntl()
@item fdatasync()
@item fpathconf()
@item fstat()
@item fsync()
@item ftruncate()
@item link()
@item lseek()
@item mkdir()
@item mknod()
@item mount()
@item open()
@item opendir()
@item pathconf()
@item read()
@item readdir()
@item rewinddir()
@item rmdir()
@item rmnod()
@item scandir()
@item seekdir()
@item stat()
@item telldir()
@item umask()
@item unlink()
@item unmount()
@item utime()
@item write()
@end enumerate

The filesystem's type as well as the node type within the filesystem
determine the nature of the processing that must be performed for each of
the functions above. The RTEMS filesystem provides a framework that
allows new filesystem to be developed and integrated without alteration
to the basic framework.

To provide the functional switching that is required, each of the POSIX
file and directory functions have been implemented as a shell function.
The shell function adheres to the POSIX interface standard. Within this
functional shell, filesystem and node type information is accessed which
is then used to invoke the appropriate filesystem and node type specific
routine to process the POSIX function call.

@subsection File/Device/Directory function access via file control block - rtems_libio_t structure

The POSIX open() function returns an integer file descriptor that is used
as a reference to file control block information for a specific file. The
file control block contains information that is used to locate node, file
system, mount table and functional handler information. The diagram in
Figure 8 depicts the relationship between and among the following
components.

@enumerate

@item File Descriptor Table

This is an internal RTEMS structure that tracks all currently defined file
descriptors in the system. The index that is returned by the file open()
operation references a slot in this table. The slot contains a pointer to
the file descriptor table entry for this file. The rtems_libio_t structure
represents the file control block.

@item Allocation of entry in the File Descriptor Table

Access to the file descriptor table is controlled through a semaphore that
is implemented using the rtems_libio_allocate() function. This routine
will grab a semaphore and then scan the file control blocks to determine
which slot is free for use. The first free slot is marked as used and the
index to this slot is returned as the file descriptor for the open()
request. After the alterations have been made to the file control block
table, the semaphore is released to allow further operations on the table.

@item Maximum number of entries in the file descriptor table is
configurable through the src/exec/sapi/headers/confdefs.h file. If the
CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS constant is defined its value
will represent the maximum number of file descriptors that are allowed.  
If CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS is not specified a default
value of 20 will be used as the maximum number of file descriptors
allowed.

@item File control block - rtems_libio_t structure

@example
struct rtems_libio_tt @{
  rtems_driver_name_t              *driver;
  off_t                             size;
  off_t                             offset;
  unsigned32                        flags;
  rtems_filesystem_location_info_t  pathinfo;
  Objects_Id                        sem;
  unsigned32                        data0;
  void                              data1;
  void                              file_info;
  rtems_filesystem_file_handlers_r  handlers;
@};
@end example

A file control block can exist for regular files, devices and directories.
The following fields are important for regular file and directory access:

@itemize @bullet

@item Size - For a file this represents the number of bytes currently
stored in a file. For a directory this field is not filled in.

@item Offset - For a file this is the byte file position index relative to
the start of the file. For a directory this is the byte offset into a
sequence of dirent structures.

@item Pathinfo - This is a structure that provides a pointer to node
information, OPS table functions, Handler functions and the mount table
entry associated with this node.

@item file_info - A pointer to node information that is used by Handler
functions

@item handlers - A pointer to a table of handler functions that operate on
a file, device or directory through a file descriptor index

@end itemize

@end enumerate

@subsection File/Directory function access via rtems_filesystem_location_info_t structure

The rtems_filesystem_location_info_tt structure below provides sufficient
information to process nodes under a mounted filesystem.

@example
struct rtems_filesystem_location_info_tt @{
    void                                     *node_access;
    rtems_filesystem_file_handlers_r         *handlers;
    rtems_filesystem_operations_table        *ops;
    rtems_filesystem_mount_table_entry_t     *mt_entry;
@};
@end example

It contains a void pointer to filesystem specific nodal structure,
pointers to the OPS table for the filesystem that contains the node, the
node type specific handlers for the node and a reference pointer to the
mount table entry associated with the filesystem containing the node

@section Operation Tables

Filesystem specific operations are invoked indirectly.  The set of 
routines that implement the filesystem are configured into two tables.
The Filesystem Handler Table has routines that are specific to a 
filesystem but remain constant regardless of the actual file type.
The File Handler Table has routines that are both filesystem and file type
specific.

@subsection Filesystem Handler Table Functions

OPS table functions are defined in a @code{rtems_filesystem_operations_table}
structure.  It defines functions that are specific to a given filesystem.
One table exists for each filesystem that is supported in the RTEMS
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
@c @page

@subsubsection evalpath Handler

@subheading Corresponding Structure Element:

evalpath

@subheading Arguments:

@example
  const char                        *pathname,      /* IN     */
  int                                flags,         /* IN     */
  rtems_filesystem_location_info_t  *pathloc        /* IN/OUT */
@end example

@subheading Description:

This routine is responsible for evaluating the pathname passed in
based upon the flags and the valid @code{rthems_filesystem_location_info_t}.
Additionally, it must make any changes to pathloc necessary to identify 
the pathname node.  This should include calling the evalpath for a mounted 
filesystem, if the given filesystem supports the mount command.  

This routine returns a 0 if the evaluation was successful. 
Otherwise, it returns a -1 and sets errno to the correct error.

This routine is required and should NOT be set to NULL.

@c
@c
@c
@c @page

@subsubsection evalformake Handler

@subheading Corresponding Structure Element:

evalformake

@subheading Arguments:

@example
   const char                       *path,       /* IN */
   rtems_filesystem_location_info_t *pathloc,    /* IN/OUT */
   const char                      **name        /* OUT    */
@end example

@subheading Description:

This method is given a path to evaluate and a valid start location.  It
is responsible for finding the parent node for a requested make command,
setting pathloc information to identify the parent node, and setting
the name pointer to the first character of the name of the new node.
Additionally, if the filesystem supports the mount command, this method 
should call the evalformake routine for the mounted filesystem. 

This routine returns a 0 if the evaluation was successful.  Otherwise, it 
returns a -1 and sets errno to the correct error.

This routine is required and should NOT be set to NULL.  However, if
the filesystem does not support user creation of a new node, it may
set errno to ENOSYS and return -1.

@c
@c
@c
@c @page

@subsubsection link Handler

@subheading Corresponding Structure Element:

link

@subheading Arguments:


@example
rtems_filesystem_location_info_t    *to_loc,      /* IN */
rtems_filesystem_location_info_t    *parent_loc,  /* IN */
const char                          *token        /* IN */
@end example

@subheading Description:


This routine is used to create a hard-link.

It will first examine the st_nlink count of the node that we are trying to.
If the link count exceeds LINK_MAX an error will be returned.

The name of the link will be normalized to remove extraneous separators from
the end of the name.

This routine is not required and may be set to NULL.

@c
@c
@c
@c @page

@subsubsection unlink Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX

@subheading Description:

XXX


@c
@c
@c
@c @page

@subsubsection node_type Handler

@subheading Corresponding Structure Element:

node_type()

@subheading Arguments:

@example
rtems_filesystem_location_info_t    *pathloc        /* IN */
@end example

@subheading Description:

XXX

@c
@c
@c
@c @page

@subsubsection mknod Handler

@subheading Corresponding Structure Element:

mknod()

@subheading Arguments:

@example
const char                          *token,        /* IN */
mode_t                               mode,         /* IN */
dev_t                                dev,          /* IN */
rtems_filesystem_location_info_t    *pathloc       /* IN/OUT */
@end example

@subheading Description:

XXX

@c
@c
@c
@c @page

@subsubsection rmnod Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX

@subheading Description:

XXX


@c
@c
@c
@c @page

@subsubsection chown Handler

@subheading Corresponding Structure Element:

chown()

@subheading Arguments:

@example
rtems_filesystem_location_info_t    *pathloc        /* IN */
uid_t                                owner          /* IN */
gid_t                                group          /* IN */
@end example

@subheading Description:

XXX

@c
@c
@c
@c @page

@subsubsection freenod Handler

@subheading Corresponding Structure Element:

freenod()

@subheading Arguments:

@example
rtems_filesystem_location_info_t      *pathloc       /* IN */
@end example

@subheading Description:

This routine is used by the generic code to allow memory to be allocated
during the evaluate routines, and set free when the generic code is finished
accessing a node.  If the evaluate routines allocate memory to identify
a node this routine should be utilized to free that memory.

This routine is not required and may be set to NULL.

@c
@c
@c
@c @page

@subsubsection mount Handler

@subheading Corresponding Structure Element:

mount()

@subheading Arguments:

@example
rtems_filesystem_mount_table_entry_t   *mt_entry
@end example

@subheading Description:

XXX

@c
@c
@c
@c @page

@subsubsection fsmount_me Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

@example
rtems_filesystem_mount_table_entry_t   *mt_entry    
@end example

@subheading Description:

This function is provided with a filesystem to take care of the internal
filesystem management details associated with mounting that filesystem
under the RTEMS environment.

It is not responsible for the mounting details associated the filesystem
containing the mount point.

The rtems_filesystem_mount_table_entry_t structure contains the key elements
below:

rtems_filesystem_location_info_t         *mt_point_node,

This structure contains information about the mount point. This 
allows us to find the ops-table and the handling functions 
associated with the filesystem containing the mount point.

rtems_filesystem_location_info_t         *fs_root_node,

This structure contains information about the root node in the file 
system to be mounted. It allows us to find the ops-table and the 
handling functions associated with the filesystem to be mounted.

rtems_filesystem_options_t                 options,

Read only or read/write access

void                                         *fs_info,

This points to an allocated block of memory the will be used to 
hold any filesystem specific information of a global nature. This 
allocated region if important because it allows us to mount the 
same filesystem type more than once under the RTEMS system. 
Each instance of the mounted filesystem has its own set of global 
management information that is separate from the global 
management information associated with the other instances of the 
mounted filesystem type.

rtems_filesystem_limits_and_options_t    pathconf_info,

The table contains the following set of values associated with the 
mounted filesystem:

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
the filesystem information. The filesystems that are currently implemented
are memory based and don't require a device specification.

If the mt_point_node.node_access is NULL then we are mounting the base file 
system.

The routine will create a directory node for the root of the IMFS file
system.

The node will have read, write and execute permissions for owner, group and
others.

The node's name will be a null string.

A filesystem information structure(fs_info) will be allocated and
initialized for the IMFS filesystem. The fs_info pointer in the mount table
entry will be set to point the filesystem information structure.

The pathconf_info element of the mount table will be set to the appropriate
table of path configuration constants (LIMITS_AND_OPTIONS).

The fs_root_node structure will be filled in with the following:

@itemize @bullet

@item pointer to the allocated root node of the filesystem

@item directory handlers for a directory node under the IMFS filesystem

@item OPS table functions for the IMFS

@end itemize

A 0 will be returned to the calling routine if the process succeeded,
otherwise a 1 will be returned.


@c
@c
@c
@c @page

@subsubsection unmount Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX

@subheading Description:

XXX 


@c
@c
@c
@c @page

@subsubsection fsunmount_me Handler

@subheading Corresponding Structure Element:

imfs_fsunmount_me()

@subheading Arguments:

@example
rtems_filesystem_mount_table_entry_t   *mt_entry    
@end example

@subheading Description:

XXX 


@c
@c
@c
@c @page

@subsubsection utime Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX

@subheading Description:

XXX 

@c
@c
@c
@c @page

@subsubsection eval_link Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX

@subheading Description:

XXX

@c
@c
@c
@c @page

@subsubsection symlink Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX

@subheading Description:

XXX

@c
@c
@c
@c @page
@subsection File Handler Table Functions

Handler table functions are defined in a @code{rtems_filesystem_file_handlers_r}
structure. It defines functions that are specific to a node type in a given
filesystem. One table exists for each of the filesystem's node types. The
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
  rtems_filesystem_fcntl_t          fcntl;
@} rtems_filesystem_file_handlers_r;
@end example

@c
@c
@c
@c @page

@subsubsection open Handler

@subheading Corresponding Structure Element:

open()

@subheading Arguments:

@example
rtems_libio_t   *iop,
const char      *pathname,
unsigned32       flag,
unsigned32       mode
@end example

@subheading Description:

XXX

@c
@c
@c
@c @page

@subsubsection close Handler

@subheading Corresponding Structure Element:

close()

@subheading Arguments:

@example
rtems_libio_t     *iop
@end example

@subheading Description:

XXX

@subheading NOTES:

XXX



@c
@c
@c
@c @page

@subsubsection read Handler

@subheading Corresponding Structure Element:

read()

@subheading Arguments:

@example
rtems_libio_t     *iop,
void              *buffer,
unsigned32         count
@end example

@subheading Description:

XXX

@subheading NOTES:

XXX


@c
@c
@c
@c @page

@subsubsection write Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX
@subheading Description:

XXX

@subheading NOTES:

XXX


@c
@c
@c
@c @page

@subsubsection ioctl Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

@example
rtems_libio_t     *iop,
unsigned32       command,
void              *buffer
@end example

@subheading Description:

XXX

@subheading NOTES:

XXX


@c
@c
@c
@c @page

@subsubsection lseek Handler

@subheading Corresponding Structure Element:

lseek()

@subheading Arguments:

@example
rtems_libio_t     *iop,
off_t              offset,
int                whence
@end example

@subheading Description:

XXX

@subheading NOTES:

XXX


@c
@c
@c
@c @page

@subsubsection fstat Handler

@subheading Corresponding Structure Element:

fstat()

@subheading Arguments:

@example
rtems_filesystem_location_info_t   *loc,
struct stat                        *buf
@end example

@subheading Description:

The following information is extracted from the filesystem
specific node and placed in the @code{stat} structure:

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


@subheading NOTES:

Both the @code{stat()} and @code{lstat()} services are 
implemented directly using the @code{fstat()} handler.  The
difference in behavior is determined by how the path is evaluated
prior to this handler being called on a particular
file entity.

The @code{fstat()} system call is implemented directly
on top of this filesystem handler.

@c
@c
@c
@c @page

@subsubsection fchmod Handler

@subheading Corresponding Structure Element:

fchmod()

@subheading Arguments:

@example
rtems_libio_t     *iop
mode_t              mode
@end example

@subheading Description:

XXX


@subheading NOTES:

XXX

@c
@c
@c
@c @page

@subsubsection ftruncate Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX
@subheading Description:

XXX

@subheading NOTES:

XXX

@subsubsection fpathconf Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX

@subheading Description:

XXX

@subheading NOTES:

XXX

@c
@c
@c
@c @page

@subsubsection fsync Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX
@subheading Description:

XXX

@subheading NOTES:

XXX

@c
@c
@c
@c @page

@subsubsection fdatasync Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX

@subheading Description:

XXX

@subheading NOTES:

XXX

@c
@c
@c
@c @page

@subsubsection fcntl Handler

@subheading Corresponding Structure Element:

XXX

@subheading Arguments:

XXX

@subheading Description:

XXX

@subheading NOTES:

XXX
