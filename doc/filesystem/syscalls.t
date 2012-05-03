@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter System Call Development Notes

This set of routines represents the application's interface to files and directories 
under the RTEMS filesystem. All routines are compliant with POSIX standards if a 
specific interface has been established. The list below represents the routines that have 
been included as part of the application's interface.

@enumerate
@item access()
@item chdir()
@item chmod()
@item chown()
@item close()
@item closedir()
@item dup()
@item dup2()
@item fchmod()
@item fcntl()
@item fdatasync()
@item fpathconf()
@item fstat()
@item ioctl()
@item link()
@item lseek()
@item mkdir()
@item mkfifo()
@item mknod()
@item mount()
@item open()
@item opendir()
@item pathconf()
@item read()
@item readdir()
@item unmount()
@end enumerate

The sections that follow provide developmental information concerning each
of these functions.

 
@c @page
@section access

@subheading File:

access.c

@subheading Processing: 

This routine is layered on the stat() function. It acquires the current
status information for the specified file and then determines if the
caller has the ability to access the file for read, write or execute
according to the mode argument to this function.

@subheading Development Comments:

This routine is layered on top of the stat() function. As long as the
st_mode element in the returned structure follow the standard UNIX
conventions, this function should support other filesystems without
alteration.

@c @page
@section chdir

@subheading File:

chdir.c


@subheading Processing: 

This routine will determine if the pathname that we are attempting to make
that current directory exists and is in fact a directory. If these
conditions are met the global indication of the current directory
(rtems_filesystem_current) is set to the rtems_filesystem_location_info_t
structure that is returned by the rtems_filesystem_evaluate_path()
routine.

@subheading Development Comments:

This routine is layered on the rtems_filesystem_evaluate_path() routine
and the filesystem specific OP table function node_type().

The routine node_type() must be a routine provided for each filesystem
since it must access the filesystems node information to determine which
of the following types the node is:

@itemize @bullet
@item RTEMS_FILESYSTEM_DIRECTORY
@item RTEMS_FILESYSTEM_DEVICE
@item RTEMS_FILESYSTEM_HARD_LINK
@item RTEMS_FILESYSTEM_MEMORY_FILE
@end itemize

This acknowledges that the form of the node management information can
vary from one filesystem implementation to another.

RTEMS has a special global structure that maintains the current directory
location. This global variable is of type rtems_filesystem_location_info_t
and is called rtems_filesystem_current. This structure is not always
valid. In order to determine if the structure is valid, you must first
test the node_access element of this structure. If the pointer is NULL,
then the structure does not contain a valid indication of what the current
directory is.

@c @page
@section chmod

@subheading File:

chmod.c

@subheading Processing: 

This routine is layered on the open(), fchmod() and close() functions. As
long as the standard interpretation of the mode_t value is maintained,
this routine should not need modification to support other filesystems.

@subheading Development Comments:

The routine first determines if the selected file can be open with
read/write access.  This is required to allow modification of the mode
associated with the selected path.

The fchmod() function is used to actually change the mode of the path
using the integer file descriptor returned by the open() function.

After mode modification, the open file descriptor is closed.

@c @page
@section chown

@subheading File:

chown.c

@subheading Processing: 

This routine is layered on the rtems_filesystem_evaluate_path() and the
file system specific chown() routine that is specified in the OPS table
for the file system.

@subheading Development Comments:

rtems_filesystem_evaluate_path() is used to determine if the path
specified actually exists. If it does a rtems_filesystem_location_info_t
structure will be obtained that allows the shell function to locate the
OPS table that is to be used for this filesystem.

It is possible that the chown() function that should be in the OPS table
is not defined. A test for a non-NULL OPS table chown() entry is performed
before the function is called.

If the chown() function is defined in the indicated OPS table, the
function is called with the rtems_filesystem_location_info_t structure
returned from the path evaluation routine, the desired owner, and group
information.

@c @page
@section close

@subheading File:

close.c

@subheading Processing: 

This routine will allow for the closing of both network connections and
file system devices. If the file descriptor is associated with a network
device, the appropriate network function handler will be selected from a
table of previously registered network functions (rtems_libio_handlers)
and that function will be invoked.

