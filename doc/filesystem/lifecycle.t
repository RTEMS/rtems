@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter File System Lifecycle

This chapter describes some of the dynamic behavior of file system control
structures

@section Mount table chain

The mount table chain is a dynamic list of structures that describe
mounted file systems a specific points in the file system hierarchy. It is
initialized to an empty state during the base file system initialization.
The mount operation will add entries to the mount table chain. The
un-mount operation will remove entries from the mount table chain.

@section Memory associated with the In Memory File System

A memory based file system draws its resources for files and directories
from the memory resources of the system. When it is time to un-mount the
file system, the memory resources that supported file system are set free.
In order to free these resources, a recursive walk of the file systems
tree structure will be performed. As the leaf nodes under the file system
are encountered their resources are freed. When directories are made empty
by this process, their resources are freed.

@section File descriptor table entries and file control blocks

The file: XXX

