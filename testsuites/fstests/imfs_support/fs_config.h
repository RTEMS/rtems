/*
 *  $Id$
 */
#ifndef __IMFS_SUPPORT_h
#define __IMFS_SUPPORT_h

#define IMFS_TEST

#define FILESYSTEM "IMFS"
#define BASE_FOR_TEST "/mnt/"

#ifdef __cplusplus
extern "C" {
#endif

extern void test_initialize_filesystem(void);

extern void test_shutdown_filesystem(void);

#ifdef __cplusplus
};
#endif

#endif