If the file descriptor refers to an entry in the filesystem, the
appropriate handler will be selected using information that has been
placed in the file control block for the device (rtems_libio_t structure).

@subheading Development Comments:

rtems_file_descriptor_type examines some of the upper bits of the file
descriptor index. If it finds that the upper bits are set in the file
descriptor index, the device referenced is a network device.

Network device handlers are obtained from a special registration table
(rtems_libio_handlers) that is set up during network initialization. The
network handler invoked and the status of the network handler will be
returned to the calling process.

If none of the upper bits are set in the file descriptor index, the file
descriptor refers to an element of the RTEMS filesystem.

The following sequence will be performed for any filesystem file
descriptor:

@enumerate

@item Use the rtems_libio_iop() function to obtain the rtems_libio_t
structure for the file descriptor

@item Range check the file descriptor using rtems_libio_check_fd()

@item Determine if there is actually a function in the selected handler
table that processes the close() operation for the filesystem and node
type selected.  This is generally done to avoid execution attempts on
functions that have not been implemented.

@item If the function has been defined it is invoked with the file control
block pointer as its argument.

@item The file control block that was associated with the open file
descriptor is marked as free using rtems_libio_free().

@item The return code from the close handler is then passed back to the
calling program.

@end enumerate

@c @page
@section closedir

@subheading File:

closedir.c

@subheading Processing: 

The code was obtained from the BSD group. This routine must clean up the
memory resources that are required to track an open directory. The code is
layered on the close() function and standard memory free() functions. It
should not require alterations to support other filesystems.

@subheading Development Comments:

The routine alters the file descriptor and the index into the DIR
structure to make it an invalid file descriptor. Apparently the memory
that is about to be freed may still be referenced before it is
reallocated.

The dd_buf structure's memory is reallocated before the control structure
that contains the pointer to the dd_buf region.

DIR control memory is reallocated.

The close() function is used to free the file descriptor index.


@c @page
@section dup()      Unimplemented

@subheading File:

dup.c

@subheading Processing: 


@subheading Development Comments:





@c @page
@section dup2()      Unimplemented

@subheading File:

dup2.c

@subheading Processing: 


@subheading Development Comments:






@c @page
@section fchmod

@subheading File:

fchmod.c

@subheading Processing: 

This routine will alter the permissions of a node in a filesystem. It is
layered on the following functions and macros:

@itemize @bullet
@item rtems_file_descriptor_type()

@item rtems_libio_iop()

@item rtems_libio_check_fd()

@item rtems_libio_check_permissions()

@item fchmod() function that is referenced by the handler table in the
file control block associated with this file descriptor

@end itemize

@subheading Development Comments:

The routine will test to see if the file descriptor index is associated
with a network connection. If it is, an error is returned from this
routine.

The file descriptor index is used to obtain the associated file control
block.

The file descriptor value is range checked.

The file control block is examined to determine if it has write
permissions to allow us to alter the mode of the file.

A test is made to determine if the handler table that is referenced in the
file control block contains an entry for the fchmod() handler function. If
it does not, an error is returned to the calling routine.

If the fchmod() handler function exists, it is called with the file
control block and the desired mode as parameters.

@c @page
@section fcntl()

@subheading File:

fcntl.c

@subheading Processing: 

This routine currently only interacts with the file control block. If the
structure of the file control block and the associated meanings do not
change, the partial implementation of fcntl() should remain unaltered for
other filesystem implementations.

@subheading Development Comments:

The only commands that have been implemented are the F_GETFD and F_SETFD.
The commands manipulate the LIBIO_FLAGS_CLOSE_ON_EXEC bit in the
@code{flags} element of the file control block associated with the file
descriptor index.

The current implementation of the function performs the sequence of
operations below:

@enumerate

@item Test to see if we are trying to operate on a file descriptor
associated with a network connection

@item Obtain the file control block that is associated with the file
descriptor index

@item Perform a range check on the file descriptor index.

@end enumerate



@c @page
@section fdatasync

@subheading File:

fdatasync.c

@subheading Processing: 

This routine is a template in the in memory filesystem that will route us to the 
appropriate handler function to carry out the fdatasync() processing. In the in 
memory filesystem this function is not necessary. Its function in a disk based file 
system that employs a memory cache is to flush all memory based data buffers to 
disk. It is layered on the following functions and macros:

