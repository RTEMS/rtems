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
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.
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

If @code{_POSIX_CFG} is defined:

   The @code{cfg_mount} function maps a configuration space defined
   by the file identified by the the @code{file} argument.  The 
   distinguished node of the mapped configuration space shall be
   mounted in the active space at the point identified bt the
   @code{cfgpath} configuration pathname.

   The @code{notification} argument specifies how changes to the
   mapped configuration space shall be communicated to the application.
   If the @code{notification} argument is NULL, no notification shall
   be performed for the mapped configuration space.  If the Event
   Logging option is defined, the notification argument defines the
   facility to which changes in the mapped configuration space shall
   be logged.  Otherwise, the @code{notification} argument shall
   specify an implementation defined method of notifying the application
   of changes to the mapped configuration space.

Otherwise:

   The @code{cfg_mount} function shall fail.

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
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.
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
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument

@end table

@subheading DESCRIPTION:

If @code{_POSIX_CFG} is defined:

   The @code{cfg_umount} function unmaps the configuration space whose 
   distinguished node is mapped in the active space at the location defined
   by @code{cfgpatah} configuration pathname.  All system resources 
   allocated for this configuration space should be deallocated.

Otherwise:

   The @code{cfg_umount} function shall fail.

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
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.
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
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.
@item EROFS
The named node resides on a read-only configuration space.

@end table

@subheading DESCRIPTION:

If @code{_POSIX_CFG} is defined:

   The @code{cfg_mknod} function creates a new node in the configuration
   space which contains the pathname prefix of @code{cfgpath}.  T he node
   name shall be defined by the pathname suffix of @code{cfgpath}.  The
   node name shall be defined by the pathname suffix of @code{cfgpath}. 
   The node permissions shall be specified by the value of @code{mode}.
   The node type shall be specified by the value of @code{type}.

Otherwise:

   The @code{cfg_mknod} function shall fail.

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
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.
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
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

If @code{_POSIX_CFG} is defined:

   The @code{cfg_get} function stores the value attribute of the
   configuration node identified by @code{cfgpath}, into the buffer
   described by the @code{value} pointer.

Otherwise:

   The @code{cfg_get} function shall fail.


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
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.
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
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

If @code{_POSIX_CFG} is defined:

   The @code{cfg_set} function stores the value specified by the
   @code{value} argument in the configuration node defined by the 
   @code{cfgpath} argument.

Otherwise:

   The @code{cfg_set} function shall fail.


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
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters while
@code{_POSIX_NO_TRUNC} is in effect.
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
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

If @code{_POSIX_CFG} is defined:

   The @code{src} and @code{dest}arguments point to pathnnames which 
   name existing nodes.  The @code{cfg_link} function shall atomically 
   create a link between specified nodes, and increment by one the link 
   count of the node specified by the @code{src} argument. 

   If the @code{cfg_lin} function fails, no link shall be created, and
   the link count of the node shall remain unchanged by this function
   call.

   This implementation may require that the calling process has permission 
   to access the specified nodes.

Otherwise:

   The @code{cfg_link} functioin shall fail.

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
A component of a pathname exceeded @code{NAME_MAX} characters,
or an entire path name exceed @code{PATH_MAX} characters.
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
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.

@end table

@subheading DESCRIPTION:

If @code{_POSIX_CFG} is defined:

   The @code{cfg_unlink} function removes the link between the node
   specified by the @code{cfgpath} path prefix and the parent node 
   specified by @code{cfgpaht}, and shall decrement the link count 
   of the @code{cfgpath} node.

   When the link count of the node becomes zero, the space occupied
   by the node shall be freed and the node shall no longer be accessible.

Otherwise:

   The @code{unlink} function shall fail.

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
The length of a pathname exceeds @code{PATH_MAX}, or a pathname
component is longer than @code{NAME_MAX} while @code{_POSIX_NO_TRUNC}
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
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the pathnames argument.
@item ENAMETOOLONG
As a result of encountering a symbolic link in resolution of the
pathname specified by the pathnames argument, the lenght of
the substituted pathname string exceeded @code{PATH_MAX}.

@end table

@subheading DESCRIPTION:

If @code{_POSIX_CFG} is defined:

   The @code{cfg_open} function shall open a configuration traversal stream
   rooted in the configuration nodes name by the @code{pathnames} argument.
   It shall store a pointer to a CFG object that represents that stream at 
   the location identified the @code{cfgstream} pointer.  The @code{pathnames}
   argument is an array of character pointers to NULL-terminated strings. 
   The last member of this array shall be a NULL pointer.

   The value of @code{options} is the bitwise inclusive OR of values from the 
   following lists.  Applications shall supply exactly one of the first two 
   values below in @code{options}.

      CFG_LOGICAL   - When symbolic links referencing existing nodes are 
                      encountered during the traversal, the @code{cfg_info}
                      field of the returned CFGENT structure shall describe
                      the target node pointed to by the link instead of the 
                      link itself, unless the target node does not exist. 
                      If the target node has children, the pre-order return,
                      followed by the return of structures referenceing all of
                      its descendants, followed by a post-order return, shall
                      be done.
                    
      CFG_PHYSICAL  - When symbolic links are encountered during the traversal,
                      the @code{cfg_info} field shall describe the symbolic 
                      link.
                    

   Any combination of the remaining flags can be specified in the value of 
   @code{options}

      CFG_COMFOLLOW - When symbolic links referencing existing nodes are
                      specified in the @code{pathnames} argument, the 
                      @code{cfg_info} field of the returned CFGENT structure
                      shall describe the target node pointed to by the link
                      instead of the link itself, unless the target node does
                      not exist.  If the target node has children, the 
                      pre-order return, followed by the return of structures
                      referencing all its descendants, followed by a post-order
                      return, shall be done.

      CFG_XDEV      - The configuration space functions shall not return a
                      CFGENT structure for any node in a different configuration
                      space than the configuration spacce of the nodes identified 
                      by the CFGENT structures for the @code{pathnames} argument.

   The @code{cfg_open} argument @code{compar} shall either be NULL or point
   to a function that shall be called with two pointers to pointers to CFGENT 
   structures that shall return less than, equal to , or greater than zero if 
   the node referenced by the first argument is considered to be respectively
   less than, equal to, or greater than the node referenced by the second.
   The CFGENT structure fields provided to the comparison routine shall be as 
   described with the exception that the contents of the @code{cfg_path} and
   @code{cfg_pathlen} fields are unspecified.

   This comparison routine is used to determine the order in which nodes in
   directories encountered during the traversal are returned, and the order
   of traversal when more than one node is specified in the @code{pathnames}
   argument to @code{cfg_open}.  If a comparison routine is specified, the
   order of traversal shall be from the least to the greatest.  If the 
   @code{compar} argument is NULL, the order of traversal shall be as listed
   in the @code{pathnames} argument. 

Otherwise:

   The @code{cfg_open} shall fail.

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
More than @code{SYMLOOP_MAX} symbolic links were encountered during
resolution of the cfgpath argument.
@item ENAMETOOLONG
As aresult of encountering a symbolic link in resolution of the
pathname specified by the pathnames argument, the length of the
substituted pathname string exceeded @code{PATH_MATH}.

@end table

@subheading DESCRIPTION:

