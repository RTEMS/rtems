@c

@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c


@chapter Applications and Functional Layering

@section General

The RTEMS file system framework was intended to be compliant with the POSIX 
Files and Directories interface standard. The following file system characteristics 
resulted in a functional switching layer (See figures 6 and 7).


@example
Figure 6
@end example

@enumerate

@item Application programs are presented with a standard set of POSIX compliant 
functions that allow them to interface with the files, devices and directories in the 
file system. The interfaces to these routines do not reflect the type of subordinate 
file system implementation in which the file will be found. 

@item The file system framework developed under RTEMS allows for mounting file 
systems of different types under the base file system.( Figure 3 ) 

@item The mechanics of locating file information may be quite different between file 
system types.

@item The process of locating a file may require crossing file system boundaries. 

@item The transitions between file systems and the processing required to access 
information in different file systems is not visible at the level of the POSIX 
function call.

@item The POSIX interface standard provides file access by character pathname to the 
file in some functions and through an integer file descriptor (Figure 8) in other 
functions. 

@item The nature of the integer file descriptor and its associated processing is operating 
system and file system specific.

@item Directory and device information must be processed with some of the same 
routines that apply to files. 

@item The form and content of directory and device information differs greatly from that 
of a regular file. 

@item Files, directories and devices represent elements (nodes) of a tree hierarchy. 

@item The rules for processing each of the node types that exist under the file system are 
node specific but are still not reflected in the POSIX interface routines.

@end enumerate


@example
Figure 7
@end example





@example
Figure 8
@end example






@section  Mapping of Generic System Calls to File System Specific OP Table or Handler Functions

@subsection Generic routines (open (), read (), write (), close (), . )

The Files and Directories section of the POSIX Application Programs Interface 
specifies a set of functions with calling arguments that are used to gain access to the 
information in a file system. To the application program, these functions allow access 
to information in any mounted file system without explicit knowledge of the file 
system type or the file system mount configuration. The following are functions that 
are provided to the application:

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

The file system's type as well as the node type within the file system determine the 
nature of the processing that must be performed for each of the functions above. The 
RTEMS file system provides a framework that allows new file systems to be 
developed and integrated without alteration to the basic framework.  

? Use of function pointers for functional redirection
To provide the functional switching that is required, each of the POSIX file and 
directory functions have been implemented as a shell function. The shell function 
adheres to the POSIX interface standard. Within this functional shell, file system and 
node type information is accessed which is then used to invoke the appropriate file 
system and node type specific routine to process the POSIX function call. 

? File/Device/Directory function access via file control block - rtems_libio_t 
structure
The POSIX open() function returns an integer file descriptor that is used as a 
reference to file control block information for a specific file. The file control block 
contains information that is used to locate node, file system, mount table and 
functional handler information. The diagram in Figure 8 depicts the relationship 
between and among the following components.

1. File Descriptor Table - This is an internal RTEMS structure that tracks all 
currently defined file descriptors in the system. The index that is returned by 
the file open() operation references a slot in this table. The slot contains a 
pointer to the file descriptor table entry for this file. The rtems_libio_t 
structure represents the file control block.
2. Allocation of entry in the File Descriptor Table - Access to the file descriptor 
table is controlled through a semaphore that is implemented using the 
rtems_libio_allocate() function. This routine will grab a semaphore and then 
scan the file control blocks to determine which slot is free for use. The first 
free slot is marked as used and the index to this slot is returned as the file 
descriptor for the open () request. After the alterations have been made to the 
file control block table, the semaphore is released to allow further operations 
on the table.
3. Maximum number of entries in the file descriptor table is configurable 
through the src/exec/sapi/headers/confdefs.h file. If the 
CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS constant is 
defined its value will represent the maximum number of file descriptors that 
are allowed.                                                   
If CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS is not 
specified a default value of 20 will be used as the maximum number of file 
descriptors allowed.
4. File control block - rtems_libio_t structure
struct rtems_libio_tt @{
    rtems_driver_name_t            *driver;
    off_t                                   size;      /* size of file */
    off_t                                   offset;    /* current offset into file */
    unsigned32                            flags;
    rtems_filesystem_location_info_t    pathinfo;
    Objects_Id                            sem;
    unsigned32                            data0;   /* private to "driver" */
    void                                   data1;     / . */
    void                                   file_info; /used by file handlers/
    rtems_filesystem_file_handlers_r   handlers; /type specific handlers/
@};

A file control block can exist for regular files, devices and directories. The 
following fields are important for regular file and directory access:
? Size - For a file this represents the number of bytes currently stored in 
a file. For a directory this field is not filled in.
? Offset - For a file this is the byte file position index relative to the start 
of the file. For a directory this is the byte offset into a sequence of 
dirent structures.
? Pathinfo - This is a structure that provides a pointer to node 
information, OPS table functions, Handler functions and the mount 
table entry associated with this node.
? file_info - A pointer to node information that is used by Handler 
functions
? handlers - A pointer to a table of handler functions that operate on a 
file, device or directory through a file descriptor index

? File/Directory function access via rtems_filesystem_location_info_t structure

The rtems_filesystem_location_info_tt structure below provides sufficient 
information to process nodes under a mounted file system.

struct rtems_filesystem_location_info_tt
@{
    void                                         *node_access;
    rtems_filesystem_file_handlers_r         *handlers;
    rtems_filesystem_operations_table        *ops;
    rtems_filesystem_mount_table_entry_t     *mt_entry;
@};

It contains a void pointer to file system specific nodal structure, pointers to the 
OPS table for the file system that contains the node, the node type specific 
handlers for the node and a reference pointer to the mount table entry associated 
with the file system containing the node

? File System Functional Interface to Files and Directories
? Access using relative or absolute path names to file
? OP Table for File System - rtems_filesystem_operations_table
1. evalpath ()
2. evalformake ()
3. link ()
4. unlink ()
5. node_type ()
6. mknod ()
7. rmnod ()
8. chown ()
9. freenod ()
10. mount ()
11. fsmount_me ()
12. unmount ()
13. fsunmount_me ()
14. utime ()
15. eval_link ()
16. symlink ()
17. readlink ()

? Access using file handle
? Handlers for file system node types ( regular file, directories, and devices ) - 
rtems_filesystem_file_handlers_r
1. open ()
2. close ()
3. read ()
4. write ()
5. ioctl ()
6. lseek ()
7. fstat ()
8. fchmod ()
9. ftruncate ()
10. fpathconf ()
11. fsync ()
12. fdatasync ()



? POSIX Directory Access Functions

BSD/newlib has provided a set of POSIX directory access routines. These routines 
allow directories to be open and the entries under the directory read. The getdents 
routine allows for customization of the BSD routines to a particular file system 
implementation. Some of the original BSD routines were modified, but maintain the 
same calling interface as the original BSD/newlib routine.
The following directory access routines are included in the BSD set:

? opendir ()closedir ()
? readdir () 
? getdents ()
? closedir () 
? ***   rewinddir ()
? scandir () (built on fstat () )
? ***   seekdir () 
? telldir ()

*** Not the original BSD stuff


Jennifer will fill this section in.