@itemize @bullet

@item rtems_file_descriptor_type()

@item rtems_libio_iop()

@item rtems_libio_check_fd()

@item rtems_libio_check_permissions()

@item fdatasync() function that is referenced by the handler table in the
file control block associated with this file descriptor

@end itemize

@subheading Development Comments:

The routine will test to see if the file descriptor index is associated
with a network connection. If it is, an error is returned from this
routine.

The file descriptor index is used to obtain the associated file control
block.

The file descriptor value is range checked.

The file control block is examined to determine if it has write
permissions to the file.

A test is made to determine if the handler table that is referenced in the
file control block contains an entry for the fdatasync() handler function.
If it does not an error is returned to the calling routine.

If the fdatasync() handler function exists, it is called with the file
control block as its parameter.

@c @page
@section fpathconf

@subheading File:

fpathconf.c

@subheading Processing: 

This routine is layered on the following functions and macros:

@itemize @bullet

@item rtems_file_descriptor_type()

@item rtems_libio_iop()

@item rtems_libio_check_fd()

@item rtems_libio_check_permissions()

@end itemize

When a filesystem is mounted, a set of constants is specified for the
filesystem.  These constants are stored with the mount table entry for the
filesystem. These constants appear in the POSIX standard and are listed
below.

@itemize @bullet

@item PCLINKMAX

@item PCMAXCANON

@item PCMAXINPUT

@item PCNAMEMAX

@item PCPATHMAX

@item PCPIPEBUF

@item PCCHOWNRESTRICTED

@item PCNOTRUNC

@item PCVDISABLE

@item PCASYNCIO

@item PCPRIOIO

@item PCSYNCIO


@end itemize

This routine will find the mount table information associated the file
control block for the specified file descriptor parameter. The mount table
entry structure contains a set of filesystem specific constants that can
be accessed by individual identifiers.

@subheading Development Comments:

The routine will test to see if the file descriptor index is associated
with a network connection. If it is, an error is returned from this
routine.

The file descriptor index is used to obtain the associated file control
block.

The file descriptor value is range checked.

The file control block is examined to determine if it has read permissions
to the file.

Pathinfo in the file control block is used to locate the mount table entry
for the filesystem associated with the file descriptor.

The mount table entry contains the pathconf_limits_and_options element.
This element is a table of constants that is associated with the
filesystem.

The name argument is used to reference the desired constant from the
pathconf_limits_and_options table.


@c @page
@section fstat

@subheading File:

fstat.c

@subheading Processing: 

This routine will return information concerning a file or network
connection. If the file descriptor is associated with a network
connection, the current implementation of @code{fstat()} will return a
mode set to @code{S_IFSOCK}. In a later version, this routine will map the
status of a network connection to an external handler routine.

If the file descriptor is associated with a node under a filesystem, the
fstat()  routine will map to the fstat() function taken from the node
handler table.

@subheading Development Comments:

This routine validates that the struct stat pointer is not NULL so that
the return location is valid.

The struct stat is then initialized to all zeros.

rtems_file_descriptor_type() is then used to determine if the file
descriptor is associated with a network connection. If it is, network
status processing is performed. In the current implementation, the file
descriptor type processing needs to be improved. It currently just drops
into the normal processing for file system nodes.

If the file descriptor is associated with a node under a filesystem, the
following steps are performed:

@enumerate

@item Obtain the file control block that is associated with the file descriptor 
index.

@item Range check the file descriptor index.

@item Test to see if there is a non-NULL function pointer in the handler
table for the fstat() function. If there is, invoke the function with the
file control block and the pointer to the stat structure.

@end enumerate

@c @page
@section ioctl

@subheading File:

ioctl.c

@subheading Processing: 

Not defined in the POSIX 1003.1b standard but commonly supported in most
UNIX and POSIX system. Ioctl() is a catchall for I/O operations. Routine
is layered on external network handlers and filesystem specific handlers.
The development of new filesystems should not alter the basic processing
performed by this routine.

@subheading Development Comments:


The file descriptor is examined to determine if it is associated with a
network device. If it is processing is mapped to an external network
handler. The value returned by this handler is then returned to the
calling program.

