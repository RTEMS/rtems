libfs
=====

This directory contains for the "file system" library. All supported
file systems live under this tree.

Currently the supported file systems in this library are:

- IMFS or In Memory File System

   This is the only root file system on RTEMS at the moment. It supports
   files, directories, device nodes and mount points. It can also be
   configured to be the miniIMFS.

- TFTP and FTP filesystem are part of the libnetworking library.

- DEVFS or Device File system

- DOSFS, a FAT 12/16/32 MSDOS compatible file system.

- NFS Client, can mount NFS exported file systems.

- PIPE, a pipe file system.

- RFS, The RTEMS File System.

--Chris Johns and Joel Sherrill
  17 Feb 2010 
