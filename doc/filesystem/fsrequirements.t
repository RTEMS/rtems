@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c


@chapter Filesystem Requirements

This chapter details the behavioral requirements that all filesystem
implementations must adhere to.

@section Filesystem File and Directory Removal Constraints

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