File descriptors that are associated with a filesystem undergo the
following processing:

@enumerate

@item The file descriptor index is used to obtain the associated file
control block.

@item The file descriptor value is range checked.

@item A test is made to determine if the handler table that is referenced
in the file control block contains an entry for the ioctl() handler
function. If it does not, an error is returned to the calling routine.

@item If the ioctl() handler function exists, it is called with the file
control block, the command and buffer as its parameters.

@item The return code from this function is then sent to the calling
routine.

@end enumerate


@c @page
@section link

@subheading File:

link.c

@subheading Processing: 

This routine will establish a hard link to a file, directory or a device.
The target of the hard link must be in the same filesystem as the new link
being created. A link to an existing link is also permitted but the
existing link is evaluated before the new link is made. This implies that
links to links are reduced to links to files, directories or devices
before they are made.

@subheading Development Comments:

Calling parameters:
const char   *existing
    const char   *new

link() will determine if the target of the link actually exists using
rtems_filesystem_evaluate_path()

rtems_filesystem_get_start_loc() is used to determine where to start the
path evaluation of the new name. This macro examines the first characters
of the name to see if the name of the new link starts with a
rtems_filesystem_is_separator. If it does the search starts from the root
of the RTEMS filesystem; otherwise the search will start from the current
directory.

The OPS table evalformake() function for the parent's filesystem is used
to locate the node that will be the parent of the new link. It will also
locate the start of the new path's name. This name will be used to define
a child under the parent directory.

If the parent is found, the routine will determine if the hard link that
we are trying to create will cross a filesystem boundary. This is not
permitted for hard-links.

If the hard-link does not cross a filesystem boundary, a check is
performed to determine if the OPS table contains an entry for the link()
function.

If a link() function is defined, the OPS table link() function will be
called to establish the actual link within the filesystem.

The return code from the OPS table link() function is returned to the
calling program.

@c @page
@section lseek

@subheading File:

lseek.c

@subheading Processing: 

This routine is layered on both external handlers and filesystem / node
type specific handlers. This routine should allow for the support of new
filesystems without modification.

@subheading Development Comments:

This routine will determine if the file descriptor is associated with a
network device. If it is lseek will map to an external network handler.
The handler will be called with the file descriptor, offset and whence as
its calling parameters. The return code from the external handler will be
returned to the calling routine.

If the file descriptor is not associated with a network connection, it is
associated with a node in a filesystem. The following steps will be
performed for filesystem nodes:

@enumerate

@item The file descriptor is used to obtain the file control block for the
node.

@item The file descriptor is range checked.

@item The offset element of the file control block is altered as indicated
by the offset and whence calling parameters

@item The handler table in the file control block is examined to determine
if it contains an entry for the lseek() function. If it does not an error
is returned to the calling program.

@item The lseek() function from the designated handler table is called
with the file control block, offset and whence as calling arguments

@item The return code from the lseek() handler function is returned to the
calling program

@end enumerate


@c @page
@section mkdir

@subheading File:

mkdir.c

@subheading Processing: 

This routine attempts to create a directory node under the filesystem. The
routine is layered the mknod() function.

@subheading Development Comments:

See mknod() for developmental comments.

@c @page
@section mkfifo

@subheading File:

mkfifo.c

@subheading Processing: 

This routine attempts to create a FIFO node under the filesystem. The
routine is layered the mknod() function.

@subheading Development Comments:

See mknod() for developmental comments

@c @page
@section mknod

@subheading File:

mknod.c

@subheading Processing: 

This function will allow for the creation of the following types of nodes
under the filesystem:

@itemize @bullet

@item directories

@item regular files

@item character devices

@item block devices

@item fifos

@end itemize

At the present time, an attempt to create a FIFO will result in an ENOTSUP
error to the calling function. This routine is layered the filesystem
specific routines evalformake and mknod. The introduction of a new
filesystem must include its own evalformake and mknod function to support
the generic mknod() function.  Under this condition the generic mknod()
function should accommodate other filesystem types without alteration.

@subheading Development Comments:

Test for nodal types - I thought that this test should look like the
following code:

