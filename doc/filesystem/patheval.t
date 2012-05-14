@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Pathname Evaluation

This chapter describes the pathname evaluation process for the 
RTEMS Filesystem Infrastructure.

@example
XXX Include graphic of the path evaluation process 
@end example

@section Pathname Evaluation Handlers

There are two pathname evaluation routines.  The handler patheval()
is called to find, verify privlages on and return information on a node
that exists.  The handler evalformake() is called to find, verify
permissions, and return information on a node that is to become a parent.  
Additionally, evalformake() returns a pointer to the start of the name of
the new node to be created.

Pathname evaluation is specific to a filesystem.  
Each filesystem is required to provide both a patheval() and an evalformake() 
routine.  Both of these routines gets a name to evaluate and a node indicating
where to start the evaluation.  

@section Crossing a Mount Point During Path Evaluation

If the filesystem supports the mount command, the evaluate routines
must handle crossing the mountpoint.  The evaluate routine should evaluate
the name upto the first directory node where the new filesystem is mounted.  
The filesystem may process terminator characters prior to calling the
evaluate routine for the new filesystem.   A pointer to the portion of the
name which has not been evaluated along with the root node of the new 
file system ( gotten from the mount table entry ) is passed to the correct 
mounted filesystem evaluate routine.


@section The rtems_filesystem_location_info_t Structure

The @code{rtems_filesystem_location_info_t} structure contains all information 
necessary for identification of a node.  

The generic rtems filesystem code defines two global 
rtems_filesystem_location_info_t structures, the 
@code{rtems_filesystem_root} and the @code{rtems_filesystem_current}.
Both are initially defined to be the root node of the base filesystem.
Once the chdir command is correctly used the @code{rtems_filesystem_current}
is set to the location specified by the command.

The filesystem generic code peeks at the first character in the name to be
evaluated.  If this character is a valid seperator, the 
@code{rtems_filesystem_root} is used as the node to start the evaluation 
with.  Otherwise, the @code{rtems_filesystem_current} node is used as the 
node to start evaluating with.  Therefore, a valid 
rtems_filesystem_location_info_t is given to the evaluate routine to start
evaluation with.  The evaluate routines are then responsible for making
any changes necessary to this structure to correspond to the name being
parsed.

@example
struct rtems_filesystem_location_info_tt @{
    void                                     *node_access;
    rtems_filesystem_file_handlers_r         *handlers;
    rtems_filesystem_operations_table        *ops;
    rtems_filesystem_mount_table_entry_t     *mt_entry;
@};
@end example

@table @b

@item node_access
This element is filesystem specific.  A filesystem can define and store
any information necessary to identify a node at this location.  This element
is normally filled in by the filesystem's evaluate routine. For the 
filesystem's root node, the filesystem's initilization routine should
fill this in, and it should remain valid until the instance of the
filesystem is unmounted.

@item handlers
This element is defined as a set of routines that may change within a
given filesystem based upon node type.  For example a directory and a
memory file may have to completely different read routines.  This element
is set to an initialization state defined by the mount table, and may
be set to the desired state by the evaluation routines.

@item ops
This element is defined as a set of routines that remain static for the
filesystem.  This element identifies entry points into the filesystem 
to the generic code.

@item mt_entry
This element identifies the mount table entry for this instance of the
filesystem.

@end table








