/*
 *  Written by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __UNTAR_H__
#define __UNTAR_H__


#define UNTAR_SUCCESSFUL         0
#define UNTAR_FAIL               1
#define UNTAR_INVALID_CHECKSUM   2
#define UNTAR_INVALID_HEADER     3


int Untar_FromMemory(unsigned char *tar_buf, unsigned long size);
int Untar_FromFile(char *tar_name);


#endif  /* __UNTAR_H__ */