@example
if ( (mode & S_IFDIR) = = S_IFDIR) ||
     (mode & S_IFREG) = = S_IFREG) ||
     (mode & S_IFCHR) = = S_IFCHR) ||
     (mode & S_IFBLK) = = S_IFBLK) ||
     (mode & S_IFIFO) = = S_IFIFO))
     Set_errno_and_return_minus_one (EINVAL);

@end example

Where:

@itemize @bullet
@item S_IFREG (0100000) - Creation of a regular file
@item S_IFCHR (0020000) - Creation of a character device
@item S_IFBLK (0060000) - Creation of a block device
@item S_IFIFO (0010000) - Creation of a FIFO
@end itemize

Determine if the pathname that we are trying to create starts at the root
directory or is relative to the current directory using the
rtems_filesystem_get_start_loc()  function.

Determine if the pathname leads to a valid directory that can be accessed
for the creation of a node.

If the pathname is a valid location to create a node, verify that a
filesystem specific mknod() function exists.

If the mknod() function exists, call the filesystem specific mknod()
function.  Pass the name, mode, device type and the location information
associated with the directory under which the node will be created.

@c @page
@section mount

@subheading File:

mount.c


Arguments (Not a standard POSIX call): 

rtems_filesystem_mount_table_entry_t   **mt_entry,

If the mount operation is successful, this pointer to a pointer will be
set to reference the mount table chain entry that has been allocated for
this file system mount.

rtems_filesystem_operations_table   *fs_ops,

This is a pointer to a table of functions that are associated with the
file system that we are about to mount. This is the mechanism to selected
file system type without keeping a dynamic database of all possible file
system types that are valid for the mount operation. Using this method, it
is only necessary to configure the filesystems that we wish to use into
the RTEMS build. Unused filesystems types will not be drawn into the
build.

char                      *fsoptions,

This argument points to a string that selects mounting for read only
access or read/write access. Valid states are "RO" and "RW"

char                      *device,

This argument is reserved for the name of a device that will be used to
access the filesystem information. Current filesystem implementations are
memory based and do not require a device to access filesystem information.

char                      *mount_point

This is a pathname to a directory in a currently mounted filesystem that
allows read, write and execute permissions.  If successful, the node found
by evaluating this name, is stored in the mt_entry.

@subheading Processing: 

This routine will handle the mounting of a filesystem on a mount point. If
the operation is successful, a pointer to the mount table chain entry
associated with the mounted filesystem will be returned to the calling
function. The specifics about the processing required at the mount point
and within the filesystem being mounted is isolated in the filesystem
specific mount() and fsmount_me()  functions. This allows the generic
mount() function to remain unaltered even if new filesystem types are
introduced.



@subheading Development Comments:

This routine will use get_file_system_options() to determine if the mount
options are valid ("RO" or "RW").

It confirms that a filesystem ops-table has been selected.

Space is allocated for a mount table entry and selective elements of the
temporary mount table entry are initialized.

If a mount point is specified: The mount point is examined to determine
that it is a directory and also has the appropriate permissions to allow a
filesystem to be mounted.

The current mount table chain is searched to determine that there is not
another filesystem mounted at the mount point we are trying to mount onto.

If a mount function is defined in the ops table for the filesystem
containing the mount point, it is called at this time.

If no mount point is specified: Processing if performed to set up the
mount table chain entry as the base filesystem.

If the fsmount_me() function is specified for ops-table of the filesystem
being mounted, that function is called to initialize for the new
filesystem.

On successful completion, the temporary mount table entry will be placed
on the mount table chain to record the presence of the mounted filesystem.

@c @page
@section open

@subheading File:

open.c

@subheading Processing: 

This routine is layered on both RTEMS calls and filesystem specific
implementations of the open() function. These functional interfaces should
not change for new filesystems and therefore this code should be stable as
new file systems are introduced.

@subheading Development Comments:

This routine will allocate a file control block for the file or device
that we are about to open.

It will then test to see if the pathname exists. If it does a
rtems_filesystem_location_info_t data structure will be filled out. This
structure contains information that associates node information,
filesystem specific functions and mount table chain information with the
pathname.

If the create option has been it will attempt to create a node for a
regular file along the specified path. If a file already exists along this
path, an error will be generated; otherwise, a node will be allocated for
the file under the filesystem that contains the pathname. When a new node
is created, it is also evaluated so that an appropriate
rtems_filesystem_location_info_t data structure can be filled out for the
newly created node.

