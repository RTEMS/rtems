/*
 * File Transfer Protocol client declarations
 *
 * Transfer file to/from remote host
 * 
 *
 * (c) Copyright 2002
 * Thomas Doerfler
 * IMD Ingenieurbuero fuer Microcomputertechnik
 * Herbststr. 8
 * 82178 Puchheim, Germany 
 * <Thomas.Doerfler@imd-systems.de>
 *
 * This code has been created after closly inspecting 
 * "tftpdriver.c" from Eric Norum.
 * 
 *  $Id$
 */

#ifndef _FTP_DRIVER_H
#define _FTP_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

  /* create mount point and mount ftp file system */
  int rtems_bsdnet_initialize_ftp_filesystem () ;

#ifdef __cplusplus
}
#endif

#endif
