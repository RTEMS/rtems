umon
====

#  Fernando Nicodemos <fgnicodemos@terra.com.br>
#  from NCB - Sistemas Embarcados Ltda. (Brazil)
#
#  Joel Sherill
#  from OAR Corporation
#

This directory contains support for utilitizing MicroMonitor
(http://www.umonfw.com/) capabilities from within an RTEMS
application.  This directory contiains:

+ "MonLib" functionality as documented in the MicroMonitor
  User's Manual.
+ TFS filesystem which makes the MicroMonitor TFS filesystem
  available under RTEMS as a regular filesystem.

Usage
-----

For any of this functionality to work, the application is
responsible for connecting the library to the monitor.
This is done by calling rtems_umon_connect() early in the
application.  This routine assumes that the BSP has provided
the routine rtems_bsp_get_umon_monptr() which returns the
value referred to as MONCOMPTR by MicroMonitor.

To use the TFS filesystem, it is necessary to mount it
by calling the rtems_initialize_tfs_filesystem() routine
and providing it the name of the mount point directory.


CONFIGURATION
-------------
The TFS filesystem uses a single Classic API Semaphore.

The monlib functionality will eventually also use a single
Classic API Semaphore.


STATUS
------

+ Limited testing -- especially of TFS RTEMS filesystem.
+ monlib is NOT currently protected by a mutex.


SOURCE ORIGIN
-------------
Some of the files in this directory are included in the
MicroMonitor distribution and may need to be updated
in the future.  

12 June 2009: Source is from umon 1.17