If the file exists or the new file was created successfully, the file
control block structure will be initialized with handler table
information, node information and the rtems_filesystem_location_info_t
data structure that describes the node and filesystem data in detail.

If an open() function exists in the filesystem specific handlers table for
the node that we are trying to open, it will be called at this time.

If any error is detected in the process, cleanup is performed. It consists
of freeing the file control block structure that was allocated at the
beginning of the generic open() routine.

On a successful open(), the index into the file descriptor table will be
calculated and returned to the calling routine.

@c @page
@section opendir

@subheading File:

opendir.c

@subheading Processing: 

This routine will attempt to open a directory for read access. It will
setup a DIR control structure that will be used to access directory
information. This routine is layered on the generic open() routine and
filesystem specific directory processing routines.

@subheading Development Comments:

The BSD group provided this routine.

@c @page
@section pathconf

@subheading File:

pathconf.c

@subheading Processing: 

This routine will obtain the value of one of the path configuration
parameters and return it to the calling routine. It is layered on the
generic open() and fpathconf()  functions. These interfaces should not
change with the addition of new filesystem types.

@subheading Development Comments:

This routine will try to open the file indicated by path.

If successful, the file descriptor will be used to access the pathconf
value specified by @code{name} using the fpathconf() function.

The file that was accessed is then closed.

@c @page
@section read

@subheading File:

deviceio.c

@subheading Processing: 

This routine is layered on a set of RTEMS calls and filesystem specific
read operations. The functions are layered in such a way as to isolate
them from change as new filesystems are introduced.

@subheading Development Comments:

This routine will examine the type of file descriptor it is sent.

If the file descriptor is associated with a network device, the read
function will be mapped to a special network handler. The return code from
the network handler will then be sent as the return code from generic
read() function.

For file descriptors that are associated with the filesystem the following
sequence will be performed:

@enumerate

@item Obtain the file control block associated with the file descriptor

@item Range check the file descriptor

@item Determine that the buffer pointer is not invalid

@item Check that the count is not zero

@item Check the file control block to see if we have permissions to read

@item If there is a read function in the handler table, invoke the handler
table read() function

@item Use the return code from the handler table read function(number of
bytes read) to increment the offset element of the file control block

@item Return the number of bytes read to the calling program

@end enumerate

@c @page
@section readdir

@subheading File:

readdir.c

@subheading Processing:

This routine was acquired from the BSD group. It has not been altered from
its original form.

@subheading Development Comments:

The routine calls a customized getdents() function that is provided by the
user.  This routine provides the filesystem specific aspects of reading a
directory.

It is layered on the read() function in the directory handler table. This
function has been mapped to the Imfs_dir_read() function.

@c @page
@section unmount

@subheading File:

unmount.c

@subheading Processing: 

This routine will attempt to dismount a mounted filesystem and then free
all resources that were allocated for the management of that filesystem.

@subheading Development Comments:

@itemize @bullet

@item This routine will determine if there are any filesystems currently
mounted under the filesystem that we are trying to dismount. This would
prevent the dismount of the filesystem.

@item It will test to see if the current directory is in the filesystem
that we are attempting to dismount. This would prevent the dismount of the
filesystem.

@item It will scan all the currently open file descriptors to determine is
there is an open file descriptor to a file in the filesystem that we are
attempting to unmount().

@end itemize

If the above preconditions are met then the following sequence is
performed:

@enumerate

@item Call the filesystem specific unmount() function for the filesystem
that contains the mount point. This routine should indicate that the mount
point no longer has a filesystem mounted below it.

@item Call the filesystem specific fsunmount_me() function for the mounted
filesystem that we are trying to unmount(). This routine should clean up
any resources that are no longer needed for the management of the file
system being un-mounted.

@item Extract the mount table entry for the filesystem that was just
dismounted from the mount table chain.

@item Free the memory associated with the extracted mount table entry.

@end enumerate

@c @page
@section eval

@subheading File:

XXX

@subheading Processing: 

XXX

@subheading Development Comments:

XXX

@c @page
@section getdentsc

@subheading File:

XXX

@subheading Processing: 

XXX

@subheading Development Comments:

XXX