if @code{_POSIX_CFG} is defined:

   The @code{cfg_read} function returns a pointer to a CFGENT sturcture
   representing a node in the configuration space to which @code{cfgp}
   refers.  The returned pointer shall be stored at the location 
   indicated by the @code{node} argument.

   The child nodes of each node in the configuration tree is returned
   by @code{cfg_read}.  If a comparison routine is specified to the
   @code{cfg_open} function, the order of return of the child nodes shall
   be as specified by the routine, from least to greatest.  Otherwise
   the order of return is unspecified.

   Structures referencing nodes with children shall be returned by the 
   function @code{cfg_read} at least twice [unless the application
   specifies otherwise with @code{cfg_mark}]-once immediately before
   the structures representing their descendants, are returned 
   (pre-order), and once immediately after structures representing all
   of their descendants, if any, are returned (post-order).  The 
   CFGENT structure returned in post-porder (with the exception of the 
   @code{cfg_info} field) shall be identical to that returned in pre-order.
   Structures referencing nodes of other types shall be returned at least
   once.

   The fields of the CFGENT structure shall contain the following 
   informatation:
      cfg_parent  - A pointer to the structure returned by the 
                    @code{cfg_read} function for the node that contains
                    the entry for the current node.  A @code{cfg_parent}
                    structure shall be provided for the node(s) specified
                    by the @code{pathnames} argument to the @code{cfg_open}
                    function, but the contents of other than its 
                    @code{cfg_number}, @code{cfg_pointer}, @code{cfg_parent},
                    and @code{cfg_parent}, and @code{cfg_level} fields are
                    unspecified.  Its @code{cfg_link} field is unspecified.
      cfg_link    - Upon return from the @code{cfg_children} function, the
                    @code{cfg_link} field points to the next CFGENT structure 
                    in a NULL-terminated linked list of CFGENT structures.  
                    Otherwise, the content of the @code{cfg_link} field is 
                    unspecified.
      cfg_cycle   - If the structure being returned by @code{cfg_read} 
                    represents a node that appears in the @code{cfg_parent}
                    linked list tree, the @code{cfg_cycle} field shall point 
                    to the structure representing that entry from the 
                    @code{cfg_parent} linked list.  Otherwise the content of
                    the @code{cfg_cycle} field is unspecified.
      cfg_number  - The @code{cfg_number} field is provided for use by the 
                    application program.  It shall be initialized to zero for 
                    each new node returned by the @code{cfg_read} function, 
                    but shall not be further modified the configuration space
                    routines.
      cfg_pointer - The @code{cfg_pointer} field is provided for use by the
                    application program.  It shall be initialized to NULL for
                    each new node returned by the @code{cfg_read} function, 
                    but shall not be further modified by the configuration 
                    space routines.
      cfg_path    - A pathname for the node including and relative to the 
                    argument supplied to the @code{cfg_open} routine for this
                    configuration space.  This pathname may be logner than
                    @code{PATH_MAX} bytes.  This patname shall be NULL-terminated.
      cfg_name    - The nodename of the node.
      cfg_pathlen - The length of the string pointed at by the @code{cfg_path}
                    field when returned by @code{cfg_read}.
      cfg_namelen - The length of the string pointed at by the @code{cfg_name}
                    field.
      cfg_level   - The depth of the current entry in the configuration space.
                    The @code{cfg_level} field of the @code{cfg_partent} 
                    structure for each of the node(s) specified in the 
                    @code{pathnames} argument to the @code{cfg_open} function
                    shall be set to 0, and this number shall be incremented for
                    for each node level descendant.
      cfg_info    - This field shall contain one of the values listed below.  If
                    an object can have more than one info value, the first 
                    appropriate value listed below shall be returned. 

                    CFG_D       - The structure represents a node with children in
                                  pre-order.
                    CFG_DC      - The structure represents a node that is a parent
                                  of the node most recently returned by @code{cfg_read}.
                                  The @code{cfg_cycle} field shall reference the 
                                  structure previously returned by @code{cfg_read} that
                                  is the same as the returned structure.
                    CFG_DEFAULT - The structure represents a node that is not 
                                  represented by one of the other node types
                    CFG_DNR     - The structure represents a node, not of type symlink,
                                  that is unreadable.   The variable @code{cfg_errno}
                                  shall be set to the appropriate value.
                    CFG_DP      - The structure represents a node with children in
                                  post-order.  This value shall occur only if CFG_D 
                                  has previously been returned for this entry.
                    CFG_ERR     - The structure represents a node for which an error has 
                                  occurred.  The variable @code{cfg_errno} shall be set
                                  to the appropriate value.
                    CFG_F       - The structure represents a node without children.
                    CFG_SL      - The structure represents a node of type symbolic link.
                    CFG_SLNONET - The structure represents a node of type symbolic link
                                  with a target node for which node characteristic
                                  information cannot be obtained.

   Structurres returned by @code{cfg_read} with a @code{cfg_info} field equal to CFG_D
   shall be accessible until a subsequent call, on the same configuration traversal 
   stream, to @code{cfg_close}, or to @code{cfg_read} after they have been returned by
   the @code{cfg_read} function in post-order.  Structures returnded by @code{cfg_read}
   with an @code{cfg_info} field not equal to CFG_D shall be accessible until a 
   subsequent call, on the same configuration traversal stream, to @code{cfg_close} or
   @code{cfg_read}.

   The content of the @code{cfg_path} field is specified only for the structure most
   recently returned by @code{cfg_read}.

   The specified fields in structures in the list representing nodes for which structures
   have previously been returned by @code{cfg_children}, shall be identical to those 
   returned by @code{cfg_children}, except that the contents of the @code{cfg_path} and
   @code{cfg_pathlen} fields are unspecified.
         
Otherwise:

   The @code{cfg_read} function shall fail.

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
The length of a pathname exceeds @code{PATH_MAX}, or a pathname
component is longer than @code{NAME_MAX} while @code{_POSIX_NO_TRUNC} is
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

If @code{_POSIX_CFG} is defined:

   The first @code{cfg_children} call after a @code{cfg_read} shall 
   return information about the first node without children under the
   node returned by @code{cfg_read}.  Subsequent calls to 
   @code{cfg_children} without the intervening @code{cfg_read} shall
   return information about the remaining nodes without children under
   that same node.

   If @code{cfg_read} has not yet been called for the configuration
   traversal stream represented by @code{cfgp}, @code{cfg_children}
   shall return a pointer to the first entry in a list of the nodes 
   represented by the @code{pathnames} argument to @code{cfg_open}.

   In either case, the list shall be NULL-terminated, ordered by the 
   user-specified comparison function, if any, and linked through the
   cfg_link field.

Otherwise:

   The @code{cfg_children} function shall fail.

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

If @code{_POSIX_CF} is defined:

   The @code{cfg_mark} function modifies the subsequent behavior of
   the cfg functions with regard to the node referenced by the structure
   pointed to by the argument @code{f} or the configuration space referenced 
   by the structure pointed to by the argument @code{cfgp}.

   Exactly one of the @code{f} argument and the @code{cfgp} argument shall
   be NULL.

   The value of the @code{options} argument shall be exactly one of the
   flags specified in the following list: 

      CFG_AGAIN  - If the @code{cfgp} argument is non-NULL, or the @code{f}
                   argument is NULL, or the structure referenced by @code{f}
                   is not the one most recently returned by @code{cfg_read},
                   @code{cfg_mark} ahall return an error.  Otherwise, the next 
                   call to teh @code{cfg_read} function shall return the 
                   structure referenced by @code{f} with the @code{cfg_info}
                   field reinitialized.  Subsequent behavior of the @code{cfg}
                   functions shall be based on the reinitialized value of 
                   @code{cfg_ingo}.

      CFG_SKIP   - If the @code{cfgp} argument is non-NULL, or the @code{f}
                   argument is NULL, or the structure referenced by @code{f}
                   is not one of those specified as accessible, or the structure
                   referenced by @code{f} is not for a node of type pre-order 
                   node, @code{cfg_mark} shall return an error.  Otherwise, no 
                   more structures for the node referenced by @code{f} or its 
                   descendants shall be returned by the @code{cfg_read} function.

      CFG_FOLLOW - If the @code{cfgp} argument is non-NULL, or the @code{f} 
                   argument is NULL, or the structure referenced by @code{f}
                   is not one of those specified as accessible, or the structure
                   referenced by @code{f} is not for a node of type symbolic link,
                   @code{cfg_mark} shall return an error.  Otherwise, the next
                   call to the @code{cfg_read} function shall return the structure
                   referenced by @code{f} with the @code{cfg_info} field reset
                   to reflect the target of the symbolic link instead of the 
                   symbolic link itself.  If the target of the link is node with
                   children, the pre-order return, followed by the return of 
                   structures referencing all of its descendants, followed by a 
                   post-order return, shall be don.

                   If the target of the symbolic link does not exist, the fields
                   of the structure by @code{cfg_read} shall be unmodified, except
                   that the @code{cfg_info} field shall be reset to @code{CFG_SLNONE}.

Otherwise:
   
   The @code{cfg_mark} function shall fail.

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

if @code{_POSIX_CFG} is defined:

   The @code{cfg_close} function closes a configuration space transversal
   stream represented by the CFG structure pointed at by the @code{cfgp}
   argument.  All system resources allocated for this configuration space
   travsversal stream should be deallocated.  Upon return, the value of 
   @code{cfgp} need not point to an accessible object of type CFG.

Otherwise:

   The @code{cfg_close} function shall fail.

@subheading NOTES:

