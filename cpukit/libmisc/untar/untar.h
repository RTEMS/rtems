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

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UNTAR_SUCCESSFUL         0
#define UNTAR_FAIL               1
#define UNTAR_INVALID_CHECKSUM   2
#define UNTAR_INVALID_HEADER     3


int Untar_FromMemory(char *tar_buf, size_t size);
int Untar_FromFile(char *tar_name);

#ifdef __cplusplus
}
#endif

#endif  /* __UNTAR_H__ */
