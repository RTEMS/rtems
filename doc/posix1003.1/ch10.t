@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Data Interchange Format

@section Archive/Interchange File Format

@subsection Extended tar Format

@example
tar format, Type, Unimplemented
TMAGIC, Constant, 
TMAGLEN, Constant, 
TVERSION, Constant, 
TVERSLEN, Constant, 
REGTYPE, Constant, 
AREGTYPE, Constant, 
LNKTYPE, Constant, 
SYMTYPE, Constant, 
CHRTYPE, Constant, 
BLKTYPE, Constant, 
DIRTYPE, Constant, 
FIFOTYPE, Constant, 
CONTTYPE, Constant, 
TSUID, Constant, 
TSGID, Constant, 
TSVTX, Constant, 
TUREAD, Constant, 
TUWRITE, Constant, 
TUEXEC, Constant, 
TGREAD, Constant, 
TGWRITE, Constant, 
TGEXEC, Constant, 
TOREAD, Constant, 
TOWRITE, Constant, 
TOEXEC, Constant, 
@end example

NOTE: Requires <tar.h> which is not in newlib.

@subsection Extended cpio Format

@example
cpio format, Type, Unimplemented
C_IRUSER, Constant, 
C_IWUSER, Constant, 
C_IXUSER, Constant, 
C_IRGRP, Constant, 
C_IWGRP, Constant, 
C_IXGRP, Constant, 
C_IROTH, Constant, 
C_IWOTH, Constant, 
C_IXOTH, Constant, 
C_ISUID, Constant, 
C_ISGID, Constant, 
C_ISVTX, Constant, 
@end example

NOTE: POSIX does not require a header file or structure.  RedHat Linux
5.0 does not have a <cpio.h> although Solaris 2.6 does.

@subsection Multiple Volumes

