@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Configuration Space Manager

@section Introduction

The 
configuration space manager is ...

The directives provided by the configuration space manager are:

@itemize @bullet
@item @code{cfg_mount} - Mount a Configuration Space
@item @code{cfg_unmount} - Unmount a Configuration Space
@item @code{cfg_mknod} - Create a Configuration Node
@item @code{cfg_get} - Get Configuration Node Value
@item @code{cfg_set} - Set Configuration Node Value
@item @code{cfg_link} - Create a Configuration Link
@item @code{cfg_unlink} - Remove a Configuration Link
@item @code{cfg_open} - Open a Configuration Space
@item @code{cfg_read} - Read a Configuration Space
@item @code{cfg_children} - Get Node Entries
@item @code{cfg_mark} - Set Configuration Space Option
@item @code{cfg_close} - Close a Configuration Space
@end itemize

@section Background

@section Operations

@section Directives

This section details the configuration space manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection cfg_mount - Mount a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_mount(
  const char     *file,
  const char     *cfgpath,
  log_facility_t  notification,
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EPERM
The caller does not have the appropriate privilege.
@item EACCES
Search permission is denied for a component of the path prefix.
@item EEXIST
The file specified by the file argument does not exist
@item ENAMETOOLONG
A component of a pathname exceeded {NAME_MAX} characters,
or an entire path name exceed {PATH_MAX} characters while
{_POSIX_NO_TRUNC} is in effect.
@item ENOENT
A component of cfgpath does not exist.
@item ENOTDIR
A component of the file path prefix is not a directory.
@item EBUSY
The configuration space defined by file is already mounted.
@item EINVAL
The notification argument specifies an invalid log facility.
@item ENOSYS
The cfg_mount() function is not supported by this implementation.

@end table

@subheading DESCRIPTION:
The @code{cfg_mount} function maps a configuration space defined
by the file identified by the the @code{file} argument. 

@subheading NOTES:

@page
@subsection cfg_unmount - Unmount a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_unmount(
  const char     *cfgpath
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EPERM
The caller does not have the appropriate privileges.
@item EACCES
Search permission is denied for a component of the path prefix.
@item ENOENT
A component of cfgpath does not exist.
@item ENAMETOOLONG
A component of a pathname exceeded {NAME_MAX} characters,
or an entire path name exceed {PATH_MAX} characters while
{_POSIX_NO_TRUNC} is in effect.
@item EINVAL
The requested node is not the distinguished node of a mounted
configuration space.
@item EBUSY
One or more processes has an open configuration traversal
stream for the configuration space whose distinguished node is
referenced by the cfgpath argument.
@item ENOSYS
The cfg_umount function is not supported by this implementation.
@item ELOOP
A node appears more than once in the path specified by the 
cfg_path argument
@item ELOOP
More than {SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument

@end table

@subheading DESCRIPTION:

The @code{cfg_umount} function unmaps the configuration space whose 
distinguished node is mapped in the active space at the location defined
by @code{cfgpatah} configuration pathname.

@subheading NOTES:

@page
@subsection cfg_mknod - Create a Configuratioin Node

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_mknod(
  const char   *cfgpath,
  mode_t        mode,
  cfg_type_t    type         
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded {NAME_MAX} characters,
or an entire path name exceed {PATH_MAX} characters while
{_POSIX_NO_TRUNC} is in effect.
@item ENOENT
A compent of the path prefix does not exist.
@item EACCES
Search permission is denied for a component of the path prefix.
@item ELOOP
Too many symbolic links were encountered in translating the 
pathname.
@item EPERM
The calling process does not have the appropriate privilege.
@item EEXIST
The named node exists.
@item EINVAL
The value of mode is invalid.
@item EINVAL
The value of type is invalid.
@item ENOSYS
The function cfg_mknod() is not supported by this implementation.
@item ELOOP
A node appears more than once in the path specified by the 
cfg_path argument
@item ELOOP
More than {SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.
@item EROFS
The named node resides on a read-only configuration space.

@end table

@subheading DESCRIPTION:

The @code{cfg_mknod} function creates a new node in the configuration
space which contains the pathname prefix of @cod{cfgpath}.

@subheading NOTES:

@page
@subsection cfg_get - Get Configuration Node Value

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_get(
  const char  *cfgpath
  cfg_value_t *value
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded {NAME_MAX} characters,
or an entire path name exceed {PATH_MAX} characters while
{_POSIX_NO_TRUNC} is in effect.
@item ENOENT
A component of cfgpath does not exist.
@item EACCES
Search permission is denied for a component of the path prefix.
@item EPERM
The calling process does not have the appropriate priviledges.
@item ENOSYS
The function cfg_get() is not supported by this implementation
@item ELOOP
A node appears more than once in the path specified by the 
cfg_path argument
@item ELOOP
More than {SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

The @code{cfg_get} function stores the value attribute of the
configuration node identified by @code{cfgpath}, into the buffer
described by the value pointer.

@subheading NOTES:

@page
@subsection cfg_set - Set Configuration Node Value

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_set(
  const char  *cfgpath
  cfg_value_t *value
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded {NAME_MAX} characters,
or an entire path name exceed {PATH_MAX} characters while
{_POSIX_NO_TRUNC} is in effect.
@item ENOENT
A component of cfgpath does not exist
@item EACCES
Search permission is denied for a component of the path prefix.
@item EPERM
The calling process does not have the appropriate privilege.
@item ENOSYS
The function cfg_set() is not supported by this implementation.
@item ELOOP
A node appears more than once in the path specified by the
cfg-path argument.
@item ELOOP
More than {SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

The @code{cfg_set} function stores the value specified by the
@code{value} argument in the configuration node defined by the 
@code{cfgpath} argument

@subheading NOTES:

@page
@subsection cfg_link - Create a Configuration Link

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_link(
  const char *src
  const char *dest
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded {NAME_MAX} characters,
or an entire path name exceed {PATH_MAX} characters while
{_POSIX_NO_TRUNC} is in effect.
@item ENOENT
A component of either path prefix does not exist.
@item EACCES
A component of either path prefix denies search permission.
@item EACCES
The requested link requires writing in a node with a mode that
denies write permission.
@item ENOENT
The node named by src does not exist.
@item EEXIST
The node named by dest does exist.
@item EPERM
The calling process does not have the appropriate privilege to
modify the node indicated by the src argument.
@item EXDEV
The link named by dest and the node named by src are from different
configuration spaces.
@item ENOSPC
The node in which the entry for the new link is boeing placed
cannot be extended because there is no space left on the 
configuration space containing the node.
@item EIO
An I/O error occurred while reading from or writing to the 
configuration space to make the link entry.
@item EROFS
The requested link requires writing in a node on a read-only
configuration space.
@item ENOSYS
The function cfg_link() is not supported by this implementation. 
@item ELOOP
A node appears more than once in the path specified by the
cfg-path argument.
@item ELOOP
More than {SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

The @code{cfg_link} function shall atomically create a link between 
specified nodes, and increment by one the link count of the node
specified by the @code{src} argument.  The @code{src} and @code{dest}
arguments point to pathnnames which name existing nodes.

@subheading NOTES:

@page
@subsection cfg_unlink - Remove a Configuration Link

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_unlink(
  const char    *cfgpath
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item ENAMETOOLONG
A component of a pathname exceeded {NAME_MAX} characters,
or an entire path name exceed {PATH_MAX} characters.
@item ENOENT
The named  node does not exist.
@item EACCES
Search permission is denied on the node containing the link to
be removed.
@item EACCES
Write permission is denied on the node containing the link to 
be removed.
@item ENOENT
A component of cfgpath does not exist.
@item EPERM
The calling process does not have the appropriate priviledge to 
modify the node indicated by the path prefix of the cfgpath
argument.
@item EBUSY
The node to be unlinked is the distinguished node of a mounted
configuration space.
@item EIO
An I/O error occurred while deleting the link entry or deallocating
the node.
@item EROFS
The named node resides in a read-opnly configuration space.
@item ENOSYS
The function cfg_unlink() is not supported by this implementation.
@item ELOOP
A node appears more than once in the path specified by the
cfg-path argument.
@item ELOOP
More than {SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

The @code{cfg_unlink} function  removes the link between the node
specified by the @code{cfgpath} path prefix and the parent node 
specified by @code{cfgpaht}, and shall decrement the link count 
of the @code{cfgpath} node.

@subheading NOTES:

@page
@subsection cfg_open - Open a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_open(
  const char     *pathnames[],
  int             options,
  int           (*compar)(const CFGENT **f1, const CFGENT **f2),
  CFG           **cfgsrteam
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for any component of a pathname.
@item ELOOP
A loop exists in symbolic links encountered during resolution 
of a pathname.
@item ENAMETOOLONG
The length of a pathname exceeds {PATH_MAX}, or a pathname
component is longer than {NAME_MAX} while {_POSIX_NO_TRUNC}
@item ENOENT
The pathname argument is an empty string or the named node
does not exist.
@item EINVAL
Either both or neither of CFG_LOGICAL and CFG_PHYSICAL are
specified by the options argument ???????????
@item ENOMEM
Not enough memory is available to create the necessary structures.
@item ENOSYS
The function cfg_open() is not supported by this implementation.
@item ELOOP
More than {SYMLOOP_MAX} symbolic links were encountered during
resolution of the pathnames argument.
@item ENAMETOOLONG
As a result of encountering a symbolic link in resolution of the
pathname specified by the pathnames argument, the lenght of
the substituted pathname string exceeded {PATH_MAX}.

@end table

@subheading DESCRIPTION:

The @code{cfg_open} function shall open a configuration traversal stream
rooted in the configuration nodes name by the @code{pathnames} argument.

@subheading NOTES:

@page
@subsection cfg_read - Read a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_read(  
  CFG           *cfgp,
  CFGENT       **node
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for any component of a pathname.
@item EBADF
The cfgp argument does not refer to an open configuration
space.
@item ELOOP
A loop exists in symbolic links encountered during resolution
of a pathname.
@item ENOENT
A named node does not exist.
@item ENOMEM
Not enough memory is available to create the necessary structures.
@item ENOSYS
The function cfg_read() is not suported by this implementation.
@item ELOOP
More than {SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.
@item ENAMETOOLONG
As aresult of encountering a symbolic link in resolution of the
pathname specified by the pathnames argument, the length of the
substituted pathname string exceeded {PATH_MATH}.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_children - Get Node Entries

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_children(
  CFG           *cfgp,
  int            options,
  CFGENT       **children
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EACCES
Search permission is denied for any component of a pathname
@item EBADF
The cfgp argument does not refer to an open configuration space.
@item ELOOP
A loop exists in symbolic links encountered during resolution of 
a pathname.
@item ENAMETOOLONG
The length of a pathname exceeds {PATH_MAX}, or a pathname
component is longer than {NAME_MAX} while {_POSIX_NO_TRUNC} is
in effect.
@item EINVAL
The specified value of the optiions argument is invalid.
@item ENOENT
The named node does not exist.
@item ENOMEM
Not enough memory is available to create the necessary structures.
@item ENOSYS
The function cfg_children() is not supported by this implementation.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection cfg_mark - Set Configuration Space Options

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_mark(
  CFG           *cfgp,
  CFGENT        *f,
  int            options
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EINVAL
The specified combination of the cfgp and f arguments is not 
supported by the implementation.
@item EINVAL
The specified value of the options argument is invalid.
@item ENOSYS
The function cfg_mark() is not supported by this implementation.

@end table

@subheading DESCRIPTION:

The @code{cfg_mark} function modifies the subsequent behavior of
the cfg functions with regard to the node referenced by the structure
pointed to by the argument f or the configuration space referenced by
the structure pointed to by the argument @code{cfgp}.

@subheading NOTES:

@page
@subsection cfg_close - Close a Configuration Space

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cfg_close(
  CFG           *cfgp
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item EBADF
The cfgp argument does not refer to an open configuration space
traversal stream.
@item ENOSYS
The function cfg_close() is not supported by this implementatioin.

@end table

@subheading DESCRIPTION:

The @code{cfg_close} function closes a configuration space transversal
stream.

@subheading NOTES:

